#include "SubsurfaceScatteringPass.hpp"

#include "Engine.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Castor3DPrerequisites.hpp"
#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"
#include "Shader/Shaders/GlslSssTransmittance.hpp"
#include "Shader/Ubos/GpInfoUbo.hpp"
#include "Shader/Ubos/SceneUbo.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Buffer/VertexBuffer.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassState.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include <random>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		glsl::Shader doGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ) );
			auto texture = writer.declAttribute< Vec2 >( cuT( "texcoord" ) );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = texture;
					gl_Position = vec4( position, 0.0, 1.0 );
				} );
			return writer.finalise();
		}

		glsl::Shader doGetBlurProgram( Engine & engine
			, bool isVertic )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto materials = shader::createMaterials( writer
				, ( engine.getMaterialsType() == MaterialType::ePbrMetallicRoughness
					? PassFlag::ePbrMetallicRoughness
					: ( engine.getMaterialsType() == MaterialType::ePbrSpecularGlossiness
						? PassFlag::ePbrSpecularGlossiness
						: PassFlag( 0u ) ) ) );
			materials->declare();
			UBO_SCENE( writer, 1u, 0u );
			UBO_GPINFO( writer, 2u, 0u );
			Ubo config{ writer, SubsurfaceScatteringPass::Config, 3u, 0u };
			auto c3d_pixelSize = config.declMember< Float >( SubsurfaceScatteringPass::PixelSize );
			auto c3d_correction = config.declMember< Float >( SubsurfaceScatteringPass::Correction );
			config.end();
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eDepth ), 4u, 0u );
			auto c3d_mapData4 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData4 ), 5u, 0u );
			auto c3d_mapData5 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData5 ), 6u, 0u );
			auto c3d_mapLightDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightDiffuse" ), 7u, 0u );

			// Gaussian weights for the six samples around the current pixel:
			//   -3 -2 -1 +1 +2 +3
			auto w = writer.declUniform( cuT( "w" )
				, 6u
				, std::vector< Float >{ { 0.006_f, 0.061_f, 0.242_f, 0.242_f, 0.061_f, 0.006_f } } );
			auto o = writer.declUniform( cuT( "o" )
				, 6u
				, std::vector< Float >{ { -1.0, -0.6667, -0.3333, 0.3333, 0.6667, 1.0 } } );

			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			glsl::Utils utils{ writer };
			utils.declareCalcVSPosition();

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto data4 = writer.declLocale( cuT( "data4" )
					, texture( c3d_mapData4, vtx_texture ) );
				auto data5 = writer.declLocale( cuT( "data5" )
					, texture( c3d_mapData5, vtx_texture ) );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );
				auto translucency = writer.declLocale( cuT( "translucency" )
					, data4.w() );
				auto material = materials->getBaseMaterial( materialId );

				IF( writer, material->m_subsurfaceScatteringEnabled() == 0_i )
				{
					writer.discard();
				}
				FI;

				// Fetch color and linear depth for current pixel:
				auto colorM = writer.declLocale( cuT( "colorM" )
					, texture( c3d_mapLightDiffuse, vtx_texture ) );
				auto depthM = writer.declLocale( cuT( "depthM" )
					, texture( c3d_mapDepth, vtx_texture ).r() );
				depthM = utils.calcVSPosition( vtx_texture
					, depthM
					, c3d_mtxInvProj ).z();

				// Accumulate center sample, multiplying it with its gaussian weight:
				pxl_fragColor = colorM;
				pxl_fragColor.rgb() *= 0.382_f;

				if ( isVertic )
				{
					auto step = writer.declLocale( cuT( "step" )
						, c3d_pixelSize * vec2( 0.0_f, 1.0_f ) );
				}
				else
				{
					auto step = writer.declLocale( cuT( "step" )
						, c3d_pixelSize * vec2( 1.0_f, 0.0_f ) );
				}

				auto step = writer.declBuiltin< Vec2 >( cuT( "step" ) );
				// Calculate the step that we will use to fetch the surrounding pixels,
				// where "step" is:
				//     step = sssStrength * gaussianWidth * pixelSize * dir
				// The closer the pixel, the stronger the effect needs to be, hence
				// the factor 1.0 / depthM.
				auto finalStep = writer.declLocale( cuT( "finalStep" )
					, translucency * step * material->m_subsurfaceScatteringStrength() * material->m_gaussianWidth() / depthM );

				auto offset = writer.declLocale< Vec2 >( cuT( "offset" ) );
				auto color = writer.declLocale< Vec3 >( cuT( "color" ) );
				auto depth = writer.declLocale< Float >( cuT( "depth" ) );
				auto s = writer.declLocale< Float >( cuT( "s" ) );

				// Accumulate the other samples:
				for ( int i = 0; i < 6; i++ )
				{
					// Fetch color and depth for current sample:
					offset = glsl::fma( vec2( o[i] ), finalStep, vtx_texture );
					color = texture( c3d_mapLightDiffuse, offset, 0.0_f ).rgb();
					depth = texture( c3d_mapDepth, offset, 0.0_f ).r();
					depth = utils.calcVSPosition( vtx_texture
						, depth
						, c3d_mtxInvProj ).z();

					// If the difference in depth is huge, we lerp color back to "colorM":
					s = min( 0.0125_f * c3d_correction * abs( depthM - depth ), 1.0_f );
					color = mix( color, colorM.rgb(), s );

					// Accumulate:
					pxl_fragColor.rgb() += w[i] * color;
				}
			} );
			return writer.finalise();
		}

		glsl::Shader doGetCombineProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto materials = shader::createMaterials( writer
				, ( engine.getMaterialsType() == MaterialType::ePbrMetallicRoughness
					? PassFlag::ePbrMetallicRoughness
					: ( engine.getMaterialsType() == MaterialType::ePbrSpecularGlossiness
						? PassFlag::ePbrSpecularGlossiness
						: PassFlag( 0u ) ) ) );
			materials->declare();
			auto c3d_mapData4 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData4 ), 1u, 0u );
			auto c3d_mapData5 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData5 ), 2u, 0u );
			auto c3d_mapBlur1 = writer.declSampler< Sampler2D >( cuT( "c3d_mapBlur1" ), 3u, 0u );
			auto c3d_mapBlur2 = writer.declSampler< Sampler2D >( cuT( "c3d_mapBlur2" ), 4u, 0u );
			auto c3d_mapBlur3 = writer.declSampler< Sampler2D >( cuT( "c3d_mapBlur3" ), 5u, 0u );
			auto c3d_mapLightDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightDiffuse" ), 6u, 0u );

			auto c3d_originalWeight = writer.declUniform< Vec4 >( cuT( "c3d_originalWeight" )
				, vec4( 0.2406_f, 0.4475_f, 0.6159_f, 0.25_f ) );
			auto c3d_blurWeights = writer.declUniform< Vec4 >( cuT( "c3d_blurWeights" )
				, 3u
				, {
					vec4( 0.1158_f, 0.3661_f, 0.3439_f, 0.25_f ),
					vec4( 0.1836_f, 0.1864_f, 0.0_f, 0.25_f ),
					vec4( 0.46_f, 0.0_f, 0.0402_f, 0.25_f )
				} );
			auto c3d_blurVariances = writer.declUniform< Float >( cuT( "c3d_blurVariances" )
				, 3u
				, {
					0.0516_f,
					0.2719_f,
					2.0062_f
				} );

			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto data4 = writer.declLocale( cuT( "data4" )
					, texture( c3d_mapData4, vtx_texture ) );
				auto data5 = writer.declLocale( cuT( "data5" )
					, texture( c3d_mapData5, vtx_texture ) );
				auto original = writer.declLocale( cuT( "original" )
					, texture( c3d_mapLightDiffuse, vtx_texture ) );
				auto blur1 = writer.declLocale( cuT( "blur1" )
					, texture( c3d_mapBlur1, vtx_texture ) );
				auto blur2 = writer.declLocale( cuT( "blur2" )
					, texture( c3d_mapBlur2, vtx_texture ) );
				auto blur3 = writer.declLocale( cuT( "blur3" )
					, texture( c3d_mapBlur3, vtx_texture ) );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );
				auto translucency = writer.declLocale( cuT( "translucency" )
					, data4.w() );
				auto material = materials->getBaseMaterial( materialId );

