#include "ReflectionPass.hpp"

#include "Engine.hpp"

#include "Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Skybox.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/SceneUbo.hpp"
#include "Castor3DPrerequisites.hpp"
#include "Shader/Shaders/GlslFog.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/Shaders/GlslPhongReflection.hpp"
#include "Shader/Shaders/GlslSssTransmittance.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureView.hpp"
#include "Texture/TextureLayout.hpp"

#include <Command/CommandBuffer.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include <Graphics/Image.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		static uint32_t constexpr c_environmentCount = 16u;

		renderer::VertexLayoutPtr doCreateVertexLayout()
		{
			auto result = std::make_unique< renderer::VertexLayout >( 0u, 16u, renderer::VertexInputRate::eVertex );
			result->createAttribute( 0u, renderer::Format::eR32G32_SFLOAT, 0u );
			result->createAttribute( 1u, renderer::Format::eR32G32_SFLOAT, 8u );
			return result;
		}

		renderer::VertexBufferBasePtr doCreateVbo( Engine & engine )
		{
			float data[] =
			{
				0, 0, 0, 0,
				1, 1, 1, 1,
				0, 1, 0, 1,
				0, 0, 0, 0,
				1, 0, 1, 0,
				1, 1, 1, 1
			};

			auto vertexBuffer = std::make_unique< renderer::VertexBufferBase >( *engine.getRenderSystem()->getCurrentDevice()
				, uint32_t( sizeof( data ) )
				, 0u
				, renderer::MemoryPropertyFlag::eHostVisible );
			if ( auto * buffer = vertexBuffer->getBuffer().lock( 0u, vertexBuffer->getSize(), renderer::MemoryMapFlag::eWrite ) )
			{
				std::memcpy( buffer, data, sizeof( data ) );
				vertexBuffer->getBuffer().flush( 0u, vertexBuffer->getSize() );
				vertexBuffer->getBuffer().unlock();
			}

			return vertexBuffer;
		}

		glsl::Shader doCreateVertexProgram( RenderSystem & renderSystem )
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0 );
			auto texture = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 0 );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
			{
				vtx_texture = texture;
				gl_Position = vec4( position.x(), position.y(), 0.0, 1.0 );
			} );
			return writer.finalise();
		}

		glsl::Shader doCreateLegacyPixelProgram( RenderSystem & renderSystem
			, FogType fogType
			, bool hasSsao )
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
			UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
			auto index = 0u;
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eDepth ), index++, 1u );
			auto c3d_mapData1 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData1 ), index++, 1u );
			auto c3d_mapData2 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData2 ), index++, 1u );
			auto c3d_mapData3 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData3 ), index++, 1u );
			auto c3d_mapData4 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData4 ), index++, 1u );
			auto c3d_mapData5 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData5 ), index++, 1u );
			auto c3d_mapSsao = writer.declSampler< Sampler2D >( cuT( "c3d_mapSsao" ), hasSsao ? index++ : 0u, 1u, hasSsao );
			auto c3d_mapLightDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightDiffuse" ), index++, 1u );
			auto c3d_mapLightSpecular = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightSpecular" ), index++, 1u );
			auto c3d_mapEnvironment = writer.declSamplerArray< SamplerCube >( cuT( "c3d_mapEnvironment" ), index++, 1u, c_environmentCount );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			shader::LegacyMaterials materials{ writer };
			materials.declare();

			glsl::Utils utils{ writer };
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();
			shader::PhongReflectionModel reflections{ writer };
			declareDecodeMaterial( writer );
			shader::Fog fog{ fogType, writer };

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0u );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
			{
				auto data1 = writer.declLocale( cuT( "data1" )
					, texture( c3d_mapData1, vtx_texture ) );
				auto data2 = writer.declLocale( cuT( "data2" )
					, texture( c3d_mapData2, vtx_texture ) );
				auto data3 = writer.declLocale( cuT( "data3" )
					, texture( c3d_mapData3, vtx_texture ) );
				auto data4 = writer.declLocale( cuT( "data4" )
					, texture( c3d_mapData4, vtx_texture ) );
				auto data5 = writer.declLocale( cuT( "data5" )
					, texture( c3d_mapData5, vtx_texture ) );
				auto flags = writer.declLocale( cuT( "flags" )
					, data1.w() );
				auto envMapIndex = writer.declLocale( cuT( "envMapIndex" )
					, 0_i );
				auto receiver = writer.declLocale( cuT( "receiver" )
					, 0_i );
				auto reflection = writer.declLocale( cuT( "reflection" )
					, 0_i );
				auto refraction = writer.declLocale( cuT( "refraction" )
					, 0_i );
				decodeMaterial( writer
					, flags
					, receiver
					, reflection
					, refraction
					, envMapIndex );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );
				auto material = writer.declLocale( cuT( "material" )
					, materials.getMaterial( materialId ) );
				auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
					, texture( c3d_mapLightDiffuse, vtx_texture ).xyz() );
				auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
					, texture( c3d_mapLightSpecular, vtx_texture ).xyz() );
				auto depth = writer.declLocale( cuT( "depth" )
					, texture( c3d_mapDepth, vtx_texture, 0.0_f ).x() );
				auto position = writer.declLocale( cuT( "position" )
					, utils.calcWSPosition( vtx_texture, depth, c3d_mtxInvViewProj ) );
				auto normal = writer.declLocale( cuT( "normal" )
					, normalize( data1.xyz() ) );
				auto diffuse = writer.declLocale( cuT( "diffuse" )
					, data2.xyz() );
				auto occlusion = writer.declLocale( cuT( "occlusion" )
					, data3.a() );
				auto emissive = writer.declLocale( cuT( "emissive" )
					, data4.xyz() );
				auto ambient = writer.declLocale( cuT( "ambient" )
					, clamp( c3d_ambientLight.xyz() + material.m_ambient() * material.m_diffuse()
						, vec3( 0.0_f )
						, vec3( 1.0_f ) ) );

				if ( hasSsao )
				{
					occlusion *= texture( c3d_mapSsao, vtx_texture ).r();
				}

				IF( writer, envMapIndex > 0_i && writer.paren( reflection != 0_i || refraction != 0_i ) )
				{
					auto incident = writer.declLocale( cuT( "incident" )
						, reflections.computeIncident( position, c3d_cameraPosition ) );
					envMapIndex = envMapIndex - 1_i;

					IF( writer, reflection != 0_i && refraction != 0_i )
					{
						ambient = reflections.computeReflRefr( incident
							, normal
							, occlusion
							, c3d_mapEnvironment[envMapIndex]
							, material.m_refractionRatio()
							, diffuse );
						diffuse = vec3( 0.0_f );
					}
					ELSEIF( writer, reflection != 0_i )
					{
						diffuse = reflections.computeRefl( incident
							, normal
							, occlusion
							, c3d_mapEnvironment[envMapIndex] );
						ambient = vec3( 0.0_f );
					}
					ELSE
					{
						ambient = reflections.computeRefr( incident
							, normal
							, occlusion
							, c3d_mapEnvironment[envMapIndex]
							, material.m_refractionRatio()
							, diffuse );
						diffuse = vec3( 0.0_f );
					}
					FI;
				}
				ELSE
				{
					ambient *= occlusion * diffuse;
					diffuse *= lightDiffuse;
				}
				FI;

				pxl_fragColor = vec4( diffuse + lightSpecular + emissive + ambient, 1.0 );

				if ( fogType != FogType::eDisabled )
				{
					position = utils.calcVSPosition( vtx_texture
							, texture( c3d_mapDepth, vtx_texture ).r()
							, c3d_mtxInvProj );
					fog.applyFog( pxl_fragColor, length( position ), position.z() );
				}
			} );
			return writer.finalise();
		}

		glsl::Shader doCreatePbrMRPixelProgram( RenderSystem & renderSystem
			, FogType fogType
			, bool hasSsao )
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
			UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
			auto index = 0u;
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eDepth ), index++, 1u );
			auto c3d_mapData1 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData1 ), index++, 1u );
			auto c3d_mapData2 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData2 ), index++, 1u );
			auto c3d_mapData3 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData3 ), index++, 1u );
			auto c3d_mapData4 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData4 ), index++, 1u );
			auto c3d_mapData5 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData5 ), index++, 1u );
			auto c3d_mapSsao = writer.declSampler< Sampler2D >( cuT( "c3d_mapSsao" ), hasSsao ? index++ : 0u, 1u, hasSsao );
			auto c3d_mapLightDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightDiffuse" ), index++, 1u );
			auto c3d_mapLightSpecular = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightSpecular" ), index++, 1u );
			auto c3d_mapBrdf = writer.declSampler< Sampler2D >( cuT( "c3d_mapBrdf" ), index++, 1u );
			auto c3d_mapIrradiance = writer.declSampler< SamplerCube >( cuT( "c3d_mapIrradiance" ), index++, 1u );
			auto c3d_mapPrefiltered = writer.declSampler< SamplerCube >( cuT( "c3d_mapPrefiltered" ), index++, 1u );
			auto c3d_mapEnvironment = writer.declSamplerArray< SamplerCube >( cuT( "c3d_mapEnvironment" ), index++, 1u, c_environmentCount );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			shader::PbrMRMaterials materials{ writer };
			materials.declare();

			glsl::Utils utils{ writer };
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();
			utils.declareFresnelSchlick();
			utils.declareComputeMetallicIBL();
			declareDecodeMaterial( writer );
			shader::Fog fog{ fogType, writer };

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0u );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
			{
				auto data1 = writer.declLocale( cuT( "data1" )
					, texture( c3d_mapData1, vtx_texture ) );
				auto flags = writer.declLocale( cuT( "flags" )
					, data1.w() );
				auto envMapIndex = writer.declLocale( cuT( "envMapIndex" )
					, 0_i );
				auto receiver = writer.declLocale( cuT( "receiver" )
					, 0_i );
				auto reflection = writer.declLocale( cuT( "reflection" )
					, 0_i );
				auto refraction = writer.declLocale( cuT( "refraction" )
					, 0_i );
				decodeMaterial( writer
					, flags
					, receiver
					, reflection
					, refraction
					, envMapIndex );

				auto data5 = writer.declLocale( cuT( "data5" )
					, texture( c3d_mapData5, vtx_texture ) );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );
				auto material = writer.declLocale( cuT( "material" )
					, materials.getMaterial( materialId ) );
				auto data2 = writer.declLocale( cuT( "data2" )
					, texture( c3d_mapData2, vtx_texture ) );
				auto data3 = writer.declLocale( cuT( "data3" )
					, texture( c3d_mapData3, vtx_texture ) );
				auto data4 = writer.declLocale( cuT( "data4" )
					, texture( c3d_mapData4, vtx_texture ) );
				auto normal = writer.declLocale( cuT( "normal" )
					, data1.xyz() );
				auto albedo = writer.declLocale( cuT( "albedo" )
					, data2.xyz() );
				auto metalness = writer.declLocale( cuT( "metalness" )
					, data3.r() );
				auto roughness = writer.declLocale( cuT( "roughness" )
					, data3.g() );
				auto occlusion = writer.declLocale( cuT( "occlusion" )
					, data3.a() );
				auto emissive = writer.declLocale( cuT( "emissive" )
					, data4.xyz() );
				auto depth = writer.declLocale( cuT( "depth" )
					, texture( c3d_mapDepth, vtx_texture, 0.0_f ).x() );
				auto position = writer.declLocale( cuT( "position" )
					, utils.calcWSPosition( vtx_texture, depth, c3d_mtxInvViewProj ) );
				auto incident = writer.declLocale( cuT( "incident" )
					, normalize( position - c3d_cameraPosition ) );
				auto ambient = writer.declLocale( cuT( "ambient" )
					, c3d_ambientLight.xyz() );
				auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
					, texture( c3d_mapLightDiffuse, vtx_texture ).xyz() );
				auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
					, texture( c3d_mapLightSpecular, vtx_texture ).xyz() );
				auto ratio = writer.declLocale( cuT( "ratio" )
					, material.m_refractionRatio() );

				if ( hasSsao )
				{
					occlusion *= texture( c3d_mapSsao, vtx_texture ).r();
				}

				IF( writer, envMapIndex > 0_i )
				{
					envMapIndex = envMapIndex - 1_i;

					IF( writer, reflection != 0_i && refraction != 0_i )
					{
						auto reflected = writer.declLocale( cuT( "reflected" )
							, reflect( incident, normal ) );
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* texture( c3d_mapEnvironment[envMapIndex], reflected ).xyz();
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						ambient = mix( texture( c3d_mapEnvironment[envMapIndex], refracted ).xyz() * albedo / length( albedo )
							, ambient
							, fresnel );
					}
					ELSEIF( writer, reflection != 0_i && ratio != 0.0_f )
					{
						auto reflected = writer.declLocale( cuT( "reflected" )
							, reflect( incident, normal ) );
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* texture( c3d_mapEnvironment[envMapIndex], reflected ).xyz();
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						refracted.y() = -refracted.y();
						ambient = mix( texture( c3d_mapPrefiltered, refracted, roughness * Utils::MaxIblReflectionLod ).xyz() * albedo / length( albedo )
							, ambient
							, fresnel );
					}
					ELSEIF( writer, reflection != 0_i )
					{
						auto reflected = writer.declLocale( cuT( "reflected" )
							, reflect( incident, normal ) );
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* texture( c3d_mapEnvironment[envMapIndex], reflected ).xyz()
							* albedo / length( albedo );
					}
					ELSEIF( writer, refraction != 0_i )
					{
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* utils.computeMetallicIBL( normal
								, position
								, albedo
								, metalness
								, roughness
								, c3d_cameraPosition
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						ambient = mix( texture( c3d_mapEnvironment[envMapIndex], refracted ).xyz() * albedo / length( albedo )
							, ambient
							, fresnel );
					}
					ELSEIF( writer, ratio != 0.0_f )
					{
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* utils.computeMetallicIBL( normal
								, position
								, albedo
								, metalness
								, roughness
								, c3d_cameraPosition
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						refracted.y() = -refracted.y();
						ambient = mix( texture( c3d_mapPrefiltered, refracted, roughness * Utils::MaxIblReflectionLod ).xyz() * albedo / length( albedo )
							, ambient
							, fresnel );
					}
					ELSE
					{
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* utils.computeMetallicIBL( normal
								, position
								, albedo
								, metalness
								, roughness
								, c3d_cameraPosition
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );
					}
					FI;
				}
				ELSEIF( writer, ratio != 0.0_f )
				{
					ambient = c3d_ambientLight.xyz()
						* occlusion
						* utils.computeMetallicIBL( normal
							, position
							, albedo
							, metalness
							, roughness
							, c3d_cameraPosition
							, c3d_mapIrradiance
							, c3d_mapPrefiltered
							, c3d_mapBrdf );
					auto subRatio = writer.declLocale( cuT( "subRatio" )
						, 1.0_f - ratio );
					auto addRatio = writer.declLocale( cuT( "addRatio" )
						, 1.0_f + ratio );
					auto reflectance = writer.declLocale( cuT( "reflectance" )
						, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
					auto product = writer.declLocale( cuT( "product" )
						, max( 0.0_f, dot( -incident, normal ) ) );
					auto fresnel = writer.declLocale( cuT( "fresnel" )
						, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
					auto refracted = writer.declLocale( cuT( "refracted" )
						, refract( incident, normal, ratio ) );
					refracted.y() = -refracted.y();
					ambient = mix( texture( c3d_mapPrefiltered, refracted, roughness * Utils::MaxIblReflectionLod ).xyz() * albedo / length( albedo )
						, ambient
						, fresnel );
				}
				ELSE
				{
					ambient = c3d_ambientLight.xyz()
						* occlusion
						* utils.computeMetallicIBL( normal
							, position
							, albedo
							, metalness
							, roughness
							, c3d_cameraPosition
							, c3d_mapIrradiance
							, c3d_mapPrefiltered
							, c3d_mapBrdf );
				}
				FI;

#if !C3D_DEBUG_SSS_TRANSMITTANCE
				pxl_fragColor = vec4( lightDiffuse * albedo + lightSpecular + emissive + ambient, 1.0 );
#else
				pxl_fragColor = vec4( lightDiffuse, 1.0 );
#endif

				if ( fogType != FogType::eDisabled )
				{
					position = utils.calcVSPosition( vtx_texture
							, texture( c3d_mapDepth, vtx_texture ).r()
							, c3d_mtxInvProj );
					fog.applyFog( pxl_fragColor, length( position ), position.z() );
				}
			} );
			return writer.finalise();
		}

		glsl::Shader doCreatePbrSGPixelProgram( RenderSystem & renderSystem
			, FogType fogType
			, bool hasSsao )
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
			UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
			auto index = 0u;
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eDepth ), index++, 1u );
			auto c3d_mapData1 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData1 ), index++, 1u );
			auto c3d_mapData2 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData2 ), index++, 1u );
			auto c3d_mapData3 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData3 ), index++, 1u );
			auto c3d_mapData4 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData4 ), index++, 1u );
			auto c3d_mapData5 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData5 ), index++, 1u );
			auto c3d_mapSsao = writer.declSampler< Sampler2D >( cuT( "c3d_mapSsao" ), hasSsao ? index++ : 0u, 1u, hasSsao );
			auto c3d_mapLightDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightDiffuse" ), index++, 1u );
			auto c3d_mapLightSpecular = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightSpecular" ), index++, 1u );
			auto c3d_mapBrdf = writer.declSampler< Sampler2D >( cuT( "c3d_mapBrdf" ), index++, 1u );
			auto c3d_mapIrradiance = writer.declSampler< SamplerCube >( cuT( "c3d_mapIrradiance" ), index++, 1u );
			auto c3d_mapPrefiltered = writer.declSampler< SamplerCube >( cuT( "c3d_mapPrefiltered" ), index++, 1u );
			auto c3d_mapEnvironment = writer.declSamplerArray< SamplerCube >( cuT( "c3d_mapEnvironment" ), index++, 1u, c_environmentCount );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			shader::PbrSGMaterials materials{ writer };
			materials.declare();
			
			glsl::Utils utils{ writer };
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();
			utils.declareFresnelSchlick();
			utils.declareComputeSpecularIBL();
			declareDecodeMaterial( writer );
			shader::Fog fog{ fogType, writer };

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0u );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
			{
				auto data1 = writer.declLocale( cuT( "data1" )
					, texture( c3d_mapData1, vtx_texture ) );
				auto flags = writer.declLocale( cuT( "flags" )
					, data1.w() );
				auto envMapIndex = writer.declLocale( cuT( "envMapIndex" )
					, 0_i );
				auto receiver = writer.declLocale( cuT( "receiver" )
					, 0_i );
				auto reflection = writer.declLocale( cuT( "reflection" )
					, 0_i );
				auto refraction = writer.declLocale( cuT( "refraction" )
					, 0_i );
				decodeMaterial( writer
					, flags
					, receiver
					, reflection
					, refraction
					, envMapIndex );

				auto data5 = writer.declLocale( cuT( "data5" )
					, texture( c3d_mapData5, vtx_texture ) );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );
				auto material = writer.declLocale( cuT( "material" )
					, materials.getMaterial( materialId ) );
				auto data2 = writer.declLocale( cuT( "data2" )
					, texture( c3d_mapData2, vtx_texture ) );
				auto data3 = writer.declLocale( cuT( "data3" )
					, texture( c3d_mapData3, vtx_texture ) );
				auto data4 = writer.declLocale( cuT( "data4" )
					, texture( c3d_mapData4, vtx_texture ) );
				auto normal = writer.declLocale( cuT( "normal" )
					, data1.xyz() );
				auto diffuse = writer.declLocale( cuT( "diffuse" )
					, data2.xyz() );
				auto glossiness = writer.declLocale( cuT( "glossiness" )
					, data2.w() );
				auto specular = writer.declLocale( cuT( "specular" )
					, data3.xyz() );
				auto occlusion = writer.declLocale( cuT( "occlusion" )
					, data3.a() );
				auto emissive = writer.declLocale( cuT( "emissive" )
					, data4.xyz() );
				auto depth = writer.declLocale( cuT( "depth" )
					, texture( c3d_mapDepth, vtx_texture, 0.0_f ).x() );
				auto position = writer.declLocale( cuT( "position" )
					, utils.calcWSPosition( vtx_texture, depth, c3d_mtxInvViewProj ) );
				auto incident = writer.declLocale( cuT( "incident" )
					, normalize( position - c3d_cameraPosition ) );
				auto ambient = writer.declLocale( cuT( "ambient" )
					, c3d_ambientLight.xyz() );
				auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
					, texture( c3d_mapLightDiffuse, vtx_texture ).xyz() );
				auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
					, texture( c3d_mapLightSpecular, vtx_texture ).xyz() );
				auto ratio = writer.declLocale( cuT( "ratio" )
					, material.m_refractionRatio() );

				if ( hasSsao )
				{
					occlusion *= texture( c3d_mapSsao, vtx_texture ).r();
				}

				IF( writer, envMapIndex > 0_i )
				{
					envMapIndex = envMapIndex - 1_i;

					IF( writer, reflection != 0_i && refraction != 0_i )
					{
						auto reflected = writer.declLocale( cuT( "reflected" )
							, reflect( incident, normal ) );
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* texture( c3d_mapEnvironment[envMapIndex], reflected ).xyz() * diffuse / length( diffuse );
						auto roughness = writer.declLocale( cuT( "roughness" )
							, 1.0_f - glossiness );
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						ambient = mix( texture( c3d_mapEnvironment[envMapIndex], refracted ).xyz() * diffuse / length( diffuse )
							, ambient
							, fresnel );
					}
					ELSEIF( writer, reflection != 0_i && ratio != 0.0_f )
					{
						auto reflected = writer.declLocale( cuT( "reflected" )
							, reflect( incident, normal ) );
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* texture( c3d_mapEnvironment[envMapIndex], reflected ).xyz() * diffuse / length( diffuse );
						auto roughness = writer.declLocale( cuT( "roughness" )
							, 1.0_f - glossiness );
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						refracted.y() = -refracted.y();
						ambient = mix( texture( c3d_mapPrefiltered, refracted, roughness * Utils::MaxIblReflectionLod ).xyz() * diffuse / length( diffuse )
							, ambient
							, fresnel );
					}
					ELSEIF( writer, reflection != 0_i )
					{
						auto reflected = writer.declLocale( cuT( "reflected" )
							, reflect( incident, normal ) );
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* texture( c3d_mapEnvironment[envMapIndex], reflected ).xyz();
					}
					ELSEIF( writer, refraction != 0_i )
					{
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* utils.computeSpecularIBL( normal
								, position
								, diffuse
								, specular
								, glossiness
								, c3d_cameraPosition
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );
						auto roughness = writer.declLocale( cuT( "roughness" )
							, 1.0_f - glossiness );
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						ambient = mix( texture( c3d_mapEnvironment[envMapIndex], refracted ).xyz() * diffuse / length( diffuse )
							, ambient
							, fresnel );
					}
					ELSEIF( writer, ratio != 0.0_f )
					{
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* utils.computeSpecularIBL( normal
								, position
								, diffuse
								, specular
								, glossiness
								, c3d_cameraPosition
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );
						auto roughness = writer.declLocale( cuT( "roughness" )
							, 1.0_f - glossiness );
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						refracted.y() = -refracted.y();
						ambient = mix( texture( c3d_mapPrefiltered, refracted, roughness * Utils::MaxIblReflectionLod ).xyz() * diffuse / length( diffuse )
							, ambient
							, fresnel );
					}
					ELSE
					{
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* utils.computeSpecularIBL( normal
								, position
								, diffuse
								, specular
								, glossiness
								, c3d_cameraPosition
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );
					}
					FI;
				}
				ELSEIF( writer, ratio != 0.0_f )
				{
					ambient = c3d_ambientLight.xyz()
						* occlusion
						* utils.computeSpecularIBL( normal
							, position
							, diffuse
							, specular
							, glossiness
							, c3d_cameraPosition
							, c3d_mapIrradiance
							, c3d_mapPrefiltered
							, c3d_mapBrdf );
					auto ratio = writer.declLocale( cuT( "ratio" )
						, material.m_refractionRatio() );
					auto roughness = writer.declLocale( cuT( "roughness" )
						, 1.0_f - glossiness );
					auto subRatio = writer.declLocale( cuT( "subRatio" )
						, 1.0_f - ratio );
					auto addRatio = writer.declLocale( cuT( "addRatio" )
						, 1.0_f + ratio );
					auto reflectance = writer.declLocale( cuT( "reflectance" )
						, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
					auto product = writer.declLocale( cuT( "product" )
						, max( 0.0_f, dot( -incident, normal ) ) );
					auto fresnel = writer.declLocale( cuT( "fresnel" )
						, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
					auto refracted = writer.declLocale( cuT( "refract" )
						, refract( incident, normal, ratio ) );
					refracted.y() = writer.ternary( envMapIndex != 0_i, refracted.y(), -refracted.y() );
					ambient = mix( texture( c3d_mapPrefiltered, refracted, roughness * Utils::MaxIblReflectionLod ).xyz() * diffuse / length( diffuse )
						, ambient
						, fresnel );
				}
				ELSE
				{
					ambient = c3d_ambientLight.xyz()
						* occlusion
						* utils.computeSpecularIBL( normal
							, position
							, diffuse
							, specular
							, glossiness
							, c3d_cameraPosition
							, c3d_mapIrradiance
							, c3d_mapPrefiltered
							, c3d_mapBrdf );
					auto ratio = writer.declLocale( cuT( "ratio" )
						, material.m_refractionRatio() );
				}
				FI;

#if !C3D_DEBUG_SSS_TRANSMITTANCE
				pxl_fragColor = vec4( lightDiffuse * diffuse + lightSpecular + emissive + ambient, 1.0 );
#else
				pxl_fragColor = vec4( lightDiffuse, 1.0 );
#endif

				if ( fogType != FogType::eDisabled )
				{
					position = utils.calcVSPosition( vtx_texture
							, texture( c3d_mapDepth, vtx_texture ).r()
							, c3d_mtxInvProj );
					fog.applyFog( pxl_fragColor, length( position ), position.z() );
				}
			} );
			return writer.finalise();
		}

		renderer::ShaderStageStateArray doCreateProgram( Engine & engine
			, FogType fogType
			, bool hasSsao
			, MaterialType matType )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto vtx = doCreateVertexProgram( renderSystem );
			auto pxl = matType == MaterialType::eLegacy
				? doCreateLegacyPixelProgram( renderSystem, fogType, hasSsao )
				: matType == MaterialType::ePbrMetallicRoughness
					? doCreatePbrMRPixelProgram( renderSystem, fogType, hasSsao )
					: doCreatePbrSGPixelProgram( renderSystem, fogType, hasSsao );

			auto & device = *renderSystem.getCurrentDevice();
			renderer::ShaderStageStateArray result
			{
				{ device.createShaderModule( renderer::ShaderStageFlag::eVertex ) },
				{ device.createShaderModule( renderer::ShaderStageFlag::eFragment ) }
			};
			result[0].module->loadShader( vtx.getSource() );
			result[1].module->loadShader( pxl.getSource() );
			return result;
		}

		inline renderer::DescriptorSetLayoutPtr doCreateUboDescriptorLayout( Engine & engine )
		{
			renderer::DescriptorSetLayoutBindingArray bindings
			{
				{ SceneUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
				{ GpInfoUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
			};
			return engine.getRenderSystem()->getCurrentDevice()->createDescriptorSetLayout( std::move( bindings ) );
		}

		inline renderer::DescriptorSetPtr doCreateUboDescriptorSet( renderer::DescriptorSetPool & pool
			, SceneUbo const & sceneUbo
			, GpInfoUbo const & gpInfoUbo )
		{
			auto & layout = pool.getLayout();
			auto result = pool.createDescriptorSet( 0u );
			result->createBinding( layout.getBinding( SceneUbo::BindingPoint )
				, sceneUbo.getUbo() );
			result->createBinding( layout.getBinding( GpInfoUbo::BindingPoint )
				, gpInfoUbo.getUbo() );
			result->update();
			return result;
		}

		inline renderer::RenderPassPtr doCreateRenderPass( Engine & engine
			, renderer::Format format )
		{
			renderer::RenderPassCreateInfo renderPass{};
			renderPass.flags = 0u;

			renderPass.attachments.resize( 1u );
			renderPass.attachments[0].index = 0u;
			renderPass.attachments[0].format = format;
			renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
			renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
			renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
			renderPass.attachments[0].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

			renderer::AttachmentReference colourReference;
			colourReference.attachment = 0u;
			colourReference.layout = renderer::ImageLayout::eColourAttachmentOptimal;

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].flags = 0u;
			renderPass.subpasses[0].colorAttachments = { colourReference };

			renderPass.dependencies.resize( 2u );
			renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[0].dstSubpass = 0u;
			renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
			renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eMemoryRead;
			renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

			renderPass.dependencies[1].srcSubpass = 0u;
			renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
			renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eMemoryRead;
			renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

			return engine.getRenderSystem()->getCurrentDevice()->createRenderPass( renderPass );
		}

		inline renderer::FrameBufferPtr doCreateFrameBuffer( renderer::RenderPass const & renderPass
			, renderer::Extent2D const size
			, renderer::TextureView const & view )
		{
			renderer::FrameBufferAttachmentArray attaches
			{
				{ *renderPass.getAttachments().begin(), view }
			};
			return renderPass.createFrameBuffer( size, std::move( attaches ) );
		}

		inline renderer::DescriptorSetLayoutPtr doCreateTexDescriptorLayout( Engine & engine
			, bool hasSsao
			, MaterialType matType )
		{
			renderer::DescriptorSetLayoutBindingArray bindings
			{
				{ 0, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },	// DsTexture::eDepth
				{ 1, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },	// DsTexture::eData1
				{ 2, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },	// DsTexture::eData2
				{ 3, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },	// DsTexture::eData3
				{ 4, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },	// DsTexture::eData4
				{ 5, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },	// DsTexture::eData5
			};
			uint32_t index = 6u;

			if ( hasSsao )
			{
				bindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );	// c3d_mapSsao
			}

			bindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );	// c3d_mapLightDiffuse
			bindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );	// c3d_mapLightSpecular

			if ( matType != MaterialType::eLegacy )
			{
				bindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );	// c3d_mapBrdf
				bindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );	// c3d_mapIrradiance
				bindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );	// c3d_mapPrefiltered
			}

			bindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment, c_environmentCount );	// c3d_mapEnvironment
			return engine.getRenderSystem()->getCurrentDevice()->createDescriptorSetLayout( std::move( bindings ) );
		}

		inline renderer::DescriptorSetPtr doCreateTexDescriptorSet( renderer::DescriptorSetPool & pool
			, GeometryPassResult const & gp
			, renderer::TextureView const & lightDiffuse
			, renderer::TextureView const & lightSpecular
			, renderer::TextureView const * ssao
			, Scene const & scene
			, MaterialType matType )
		{
			uint32_t index = 0u;
			auto & layout = pool.getLayout();
			auto result = pool.createDescriptorSet( 1u );
			result->createBinding( layout.getBinding( index++ )
				, gp[size_t( DsTexture::eDepth )]->getTexture()->getDefaultView()
				, gp[size_t( DsTexture::eDepth )]->getSampler()->getSampler() );
			result->createBinding( layout.getBinding( index++ )
				, gp[size_t( DsTexture::eData1 )]->getTexture()->getDefaultView()
				, gp[size_t( DsTexture::eData1 )]->getSampler()->getSampler() );
			result->createBinding( layout.getBinding( index++ )
				, gp[size_t( DsTexture::eData2 )]->getTexture()->getDefaultView()
				, gp[size_t( DsTexture::eData2 )]->getSampler()->getSampler() );
			result->createBinding( layout.getBinding( index++ )
				, gp[size_t( DsTexture::eData3 )]->getTexture()->getDefaultView()
				, gp[size_t( DsTexture::eData3 )]->getSampler()->getSampler() );
			result->createBinding( layout.getBinding( index++ )
				, gp[size_t( DsTexture::eData4 )]->getTexture()->getDefaultView()
				, gp[size_t( DsTexture::eData4 )]->getSampler()->getSampler() );
			result->createBinding( layout.getBinding( index++ )
				, gp[size_t( DsTexture::eData5 )]->getTexture()->getDefaultView()
				, gp[size_t( DsTexture::eData5 )]->getSampler()->getSampler() );

			if ( ssao )
			{
				result->createBinding( layout.getBinding( index++ )
					, *ssao
					, gp[size_t( DsTexture::eData5 )]->getSampler()->getSampler() );
			}

			result->createBinding( layout.getBinding( index++ )
				, lightDiffuse
				, gp[size_t( DsTexture::eData4 )]->getSampler()->getSampler() );
			result->createBinding( layout.getBinding( index++ )
				, lightSpecular
				, gp[size_t( DsTexture::eData5 )]->getSampler()->getSampler() );

			if ( matType != MaterialType::eLegacy )
			{
				auto & skyboxIbl = scene.getSkybox().getIbl();
				result->createBinding( layout.getBinding( index++ )
					, skyboxIbl.getPrefilteredBrdf().getTexture()->getDefaultView()
					, skyboxIbl.getPrefilteredBrdf().getSampler()->getSampler() );
				result->createBinding( layout.getBinding( index++ )
					, skyboxIbl.getIrradiance().getTexture()->getDefaultView()
					, skyboxIbl.getIrradiance().getSampler()->getSampler() );
				result->createBinding( layout.getBinding( index++ )
					, skyboxIbl.getPrefilteredEnvironment().getTexture()->getDefaultView()
					, skyboxIbl.getPrefilteredEnvironment().getSampler()->getSampler() );
			}

			result->update();
			return result;
		}

		renderer::PipelinePtr doCreateRenderPipeline( renderer::PipelineLayout const & pipelineLayout
			, renderer::ShaderStageStateArray program
			, renderer::RenderPass const & renderPass
			, renderer::Extent2D const & size )
		{
			renderer::RasterisationState rsState;
			rsState.cullMode = renderer::CullModeFlag::eNone;
			renderer::DepthStencilState dsState{ 0u, false, false };
			renderer::ColourBlendState blState = renderer::ColourBlendState::createDefault();
			return pipelineLayout.createPipeline(
			{
				program,
				renderPass,
				renderer::VertexInputState::create( *doCreateVertexLayout() ),
				renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
				rsState,
				renderer::MultisampleState{},
				blState,
				{},
				dsState,
				std::nullopt,
				renderer::Viewport{ size.width, size.height, 0, 0 },
				renderer::Scissor{ 0, 0, size.width, size.height }
			} );
		}

		template< typename T >
		struct Reverse
		{
			explicit Reverse( T const & collection )
				: m_collection{ collection }
			{
			}

			auto begin()const
			{
				return m_collection.rbegin();
			}

			auto end()const
			{
				return m_collection.rend();
			}

		private:
			T const & m_collection;
		};

		template< typename T >
		Reverse< T > makeReverse( T const & collection )
		{
			return Reverse< T >( collection );
		}
	}

	//*********************************************************************************************

	ReflectionPass::ProgramPipeline::ProgramPipeline( Engine & engine
		, Scene const & scene
		, renderer::VertexBufferBase & vbo
		, renderer::DescriptorSetLayout const & uboLayout
		, renderer::DescriptorSet const & uboSet
		, renderer::RenderPass const & renderPass
		, renderer::FrameBuffer const & frameBuffer
		, GeometryPassResult const & gp
		, renderer::TextureView const & lightDiffuse
		, renderer::TextureView const & lightSpecular
		, renderer::TextureView const * ssao
		, renderer::Extent2D const & size
		, FogType fogType
		, MaterialType matType )
		: m_program{ doCreateProgram( engine, fogType, ssao != nullptr, matType ) }
		, m_texDescriptorLayout{ doCreateTexDescriptorLayout( engine, ssao != nullptr, matType ) }
		, m_texDescriptorPool{ m_texDescriptorLayout->createPool( 1u ) }
		, m_texDescriptorSet{ doCreateTexDescriptorSet( *m_texDescriptorPool, gp, lightDiffuse, lightSpecular, ssao, scene, matType ) }
		, m_pipelineLayout{ engine.getRenderSystem()->getCurrentDevice()->createPipelineLayout( { uboLayout, *m_texDescriptorLayout } ) }
		, m_pipeline{ doCreateRenderPipeline( *m_pipelineLayout, m_program, renderPass, size ) }
		, m_commandBuffer{ engine.getRenderSystem()->getCurrentDevice()->getGraphicsCommandPool().createCommandBuffer( true ) }
	{
		static renderer::ClearColorValue const clear{ 0.0, 0.0, 0.0, 0.0 };

		if ( m_commandBuffer->begin() )
		{
			m_commandBuffer->beginRenderPass( renderPass
				, frameBuffer
				, { clear }
				, renderer::SubpassContents::eInline );
			m_commandBuffer->bindPipeline( *m_pipeline );
			m_commandBuffer->bindDescriptorSets( { uboSet, *m_texDescriptorSet }, *m_pipelineLayout );
			m_commandBuffer->bindVertexBuffer( 0u, vbo.getBuffer(), 0u );
			m_commandBuffer->draw( 6u );
			m_commandBuffer->endRenderPass();
			m_commandBuffer->end();
		}
	}

	//*********************************************************************************************

	ReflectionPass::ReflectionPass( Engine & engine
		, Scene const & scene
		, GeometryPassResult const & gp
		, renderer::TextureView const & lightDiffuse
		, renderer::TextureView const & lightSpecular
		, renderer::TextureView const & result
		, SceneUbo & sceneUbo
		, GpInfoUbo & gpInfoUbo
		, SsaoConfig const & config
		, Viewport const & viewport )
		: OwnedBy< Engine >{ engine }
		, m_device{ *engine.getRenderSystem()->getCurrentDevice() }
		, m_scene{ scene }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_size{ result.getTexture().getDimensions().width, result.getTexture().getDimensions().height }
		, m_vertexBuffer{ doCreateVbo( engine ) }
		, m_uboDescriptorLayout{ doCreateUboDescriptorLayout( engine ) }
		, m_uboDescriptorPool{ m_uboDescriptorLayout->createPool( 1u ) }
		, m_uboDescriptorSet{ doCreateUboDescriptorSet( *m_uboDescriptorPool, sceneUbo, gpInfoUbo ) }
		, m_renderPass{ doCreateRenderPass( engine, result.getFormat() ) }
		, m_frameBuffer{ doCreateFrameBuffer( *m_renderPass, m_size, result ) }
		, m_finished{ m_device.createSemaphore() }
		, m_ssao{ engine, m_size, config, gp, viewport }
		, m_programs
		{
			{
				ProgramPipeline
				{
					engine,
					scene,
					*m_vertexBuffer,
					*m_uboDescriptorLayout,
					*m_uboDescriptorSet,
					*m_renderPass,
					*m_frameBuffer,
					gp,
					lightDiffuse,
					lightSpecular,
					config.m_enabled ? &m_ssao.getResult().getTexture()->getDefaultView() : nullptr,
					m_size,
					FogType::eDisabled,
					engine.getMaterialsType()
				},
				ProgramPipeline
				{
					engine,
					scene,
					*m_vertexBuffer,
					*m_uboDescriptorLayout,
					*m_uboDescriptorSet,
					*m_renderPass,
					*m_frameBuffer,
					gp,
					lightDiffuse,
					lightSpecular,
					config.m_enabled ? &m_ssao.getResult().getTexture()->getDefaultView() : nullptr,
					m_size,
					FogType::eLinear,
					engine.getMaterialsType()
				},
				ProgramPipeline
				{
					engine,
					scene,
					*m_vertexBuffer,
					*m_uboDescriptorLayout,
					*m_uboDescriptorSet,
					*m_renderPass,
					*m_frameBuffer,
					gp,
					lightDiffuse,
					lightSpecular,
					config.m_enabled ? &m_ssao.getResult().getTexture()->getDefaultView() : nullptr,
					m_size,
					FogType::eExponential,
					engine.getMaterialsType()
				},
				ProgramPipeline
				{
					engine,
					scene,
					*m_vertexBuffer,
					*m_uboDescriptorLayout,
					*m_uboDescriptorSet,
					*m_renderPass,
					*m_frameBuffer,
					gp,
					lightDiffuse,
					lightSpecular,
					config.m_enabled ? &m_ssao.getResult().getTexture()->getDefaultView() : nullptr,
					m_size,
					FogType::eSquaredExponential,
					engine.getMaterialsType()
				},
			}
		}
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "Reflection" ), cuT( "Reflection" ) ) }
		, m_ssaoEnabled{ config.m_enabled }
	{
		m_viewport.setOrtho( 0, 1, 0, 1, 0, 1 );
		m_viewport.resize( { m_size.width, m_size.height } );
		m_viewport.update();
	}

	void ReflectionPass::update( Camera const & camera )
	{
		if ( m_ssaoEnabled )
		{
			m_ssao.update( camera );
		}

	}

	void ReflectionPass::render( renderer::Semaphore const & toWait )const
	{
		renderer::Semaphore const * semaphore = &toWait;
		TextureUnit const * ssao = nullptr;

		if ( m_ssaoEnabled )
		{
			m_ssao.render( toWait );
			semaphore = &m_ssao.getSemaphore();
		}

		m_timer->start();
		auto program = size_t( m_scene.getFog().getType() );
		m_device.getGraphicsQueue().submit( *m_programs[program].m_commandBuffer
			, *semaphore
			, renderer::PipelineStageFlag::eColourAttachmentOutput
			, *m_finished
			, nullptr );
		m_timer->stop();
	}
}
