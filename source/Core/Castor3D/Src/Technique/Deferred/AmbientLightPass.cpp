#include "AmbientLightPass.hpp"

#include <Engine.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Light/DirectionalLight.hpp>
#include <Shader/SceneUbo.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>
#include <GlslUtils.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Castor3D
{
	//*********************************************************************************************

	namespace
	{
		static constexpr uint32_t VertexCount = 6u;
	}

	//*********************************************************************************************

	AmbientLightPass::Program::Program( Engine & p_engine
		, String const & p_vtx
		, String const & p_pxl )
		: LightPass::Program{ p_engine, p_vtx, p_pxl }
	{
	}

	AmbientLightPass::Program::~Program()
	{
	}

	RenderPipelineUPtr AmbientLightPass::Program::DoCreatePipeline( bool p_blend )
	{
		DepthStencilState l_dsstate;
		l_dsstate.SetDepthTest( false );
		l_dsstate.SetDepthMask( WritingMask::eZero );
		BlendState l_blstate;

		if ( p_blend )
		{
			l_blstate.EnableBlend( true );
			l_blstate.SetBlendOp( BlendOperation::eAdd );
			l_blstate.SetSrcBlend( BlendOperand::eOne );
			l_blstate.SetDstBlend( BlendOperand::eOne );
		}

		return m_program->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate )
			, RasteriserState{}
			, std::move( l_blstate )
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );
	}

	void AmbientLightPass::Program::DoBind( Light const & p_light )
	{
	}

	//*********************************************************************************************

	AmbientLightPass::AmbientLightPass( Engine & p_engine
		, FrameBuffer & p_frameBuffer
		, FrameBufferAttachment & p_depthAttach )
		: LightPass{ p_engine, p_frameBuffer, p_depthAttach, false }
		, m_viewport{ p_engine }
	{
		auto l_declaration = BufferDeclaration(
		{
			BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
		} );

		real l_data[] =
		{
			0, 0,
			1, 1,
			0, 1,
			0, 0,
			1, 0,
			1, 1,
		};

		m_vertexBuffer = std::make_shared< VertexBuffer >( m_engine, l_declaration );
		uint32_t l_stride = l_declaration.stride();
		m_vertexBuffer->Resize( sizeof( l_data ) );
		uint8_t * l_buffer = m_vertexBuffer->data();
		std::memcpy( l_buffer, l_data, sizeof( l_data ) );
		m_viewport.SetOrtho( 0, 1, 0, 1, 0, 1 );
		m_vertexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_viewport.Initialise();
	}

	AmbientLightPass::~AmbientLightPass()
	{
		m_vertexBuffer->Cleanup();
		m_vertexBuffer.reset();
		m_viewport.Cleanup();
		m_matrixUbo.GetUbo().Cleanup();
	}

	void AmbientLightPass::Initialise( Scene const & p_scene
		, SceneUbo & p_sceneUbo )
	{
		DoInitialise( p_scene
			, LightType::eDirectional
			, *m_vertexBuffer
			, nullptr
			, p_sceneUbo
			, nullptr );
		m_viewport.Update();
	}

	void AmbientLightPass::Cleanup()
	{
		DoCleanup();
	}

	void AmbientLightPass::Render( Size const & p_size
		, GeometryPassResult const & p_gp
		, Camera const & p_camera
		, Matrix4x4r const & p_invViewProj
		, Castor::Matrix4x4r const & p_invView
		, Castor::Matrix4x4r const & p_invProj
		, bool p_first )
	{
		m_gpInfo->Update( p_size
			, p_camera
			, p_invViewProj
			, p_invView
			, p_invProj );

		m_viewport.Resize( p_size );
		m_matrixUbo.Update( p_camera.GetView(), m_viewport.GetProjection() );

		DoRender( p_size
			, p_gp
			, rgb_float( p_camera.GetScene()->GetAmbientLight() )
			, p_first );
	}

	uint32_t AmbientLightPass::GetCount()const
	{
		return VertexCount;
	}

	void AmbientLightPass::DoUpdate( Size const & p_size
		, Light const & p_light
		, Camera const & p_camera )
	{
	}

	String AmbientLightPass::DoGetVertexShaderSource( SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_engine.GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_MATRIX( l_writer );
		UBO_GPINFO( l_writer );
		auto vertex = l_writer.DeclAttribute< Vec2 >( ShaderProgram::Position );

		// Shader outputs
		auto gl_Position = l_writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			gl_Position = c3d_mtxProjection * vec4( vertex, 0.0, 1.0 );
		} );

		return l_writer.Finalise();
	}

	String AmbientLightPass::DoGetPixelShaderSource( SceneFlags const & p_sceneFlags
		, LightType p_type )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_engine.GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_SCENE( l_writer );
		UBO_GPINFO( l_writer );
		auto c3d_mapDiffuse = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eDiffuse ) );
		auto c3d_mapEmissive = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eEmissive ) );
		auto gl_FragCoord = l_writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

		// Shader outputs
		auto pxl_v4FragColor = l_writer.DeclFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

		// Utility functions
		GLSL::Fog l_fog{ GetFogType( p_sceneFlags ), l_writer };
		GLSL::Utils l_utils{ l_writer };
		l_utils.DeclareCalcTexCoord();

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto l_texCoord = l_writer.DeclLocale( cuT( "l_texCoord" ), l_utils.CalcTexCoord() );
			auto l_colour = l_writer.DeclLocale( cuT( "l_colour" ), texture( c3d_mapDiffuse, l_texCoord ).xyz() );
			auto l_ambient = l_writer.DeclLocale( cuT( "l_ambient" ), c3d_v4AmbientLight.xyz() );
			auto l_emissive = l_writer.DeclLocale( cuT( "l_emissive" ), texture( c3d_mapEmissive, l_texCoord ).xyz() );

			pxl_v4FragColor = vec4( l_colour * l_ambient, 1.0 );
		} );

		return l_writer.Finalise();
	}

	LightPass::ProgramPtr AmbientLightPass::DoCreateProgram( String const & p_vtx
		, String const & p_pxl )const
	{
		return std::make_unique< Program >( m_engine, p_vtx, p_pxl );
	}
}