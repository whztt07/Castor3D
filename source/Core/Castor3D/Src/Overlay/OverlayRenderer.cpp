#include "OverlayRenderer.hpp"

#include "Engine.hpp"

#include "BorderPanelOverlay.hpp"
#include "PanelOverlay.hpp"

#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Graphics/Font.hpp>

#include <GlslSource.hpp>
#include "Shader/Shaders/GlslMaterial.hpp"

using namespace castor;

#if defined( drawText )
#	undef drawText
#endif

namespace castor3d
{
	static const int32_t C3D_MAX_CHARS_PER_BUFFER = 6000;

	namespace
	{
		uint32_t doFillBuffers( OverlayCategory::VertexArray::const_iterator begin
			, uint32_t count
			, VertexBuffer & vbo )
		{
			OverlayCategory::Vertex const & vertex = *begin;
			auto size = count * sizeof( OverlayCategory::Vertex );

			vbo.bind();
			vbo.upload( 0, uint32_t( size ), reinterpret_cast< uint8_t const * >( &vertex ) );
			vbo.unbind();
			return count;
		}

		uint32_t doFillBuffers( TextOverlay::VertexArray::const_iterator begin
			, uint32_t count
			, VertexBuffer & vbo )
		{
			TextOverlay::Vertex const & vertex = *begin;
			auto size = count * sizeof( TextOverlay::Vertex );

			vbo.bind();
			vbo.upload( 0, uint32_t( size ), reinterpret_cast< uint8_t const * >( &vertex ) );
			vbo.unbind();
			return count;
		}
	}