#if !C3D_DEBUG_SSS_TRANSMITTANCE
				IF( writer, material->m_subsurfaceScatteringEnabled() == 0_i )
				{
					pxl_fragColor = original;
				}
				ELSE
				{
					pxl_fragColor = original * c3d_originalWeight
						+ blur1 * c3d_blurWeights[0]
						+ blur2 * c3d_blurWeights[1]
						+ blur3 * c3d_blurWeights[2];
				}
				FI;
#else
				pxl_fragColor = original;
#endif
			} );
			return writer.finalise();
		}

		renderer::ShaderProgram & doCreateBlurProgram( Engine & engine
			, bool isVertic )
		{
			auto & cache = engine.getShaderProgramCache();
			auto const vtx = doGetVertexProgram( engine );
			auto const pxl = doGetBlurProgram( engine, isVertic );

			auto & program = cache.getNewProgram( false );
			program.createModule( vtx.getSource(), renderer::ShaderStageFlag::eVertex );
			program.createModule( pxl.getSource(), renderer::ShaderStageFlag::eFragment );
			return program;
		}

		renderer::ShaderProgram & doCreateCombineProgram( Engine & engine )
		{
			auto & cache = engine.getShaderProgramCache();
			auto const vtx = doGetVertexProgram( engine );
			auto const pxl = doGetCombineProgram( engine );

			auto & program = cache.getNewProgram( false );
			program.createModule( vtx.getSource(), renderer::ShaderStageFlag::eVertex );
			program.createModule( pxl.getSource(), renderer::ShaderStageFlag::eFragment );
			return program;
		}

		SamplerSPtr doCreateSampler( Engine & engine )
		{
			SamplerSPtr sampler;
			String const name{ cuT( "SubsurfaceScatteringPass" ) };

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( renderer::Filter::eNearest );
				sampler->setMagFilter( renderer::Filter::eNearest );
				sampler->setWrapS( renderer::WrapMode::eClampToEdge );
				sampler->setWrapT( renderer::WrapMode::eClampToEdge );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine
			, Size const & size )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine );
			auto texture = std::make_shared< TextureLayout >( renderSystem
				, renderer::TextureType::e2D
				, renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled
				, renderer::MemoryPropertyFlag::eHostVisible
				, PixelFormat::eRGBA32F
				, size );
			texture->getImage().initialiseSource();
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );
			unit.setIndex( MinTextureIndex );
			unit.initialise();
			return unit;
		}
	}

	//*********************************************************************************************

	SubsurfaceScatteringPass::Blur::Blur( RenderSystem & renderSystem
		, castor::Size const & size
		, GpInfoUbo & gpInfoUbo
		, SceneUbo & sceneUbo
		, renderer::UniformBuffer< Configuration > const & blurUbo
		, GeometryPassResult const & gp
		, TextureUnit const & source
		, TextureUnit const & destination
		, bool isVertic )
		: RenderQuad{ renderSystem, false, false }
		, m_renderSystem{ renderSystem }
		, m_geometryBufferResult{ gp }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_sceneUbo{ sceneUbo }
		, m_blurUbo{ blurUbo }
	{
		auto & device = *renderSystem.getCurrentDevice();
		std::vector< PixelFormat > formats{ destination.getTexture()->getPixelFormat() };
		renderer::RenderPassAttachmentArray rpAttaches{ { destination.getTexture()->getPixelFormat(), false } };

		// Create the render pass.
		renderer::RenderSubpassPtrArray subpasses;
		subpasses.emplace_back( device.createRenderSubpass( formats
			, renderer::RenderSubpassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
			, renderer::AccessFlag::eColourAttachmentWrite } ) );
		m_renderPass = device.createRenderPass( rpAttaches
			, std::move( subpasses )
			, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::AccessFlag::eColourAttachmentWrite
				, { renderer::ImageLayout::eColourAttachmentOptimal } }
			, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::AccessFlag::eColourAttachmentWrite
				, { renderer::ImageLayout::eColourAttachmentOptimal } } );

		// Initialise the frame buffer.
		renderer::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *m_renderPass->begin(), destination.getTexture()->getView() );
		m_frameBuffer = m_renderPass->createFrameBuffer( renderer::UIVec2{ size }
			, std::move( attaches ) );

		renderer::DescriptorSetLayoutBindingArray bindings
		{
			renderSystem.getEngine()->getMaterialCache().getPassBuffer().createLayoutBinding(),
			{ 1u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment }, // Scene UBO
			{ 2u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment }, // GpInfo UBO
			{ 3u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment }, // Blur UBO
			{ 4u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }, // Depth map
			{ 5u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }, // Translucency map
			{ 6u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }, // MaterialIndex map
		};

		createPipeline( size
			, Position{}
			, doCreateBlurProgram( *renderSystem.getEngine(), isVertic )
			, source.getTexture()->getView()
			, *m_renderPass
			, bindings );
	}

	void SubsurfaceScatteringPass::Blur::prepareFrame( renderer::CommandBuffer const & commandBuffer )const
	{
		commandBuffer.beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { renderer::RgbaColour{ 0, 0, 0, 1 } }
			, renderer::SubpassContents::eSecondaryCommandBuffers );
		commandBuffer.executeCommands( { getCommandBuffer() } );
		commandBuffer.endRenderPass();
	}

	void SubsurfaceScatteringPass::Blur::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
	{
		m_renderSystem.getEngine()->getMaterialCache().getPassBuffer().createBinding( descriptorSet
			, descriptorSetLayout.getBinding( PassBufferIndex ) );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 1u )
			, m_sceneUbo.getUbo()
			, 0u
			, 1u );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 2u )
			, m_gpInfoUbo.getUbo()
			, 0u
			, 1u );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 3u )
			, m_blurUbo
			, 0u
			, 1u );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 4u )
			, m_geometryBufferResult[size_t( DsTexture::eDepth )]->getTexture()->getView()
			, m_geometryBufferResult[size_t( DsTexture::eDepth )]->getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 5u )
			, m_geometryBufferResult[size_t( DsTexture::eData4 )]->getTexture()->getView()
			, m_geometryBufferResult[size_t( DsTexture::eData4 )]->getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 6u )
			, m_geometryBufferResult[size_t( DsTexture::eData5 )]->getTexture()->getView()
			, m_geometryBufferResult[size_t( DsTexture::eData5 )]->getSampler()->getSampler() );
	}

	//*********************************************************************************************

	SubsurfaceScatteringPass::Combine::Combine( RenderSystem & renderSystem
		, Size const & size
		, GeometryPassResult const & gp
		, TextureUnit const & source
		, std::array< TextureUnit, 3u > const & blurResults
		, TextureUnit const & destination )
		: RenderQuad{ renderSystem, false, false }
		, m_renderSystem{ renderSystem }
		, m_geometryBufferResult{ gp }
		, m_source{ source }
		, m_blurResults{ blurResults }
	{
		auto & device = *renderSystem.getCurrentDevice();
		std::vector< PixelFormat > formats{ destination.getTexture()->getPixelFormat() };
		renderer::RenderPassAttachmentArray rpAttaches{ { destination.getTexture()->getPixelFormat(), false } };

		// Create the render pass.
		renderer::RenderSubpassPtrArray subpasses;
		subpasses.emplace_back( device.createRenderSubpass( formats
			, renderer::RenderSubpassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
			, renderer::AccessFlag::eColourAttachmentWrite } ) );
		m_renderPass = device.createRenderPass( rpAttaches
			, std::move( subpasses )
			, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::AccessFlag::eColourAttachmentWrite
				, { renderer::ImageLayout::eColourAttachmentOptimal } }
			, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::AccessFlag::eColourAttachmentWrite
				, { renderer::ImageLayout::eColourAttachmentOptimal } } );

		// Initialise the frame buffer.
		renderer::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *m_renderPass->begin(), destination.getTexture()->getView() );
		m_frameBuffer = m_renderPass->createFrameBuffer( renderer::UIVec2{ size }
			, std::move( attaches ) );

		renderer::DescriptorSetLayoutBindingArray bindings
		{
			renderSystem.getEngine()->getMaterialCache().getPassBuffer().createLayoutBinding(),
			{ 1u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }, // Translucency map
			{ 2u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }, // MaterialIndex map
			{ 3u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }, // Blur result 0
			{ 4u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }, // Blur result 1
			{ 5u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }, // Blur result 2
		};

		createPipeline( size
			, Position{}
			, doCreateCombineProgram( *renderSystem.getEngine() )
			, source.getTexture()->getView()
			, *m_renderPass
			, bindings );
	}

	void SubsurfaceScatteringPass::Combine::prepareFrame( renderer::CommandBuffer const & commandBuffer )const
	{
		commandBuffer.beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { renderer::RgbaColour{ 0, 0, 0, 1 } }
		, renderer::SubpassContents::eSecondaryCommandBuffers );
		commandBuffer.executeCommands( { getCommandBuffer() } );
		commandBuffer.endRenderPass();
	}

	void SubsurfaceScatteringPass::Combine::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
	{
		m_renderSystem.getEngine()->getMaterialCache().getPassBuffer().createBinding( descriptorSet
			, descriptorSetLayout.getBinding( PassBufferIndex ) );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 1u )
			, m_geometryBufferResult[size_t( DsTexture::eData4 )]->getTexture()->getView()
			, m_geometryBufferResult[size_t( DsTexture::eData4 )]->getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 2u )
			, m_geometryBufferResult[size_t( DsTexture::eData5 )]->getTexture()->getView()
			, m_geometryBufferResult[size_t( DsTexture::eData5 )]->getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 3u )
			, m_blurResults[0].getTexture()->getView()
			, m_blurResults[0].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 4u )
			, m_blurResults[1].getTexture()->getView()
			, m_blurResults[1].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 5u )
			, m_blurResults[2].getTexture()->getView()
			, m_blurResults[2].getSampler()->getSampler() );
	}

	//*********************************************************************************************

	String const SubsurfaceScatteringPass::Config = cuT( "Config" );
	String const SubsurfaceScatteringPass::Step = cuT( "c3d_step" );
	String const SubsurfaceScatteringPass::Correction = cuT( "c3d_correction" );
	String const SubsurfaceScatteringPass::PixelSize = cuT( "c3d_pixelSize" );

	SubsurfaceScatteringPass::SubsurfaceScatteringPass( Engine & engine
		, GpInfoUbo & gpInfoUbo
		, SceneUbo & sceneUbo
		, Size const & textureSize
		, GeometryPassResult const & gp
		, TextureUnit const & lightDiffuse )
		: OwnedBy< Engine >{ engine }
		, m_size{ textureSize }
		, m_blurUbo{ renderer::makeUniformBuffer< Configuration >( *engine.getRenderSystem()->getCurrentDevice()
			, 1u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible ) }
		, m_intermediate{ doCreateTexture( engine, textureSize ) }
		, m_blurResults
		{
			{
				doCreateTexture( engine, textureSize ),
				doCreateTexture( engine, textureSize ),
				doCreateTexture( engine, textureSize ),
			}
		}
		, m_result{ doCreateTexture( engine, textureSize ) }
		, m_blurX
		{
			{ *engine.getRenderSystem(), m_size, gpInfoUbo, sceneUbo, *m_blurUbo, gp, lightDiffuse, m_intermediate, false },
			{ *engine.getRenderSystem(), m_size, gpInfoUbo, sceneUbo, *m_blurUbo, gp, m_blurResults[0], m_intermediate, false },
			{ *engine.getRenderSystem(), m_size, gpInfoUbo, sceneUbo, *m_blurUbo, gp, m_blurResults[1], m_intermediate, false },
		}
		, m_blurY
		{
			{ *engine.getRenderSystem(), m_size, gpInfoUbo, sceneUbo, *m_blurUbo, gp, m_intermediate, m_blurResults[0], true },
			{ *engine.getRenderSystem(), m_size, gpInfoUbo, sceneUbo, *m_blurUbo, gp, m_intermediate, m_blurResults[1], true },
			{ *engine.getRenderSystem(), m_size, gpInfoUbo, sceneUbo, *m_blurUbo, gp, m_intermediate, m_blurResults[2], true },
		}
		, m_combine{ *engine.getRenderSystem(), textureSize, gp, lightDiffuse, m_blurResults, m_result }
	{
		auto & configuration = m_blurUbo->getData( 0u );
		configuration.blurCorrection = 1.0f;
		configuration.blurPixelSize = renderer::Vec2{ 1.0f / m_size.getWidth(), 1.0f / m_size.getHeight() };
		m_blurUbo->upload( 0u );
	}

	SubsurfaceScatteringPass::~SubsurfaceScatteringPass()
	{
		m_result.cleanup();
		m_intermediate.cleanup();

		for ( auto & result : m_blurResults )
		{
			result.cleanup();
		}
	}

	void SubsurfaceScatteringPass::prepare( renderer::CommandBuffer const & commandBuffer )const
	{
		for ( size_t i{ 0u }; i < m_blurResults.size(); ++i )
		{
			m_blurX[i].prepareFrame( commandBuffer );
			m_blurY[i].prepareFrame( commandBuffer );
		}

		m_combine.prepareFrame( commandBuffer );
	}

	void SubsurfaceScatteringPass::debugDisplay( castor::Size const & size )const
	{
		//auto count = 9u;
		//int width = int( m_size.getWidth() ) / count;
		//int height = int( m_size.getHeight() ) / count;
		//int top = int( m_size.getHeight() ) - height;
		//auto renderSize = Size( width, height );
		//auto & context = *getEngine()->getRenderSystem()->getCurrentContext();
		//auto index = 0;
		//context.renderTexture( Position{ width * index++, top }
		//	, renderSize
		//	, *m_blurResults[0].getTexture() );
		//context.renderTexture( Position{ width * index++, top }
		//	, renderSize
		//	, *m_blurResults[1].getTexture() );
		//context.renderTexture( Position{ width * index++, top }
		//	, renderSize
		//	, *m_blurResults[2].getTexture() );
		//context.renderTexture( Position{ width * index++, top }
		//	, renderSize
		//	, *m_result.getTexture() );
	}
}