	OverlayRenderer::OverlayRenderer( RenderSystem & renderSystem )
		: OwnedBy< RenderSystem >( renderSystem )
		, m_declaration{
			{
				{
					BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eIVec2 ),
					BufferElementDeclaration( ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 )
				}
			} }
		, m_textDeclaration{
			{
				{
					BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eIVec2 ),
					BufferElementDeclaration( ShaderProgram::Text, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2, 0 ),
					BufferElementDeclaration( ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2, 1 )
				}
			} }
		, m_matrixUbo{ *renderSystem.getEngine() }
		, m_overlayUbo{ *renderSystem.getEngine() }
	{
	}

	OverlayRenderer::~OverlayRenderer()
	{
		for ( auto & vertex : m_borderVertex )
		{
			vertex.reset();
		}

		for ( auto & vertex : m_panelVertex )
		{
			vertex.reset();
		}
	}

	void OverlayRenderer::initialise()
	{
		if ( !m_panelVertexBuffer )
		{
			// Panel Overlays buffers
			m_panelVertexBuffer = std::make_shared< VertexBuffer >( *getRenderSystem()->getEngine(), m_declaration );
			uint32_t stride = m_declaration.stride();
			m_panelVertexBuffer->resize( uint32_t( m_panelVertex.size() * stride ) );
			uint8_t * buffer = m_panelVertexBuffer->getData();

			for ( auto & vertex : m_panelVertex )
			{
				vertex = std::make_shared< BufferElementGroup >( buffer );
				buffer += stride;
			}

			m_panelVertexBuffer->initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );

			{
				auto & pipeline = doGetPanelPipeline( 0u );
				m_panelGeometryBuffers.m_noTexture = getRenderSystem()->createGeometryBuffers( Topology::eTriangles, pipeline.getProgram() );
				m_panelGeometryBuffers.m_noTexture->initialise( { *m_panelVertexBuffer }, nullptr );
			}
			{
				auto & pipeline = doGetPanelPipeline( uint32_t( TextureChannel::eDiffuse ) );
				m_panelGeometryBuffers.m_textured = getRenderSystem()->createGeometryBuffers( Topology::eTriangles, pipeline.getProgram() );
				m_panelGeometryBuffers.m_textured->initialise( { *m_panelVertexBuffer }, nullptr );
			}
		}

		if ( !m_borderVertexBuffer )
		{
			// Border Overlays buffers
			m_borderVertexBuffer = std::make_shared< VertexBuffer >( *getRenderSystem()->getEngine(), m_declaration );
			uint32_t stride = m_declaration.stride();
			m_borderVertexBuffer->resize( uint32_t( m_borderVertex.size() * stride ) );
			uint8_t * buffer = m_borderVertexBuffer->getData();

			for ( auto & vertex : m_borderVertex )
			{
				vertex = std::make_shared< BufferElementGroup >( buffer );
				buffer += stride;
			}

			m_borderVertexBuffer->initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );

			{
				auto & pipeline = doGetPanelPipeline( 0 );
				m_borderGeometryBuffers.m_noTexture = getRenderSystem()->createGeometryBuffers( Topology::eTriangles, pipeline.getProgram() );
				m_borderGeometryBuffers.m_noTexture->initialise( { *m_borderVertexBuffer }, nullptr );
			}
			{
				auto & pipeline = doGetPanelPipeline( uint32_t( TextureChannel::eDiffuse ) );
				m_borderGeometryBuffers.m_textured = getRenderSystem()->createGeometryBuffers( Topology::eTriangles, pipeline.getProgram() );
				m_borderGeometryBuffers.m_textured->initialise( { *m_borderVertexBuffer }, nullptr );
			}
		}

		// create one text overlays buffer
		doCreateTextGeometryBuffers();
	}

	void OverlayRenderer::cleanup()
	{
		for ( auto & vertex : m_borderVertex )
		{
			vertex.reset();
		}

		for ( auto & vertex : m_panelVertex )
		{
			vertex.reset();
		}

		for ( auto & pair : m_pipelines )
		{
			pair.second->cleanup();
		}

		m_overlayUbo.getUbo().cleanup();
		m_matrixUbo.getUbo().cleanup();
		m_pipelines.clear();
		m_mapPanelNodes.clear();
		m_mapTextNodes.clear();

		if ( m_panelVertexBuffer )
		{
			m_panelGeometryBuffers.m_noTexture->cleanup();
			m_panelGeometryBuffers.m_textured->cleanup();
			m_panelGeometryBuffers.m_noTexture.reset();
			m_panelGeometryBuffers.m_textured.reset();
			m_panelVertexBuffer->cleanup();
			m_panelVertexBuffer.reset();
		}

		if ( m_borderVertexBuffer )
		{
			m_borderGeometryBuffers.m_noTexture->cleanup();
			m_borderGeometryBuffers.m_textured->cleanup();
			m_borderGeometryBuffers.m_noTexture.reset();
			m_borderGeometryBuffers.m_textured.reset();
			m_borderVertexBuffer->cleanup();
			m_borderVertexBuffer.reset();
		}

		for ( auto & buffer : m_textsGeometryBuffers )
		{
			buffer.m_noTexture->cleanup();
			buffer.m_textured->cleanup();
		}

		m_textsGeometryBuffers.clear();

		for ( auto buffer : m_textsVertexBuffers )
		{
			buffer->cleanup();
		}

		m_textsVertexBuffers.clear();
	}

	void OverlayRenderer::drawPanel( PanelOverlay & overlay )
	{
		MaterialSPtr material = overlay.getMaterial();

		if ( material )
		{
			m_overlayUbo.setPosition( overlay.getAbsolutePosition()
				, m_size
				, overlay.getRenderRatio( m_size ) );
			doDrawItem( *material
				, *m_panelVertexBuffer
				, m_panelGeometryBuffers
				, doFillBuffers( overlay.getPanelVertex().begin()
					, uint32_t( overlay.getPanelVertex().size() )
					, *m_panelVertexBuffer ) );
		}
	}

	void OverlayRenderer::drawBorderPanel( BorderPanelOverlay & overlay )
	{
		{
			auto material = overlay.getMaterial();

			if ( material )
			{
				m_overlayUbo.setPosition( overlay.getAbsolutePosition()
					, m_size
					, overlay.getRenderRatio( m_size ) );
				doDrawItem( *material
					, *m_panelVertexBuffer
					, m_panelGeometryBuffers
					, doFillBuffers( overlay.getPanelVertex().begin()
						, uint32_t( overlay.getPanelVertex().size() )
						, *m_panelVertexBuffer ) );
			}
		}
		{
			auto material = overlay.getBorderMaterial();

			if ( material )
			{
				m_overlayUbo.setPosition( overlay.getAbsolutePosition()
					, m_size
					, overlay.getRenderRatio( m_size ) );
				doDrawItem( *material
					, *m_borderVertexBuffer
					, m_borderGeometryBuffers
					, doFillBuffers( overlay.getBorderVertex().begin()
						, uint32_t( overlay.getBorderVertex().size() )
						, *m_borderVertexBuffer ) );
			}
		}
	}

	void OverlayRenderer::drawText( TextOverlay & overlay )
	{
		FontSPtr pFont = overlay.getFontTexture()->getFont();

		if ( pFont )
		{
			MaterialSPtr material = overlay.getMaterial();

			if ( material )
			{
				TextOverlay::VertexArray arrayVtx = overlay.getTextVertex();
				int32_t count = uint32_t( arrayVtx.size() );
				uint32_t index = 0;
				std::vector< OverlayGeometryBuffers > geometryBuffers;
				std::vector< VertexBufferSPtr > vertexBuffers;
				TextOverlay::VertexArray::const_iterator it = arrayVtx.begin();

				while ( count > C3D_MAX_CHARS_PER_BUFFER )
				{
					geometryBuffers.push_back( doFillTextPart( count, it, index ) );
					vertexBuffers.push_back( m_textsVertexBuffers[index] );
					++index;
					count -= C3D_MAX_CHARS_PER_BUFFER;
				}

				if ( count > 0 )
				{
					geometryBuffers.push_back( doFillTextPart( count, it, index ) );
					vertexBuffers.push_back( m_textsVertexBuffers[index] );
				}

				auto texture = overlay.getFontTexture()->getTexture();
				auto sampler = overlay.getFontTexture()->getSampler();
				count = uint32_t( arrayVtx.size() );
				m_overlayUbo.setPosition( overlay.getAbsolutePosition()
					, m_size
					, overlay.getRenderRatio( m_size ) );

				for ( auto pass : *material )
				{
					auto itV = vertexBuffers.begin();

					if ( checkFlag( pass->getTextureFlags(), TextureChannel::eDiffuse ) )
					{
						for ( auto geoBuffers : geometryBuffers )
						{
							doDrawItem( *pass
								, *( *itV )
								, *geoBuffers.m_textured
								, *texture
								, *sampler
								, std::min( count, C3D_MAX_CHARS_PER_BUFFER ) );
							count -= C3D_MAX_CHARS_PER_BUFFER;
							++itV;
						}
					}
					else
					{
						for ( auto geoBuffers : geometryBuffers )
						{
							doDrawItem( *pass
								, *( *itV )
								, *geoBuffers.m_noTexture
								, *texture
								, *sampler
								, std::min( count, C3D_MAX_CHARS_PER_BUFFER ) );
							count -= C3D_MAX_CHARS_PER_BUFFER;
							++itV;
						}
					}
				}
			}
		}
	}

	void OverlayRenderer::beginRender( Viewport const & viewport )
	{
		auto size = viewport.getSize();

		if ( m_size != size )
		{
			m_sizeChanged = true;
			m_size = size;
		}

		m_matrixUbo.update( viewport.getProjection() );
	}

	void OverlayRenderer::endRender()
	{
		m_sizeChanged = false;
	}

	OverlayRenderer::OverlayRenderNode & OverlayRenderer::doGetPanelNode( Pass & pass )
	{
		auto it = m_mapPanelNodes.find( &pass );

		if ( it == m_mapPanelNodes.end() )
		{
			auto & pipeline = doGetPanelPipeline( pass.getTextureFlags() );
			it = m_mapPanelNodes.insert( { &pass, OverlayRenderNode
				{
					pipeline,
					{
						pass,
						pipeline.getProgram(),
					},
					m_overlayUbo,
				}
			} ).first;
		}

		return it->second;
	}

	OverlayRenderer::OverlayRenderNode & OverlayRenderer::doGetTextNode( Pass & pass )
	{
		auto it = m_mapTextNodes.find( &pass );

		if ( it == m_mapTextNodes.end() )
		{
			auto & pipeline = doGetTextPipeline( pass.getTextureFlags() );
			it = m_mapTextNodes.insert( { &pass, OverlayRenderNode
				{
					pipeline,
					{
						pass,
						pipeline.getProgram(),
					},
					m_overlayUbo,
				}
			} ).first;
		}

		return it->second;
	}

	RenderPipeline & OverlayRenderer::doGetPanelPipeline( TextureChannels textureFlags )
	{
		// Remove unwanted flags
		remFlag( textureFlags, TextureChannel::eNormal );
		remFlag( textureFlags, TextureChannel::eSpecular );
		remFlag( textureFlags, TextureChannel::eGloss );
		remFlag( textureFlags, TextureChannel::eHeight );
		remFlag( textureFlags, TextureChannel::eEmissive );

		// get shader
		return doGetPipeline( textureFlags );
	}

	RenderPipeline & OverlayRenderer::doGetTextPipeline( TextureChannels textureFlags )
	{
		// Remove unwanted flags
		remFlag( textureFlags, TextureChannel::eNormal );
		remFlag( textureFlags, TextureChannel::eSpecular );
		remFlag( textureFlags, TextureChannel::eGloss );
		remFlag( textureFlags, TextureChannel::eHeight );
		remFlag( textureFlags, TextureChannel::eEmissive );
		addFlag( textureFlags, TextureChannel::eText );

		// get shader
		return doGetPipeline( textureFlags );
	}

	RenderPipeline & OverlayRenderer::doGetPipeline( TextureChannels const & textureFlags )
	{
		auto it = m_pipelines.find( textureFlags );

		if ( it == m_pipelines.end() )
		{
			// Since it does not exist yet, create it and initialise it
			auto program = doCreateOverlayProgram( textureFlags );

			if ( program )
			{
				program->initialise();
				BlendState blState;
				blState.setAlphaSrcBlend( BlendOperand::eSrcAlpha );
				blState.setAlphaDstBlend( BlendOperand::eInvSrcAlpha );
				blState.setRgbSrcBlend( BlendOperand::eSrcAlpha );
				blState.setRgbDstBlend( BlendOperand::eInvSrcAlpha );
				blState.enableBlend( true );

				MultisampleState msState;
				msState.enableAlphaToCoverage( false );

				DepthStencilState dsState;
				dsState.setDepthMask( WritingMask::eZero );
				dsState.setDepthTest( false );

				RasteriserState rsState;
				rsState.setCulledFaces( Culling::eBack );

				auto pipeline = getRenderSystem()->createRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, std::move( blState )
					, std::move( msState )
					, *program
					, PipelineFlags{} );
				pipeline->addUniformBuffer( m_matrixUbo.getUbo() );
				pipeline->addUniformBuffer( m_overlayUbo.getUbo() );
				it = m_pipelines.emplace( textureFlags, std::move( pipeline ) ).first;
			}
			else
			{
				CASTOR_EXCEPTION( "Couldn't create an overlay shader program" );
			}
		}

		return *it->second;
	}

	OverlayRenderer::OverlayGeometryBuffers OverlayRenderer::doCreateTextGeometryBuffers()
	{
		auto vertexBuffer = std::make_shared< VertexBuffer >( *getRenderSystem()->getEngine(), m_textDeclaration );
		vertexBuffer->resize( C3D_MAX_CHARS_PER_BUFFER * m_textDeclaration.stride() );
		vertexBuffer->initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );

		OverlayGeometryBuffers geometryBuffers;
		{
			auto & pipeline = doGetTextPipeline( 0 );
			geometryBuffers.m_noTexture = getRenderSystem()->createGeometryBuffers( Topology::eTriangles, pipeline.getProgram() );
			geometryBuffers.m_noTexture->initialise( { *vertexBuffer }, nullptr );
		}
		{
			auto & pipeline = doGetTextPipeline( uint32_t( TextureChannel::eDiffuse ) );
			geometryBuffers.m_textured = getRenderSystem()->createGeometryBuffers( Topology::eTriangles, pipeline.getProgram() );
			geometryBuffers.m_textured->initialise( { *vertexBuffer }, nullptr );
		}

		m_textsVertexBuffers.push_back( std::move( vertexBuffer ) );
		m_textsGeometryBuffers.push_back( geometryBuffers );
		return geometryBuffers;
	}

	void OverlayRenderer::doDrawItem( Pass & pass
		, VertexBuffer const & vertexBuffer
		, GeometryBuffers const & geometryBuffers
		, uint32_t count )
	{
		auto & node = doGetPanelNode( pass );
		m_overlayUbo.update( pass.getId() );
		node.m_pipeline.apply();
		pass.bindTextures();
		geometryBuffers.draw( count, 0u );
		pass.unbindTextures();
	}

	void OverlayRenderer::doDrawItem( Pass & pass
		, VertexBuffer const & vertexBuffer
		, GeometryBuffers const & geometryBuffers
		, TextureLayout const & texture
		, Sampler const & sampler
		, uint32_t count )
	{
		auto & node = doGetTextNode( pass );

		auto textureVariable = node.m_pipeline.getProgram().findUniform< UniformType::eSampler >( ShaderProgram::MapText, ShaderType::ePixel );

		if ( textureVariable )
		{
			textureVariable->setValue( LightBufferIndex );
		}

		m_overlayUbo.update( pass.getId() );
		node.m_pipeline.apply();
		pass.bindTextures();
		texture.bind( LightBufferIndex );
		sampler.bind( LightBufferIndex );
		geometryBuffers.draw( count, 0u );
		sampler.unbind( LightBufferIndex );
		texture.unbind( LightBufferIndex );
		pass.unbindTextures();
	}

	void OverlayRenderer::doDrawItem( Material & material
		, VertexBuffer const & vertexBuffer
		, OverlayRenderer::OverlayGeometryBuffers const & geometryBuffers
		, uint32_t count )
	{
		for ( auto pass : material )
		{
			if ( checkFlag( pass->getTextureFlags(), TextureChannel::eDiffuse ) )
			{
				doDrawItem( *pass
					, vertexBuffer
					, *geometryBuffers.m_textured
					, count );
			}
			else
			{
				doDrawItem( *pass
					, vertexBuffer
					, *geometryBuffers.m_noTexture
					, count );
			}
		}
	}

	OverlayRenderer::OverlayGeometryBuffers OverlayRenderer::doFillTextPart( int32_t count
		, TextOverlay::VertexArray::const_iterator & it
		, uint32_t index )
	{
		OverlayGeometryBuffers geometryBuffers;

		if ( m_textsGeometryBuffers.size() <= index )
		{
			doCreateTextGeometryBuffers();
		}

		if ( m_textsGeometryBuffers.size() > index )
		{
			geometryBuffers = m_textsGeometryBuffers[index];
			auto & vertexBuffer = m_textsVertexBuffers[index];
			count = std::min( count, C3D_MAX_CHARS_PER_BUFFER );
			doFillBuffers( it, count, *vertexBuffer );
			it += count;
		}
		else
		{
			CASTOR_EXCEPTION( "Bad index for geometry buffers" );
		}

		return geometryBuffers;
	}

	ShaderProgramSPtr OverlayRenderer::doCreateOverlayProgram( TextureChannels const & textureFlags )
	{
		using namespace glsl;
		using namespace shader;

		// Shader program
		auto & cache = getRenderSystem()->getEngine()->getShaderProgramCache();
		ShaderProgramSPtr program = cache.getNewProgram( false );
		program->createObject( ShaderType::eVertex );
		program->createObject( ShaderType::ePixel );

		// Vertex shader
		glsl::Shader strVs;
		{
			auto writer = getRenderSystem()->createGlslWriter();

			UBO_MATRIX( writer );
			UBO_OVERLAY( writer );

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( ShaderProgram::Position );
			auto text = writer.declAttribute< Vec2 >( ShaderProgram::Text, checkFlag( textureFlags, TextureChannel::eText ) );
			auto texture = writer.declAttribute< Vec2 >( ShaderProgram::Texture, checkFlag( textureFlags, TextureChannel::eDiffuse ) );

			// Shader outputs
			auto vtx_text = writer.declOutput< Vec2 >( cuT( "vtx_text" ), checkFlag( textureFlags, TextureChannel::eText ) );
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), checkFlag( textureFlags, TextureChannel::eDiffuse ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				if ( checkFlag( textureFlags, TextureChannel::eText ) )
				{
					vtx_text = text;
				}

				if ( checkFlag( textureFlags, TextureChannel::eDiffuse ) )
				{
					vtx_texture = texture;
				}

				auto size = writer.declLocale( cuT( "size" )
					, c3d_renderRatio * c3d_renderSize );
				gl_Position = c3d_projection * vec4( size * writer.paren( c3d_position + position )
					, 0.0
					, 1.0 );
			} );

			strVs = writer.finalise();
		}

		// Pixel shader
		glsl::Shader strPs;
		{
			auto writer = getRenderSystem()->createGlslWriter();

			std::unique_ptr< Materials > materials;

			switch ( getRenderSystem()->getEngine()->getMaterialsType() )
			{
			case MaterialType::eLegacy:
				materials = std::make_unique< shader::LegacyMaterials >( writer );
				break;

			case MaterialType::ePbrMetallicRoughness:
				materials = std::make_unique< PbrMRMaterials >( writer );
				break;

			case MaterialType::ePbrSpecularGlossiness:
				materials = std::make_unique< PbrSGMaterials >( writer );
				break;
			}

			materials->declare();
			UBO_OVERLAY( writer );

			// Shader inputs
			auto vtx_text = writer.declInput< Vec2 >( cuT( "vtx_text" )
				, checkFlag( textureFlags, TextureChannel::eText ) );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" )
				, checkFlag( textureFlags, TextureChannel::eDiffuse ) );
			auto c3d_mapText = writer.declSampler< Sampler2D >( ShaderProgram::MapText
				, LightBufferIndex + 0u
				, checkFlag( textureFlags, TextureChannel::eText ) );
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( ShaderProgram::MapDiffuse
				, LightBufferIndex + 1u
				, checkFlag( textureFlags, TextureChannel::eDiffuse ) );
			auto c3d_mapOpacity = writer.declSampler< Sampler2D >( ShaderProgram::MapOpacity
				, LightBufferIndex + 2u
				, checkFlag( textureFlags, TextureChannel::eOpacity ) );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto material = materials->getBaseMaterial( c3d_materialIndex );
				auto diffuse = writer.declLocale( cuT( "diffuse" )
					, material->m_diffuse() );
				auto alpha = writer.declLocale( cuT( "alpha" )
					, material->m_opacity() );

				if ( checkFlag( textureFlags, TextureChannel::eText ) )
				{
					alpha *= texture( c3d_mapText, vtx_text ).r();
				}

				if ( checkFlag( textureFlags, TextureChannel::eDiffuse ) )
				{
					diffuse = texture( c3d_mapDiffuse, vtx_texture ).xyz();
				}

				if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, vtx_texture ).r();
				}

				pxl_fragColor = vec4( diffuse.xyz(), alpha );
			} );

			strPs = writer.finalise();
		}

		if ( checkFlag( textureFlags, TextureChannel::eText ) )
		{
			program->createUniform< UniformType::eSampler >( ShaderProgram::MapText, ShaderType::ePixel )->setValue( LightBufferIndex );
		}

		auto index = MinTextureIndex;

		if ( checkFlag( textureFlags, TextureChannel::eDiffuse ) )
		{
			program->createUniform< UniformType::eSampler >( ShaderProgram::MapDiffuse, ShaderType::ePixel )->setValue( index++ );
		}

		if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
		{
			program->createUniform< UniformType::eSampler >( ShaderProgram::MapOpacity, ShaderType::ePixel )->setValue( index++ );
		}

		program->setSource( ShaderType::eVertex, strVs );
		program->setSource( ShaderType::ePixel, strPs );
		program->initialise();
		return program;
	}
}
