#include "ShaderCache.hpp"

#include "Engine.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Event/Frame/CleanupEvent.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderPass.hpp"
#include "Render/RenderSystem.hpp"
#include "Technique/RenderTechnique.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/OneFrameVariable.hpp"
#include "Shader/ShaderObject.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		uint64_t MakeKey(
			TextureChannels const & p_textureFlags,
			ProgramFlags const & p_programFlags,
			uint8_t p_sceneFlags,
			bool p_invertNormals )
		{
			return ( uint64_t( p_textureFlags ) << 48 )
				   | ( uint64_t( p_programFlags ) << 40 )
				   | ( uint64_t( p_sceneFlags ) << 32 )
				   | ( uint64_t( p_invertNormals ? 0x01 : 0x00 ) );
		}
	}

	ShaderProgramCache::ShaderProgramCache( Engine & p_engine )
		: OwnedBy< Engine >( p_engine )
	{
	}

	ShaderProgramCache::~ShaderProgramCache()
	{
	}

	void ShaderProgramCache::Cleanup()
	{
		for ( auto l_program : m_arrayPrograms )
		{
			GetEngine()->PostEvent( MakeCleanupEvent( *l_program ) );
		}
	}

	void ShaderProgramCache::Clear()
	{
		m_mapBillboards.clear();
		m_mapAutogenerated.clear();
		m_arrayPrograms.clear();
	}

	ShaderProgramSPtr ShaderProgramCache::GetNewProgram( bool p_initialise )
	{
		ShaderProgramSPtr l_return = GetEngine()->GetRenderSystem()->CreateShaderProgram();

		if ( l_return )
		{
			DoAddProgram( l_return, p_initialise );
		}

		return l_return;
	}

	ShaderProgramSPtr ShaderProgramCache::GetAutomaticProgram(
		RenderPass const & p_renderPass,
		TextureChannels const & p_textureFlags,
		ProgramFlags const & p_programFlags,
		uint8_t p_sceneFlags,
		bool p_invertNormals )
	{
		ShaderProgramSPtr l_return;

		if ( CheckFlag( p_programFlags, ProgramFlag::eBillboards ) )
		{
			uint64_t l_key = MakeKey( p_textureFlags, p_programFlags, p_sceneFlags, false );
			auto const & l_it = m_mapBillboards.find( l_key );

			if ( l_it != m_mapBillboards.end() )
			{
				l_return = l_it->second.lock();
			}
			else
			{
				l_return = DoCreateBillboardProgram( p_renderPass, p_textureFlags, p_programFlags, p_sceneFlags );

				if ( l_return )
				{
					DoAddBillboardProgram( l_return, p_textureFlags, p_programFlags, p_sceneFlags );
				}
			}
		}
		else
		{
			uint64_t l_key = MakeKey( p_textureFlags, p_programFlags, p_sceneFlags, p_invertNormals );
			ShaderProgramWPtrUInt64MapConstIt l_it = m_mapAutogenerated.find( l_key );

			if ( l_it != m_mapAutogenerated.end() )
			{
				l_return = l_it->second.lock();
			}
			else
			{
				l_return = DoCreateAutomaticProgram( p_renderPass, p_textureFlags, p_programFlags, p_sceneFlags, p_invertNormals );

				if ( l_return )
				{
					DoAddAutomaticProgram( l_return, p_textureFlags, p_programFlags, p_sceneFlags, p_invertNormals );
				}
			}
		}

		return l_return;
	}

	FrameVariableBuffer & ShaderProgramCache::CreateMatrixBuffer(
		ShaderProgram & p_shader,
		ProgramFlags const & p_programFlags,
		FlagCombination< ShaderTypeFlag > const & p_shaderMask )const
	{
		auto & l_buffer = p_shader.CreateFrameVariableBuffer( ShaderProgram::BufferMatrix, p_shaderMask );
		l_buffer.CreateVariable( FrameVariableType::eMat4x4r, RenderPipeline::MtxProjection, 1 );
		l_buffer.CreateVariable( FrameVariableType::eMat4x4r, RenderPipeline::MtxModel, 1 );
		l_buffer.CreateVariable( FrameVariableType::eMat4x4r, RenderPipeline::MtxView, 1 );
		l_buffer.CreateVariable( FrameVariableType::eMat4x4r, RenderPipeline::MtxNormal, 1 );

		for ( uint32_t i = 0; i < C3D_MAX_TEXTURE_MATRICES; ++i )
		{
			l_buffer.CreateVariable( FrameVariableType::eMat4x4r, RenderPipeline::MtxTexture[i], 1 );
		}

		return l_buffer;
	}

	FrameVariableBuffer & ShaderProgramCache::CreateSceneBuffer(
		ShaderProgram & p_shader,
		ProgramFlags const & p_programFlags,
		FlagCombination< ShaderTypeFlag > const & p_shaderMask )const
	{
		auto & l_buffer = p_shader.CreateFrameVariableBuffer( ShaderProgram::BufferScene, p_shaderMask );
		l_buffer.CreateVariable( FrameVariableType::eVec4f, ShaderProgram::AmbientLight, 1 );
		l_buffer.CreateVariable( FrameVariableType::eVec4f, ShaderProgram::BackgroundColour, 1 );
		l_buffer.CreateVariable( FrameVariableType::eVec4i, ShaderProgram::LightsCount, 1 );
		l_buffer.CreateVariable( FrameVariableType::eVec3r, ShaderProgram::CameraPos, 1 );
		l_buffer.CreateVariable( FrameVariableType::eInt, ShaderProgram::FogType, 1 );
		l_buffer.CreateVariable( FrameVariableType::eFloat, ShaderProgram::FogDensity, 1 );

		return l_buffer;
	}

	FrameVariableBuffer & ShaderProgramCache::CreatePassBuffer(
		ShaderProgram & p_shader,
		ProgramFlags const & p_programFlags,
		FlagCombination< ShaderTypeFlag > const & p_shaderMask )const
	{
		auto & l_buffer = p_shader.CreateFrameVariableBuffer( ShaderProgram::BufferPass, p_shaderMask );
		l_buffer.CreateVariable( FrameVariableType::eVec4f, ShaderProgram::MatAmbient, 1 );
		l_buffer.CreateVariable( FrameVariableType::eVec4f, ShaderProgram::MatDiffuse, 1 );
		l_buffer.CreateVariable( FrameVariableType::eVec4f, ShaderProgram::MatEmissive, 1 );
		l_buffer.CreateVariable( FrameVariableType::eVec4f, ShaderProgram::MatSpecular, 1 );
		l_buffer.CreateVariable( FrameVariableType::eFloat, ShaderProgram::MatShininess, 1 );
		l_buffer.CreateVariable( FrameVariableType::eFloat, ShaderProgram::MatOpacity, 1 );
		return l_buffer;
	}

	FrameVariableBuffer & ShaderProgramCache::CreateModelBuffer(
		ShaderProgram & p_shader,
		ProgramFlags const & p_programFlags,
		FlagCombination< ShaderTypeFlag > const & p_shaderMask )const
	{
		auto & l_buffer = p_shader.CreateFrameVariableBuffer( ShaderProgram::BufferModel, p_shaderMask );
		l_buffer.CreateVariable( FrameVariableType::eInt, ShaderProgram::ShadowReceiver, 1 );
		return l_buffer;
	}

	FrameVariableBuffer & ShaderProgramCache::CreateAnimationBuffer(
		ShaderProgram & p_shader,
		ProgramFlags const & p_programFlags,
		FlagCombination< ShaderTypeFlag > const & p_shaderMask )const
	{
		REQUIRE( CheckFlag( p_programFlags, ProgramFlag::eSkinning ) || CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto & l_buffer = p_shader.CreateFrameVariableBuffer( ShaderProgram::BufferAnimation, p_shaderMask );

		if ( CheckFlag( p_programFlags, ProgramFlag::eSkinning ) )
		{
			l_buffer.CreateVariable( FrameVariableType::eMat4x4r, ShaderProgram::Bones, 400 );
		}

		if ( CheckFlag( p_programFlags, ProgramFlag::eMorphing ) )
		{
			l_buffer.CreateVariable( FrameVariableType::eFloat, ShaderProgram::Time );
		}

		return l_buffer;
	}

	void ShaderProgramCache::CreateTextureVariables(
		ShaderProgram & p_shader,
		TextureChannels const & p_textureFlags,
		ProgramFlags const & p_programFlags )const
	{
		if ( CheckFlag( p_programFlags, ProgramFlag::eLighting ) )
		{
			p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::Lights, ShaderType::ePixel );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eAmbient ) )
		{
			p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapAmbient, ShaderType::ePixel );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eColour ) )
		{
			p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapColour, ShaderType::ePixel );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eDiffuse ) )
		{
			p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapDiffuse, ShaderType::ePixel );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eNormal ) )
		{
			p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapNormal, ShaderType::ePixel );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eSpecular ) )
		{
			p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapSpecular, ShaderType::ePixel );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eEmissive ) )
		{
			p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapEmissive, ShaderType::ePixel );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eOpacity ) )
		{
			p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapOpacity, ShaderType::ePixel );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eGloss ) )
		{
			p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapGloss, ShaderType::ePixel );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eHeight ) )
		{
			p_shader.CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapHeight, ShaderType::ePixel );
		}
	}

	void ShaderProgramCache::DoAddProgram( ShaderProgramSPtr p_program, bool p_initialise )
	{
		m_arrayPrograms.push_back( p_program );

		if ( p_initialise )
		{
			if ( GetEngine()->GetRenderSystem()->GetCurrentContext() )
			{
				p_program->Initialise();
			}
			else
			{
				GetEngine()->PostEvent( MakeInitialiseEvent( *p_program ) );
			}
		}
	}

	ShaderProgramSPtr ShaderProgramCache::DoCreateAutomaticProgram(
		RenderPass const & p_renderPass,
		TextureChannels const & p_textureFlags,
		ProgramFlags const & p_programFlags,
		uint8_t p_sceneFlags,
		bool p_invertNormals )const
	{
		ShaderProgramSPtr l_return = GetEngine()->GetRenderSystem()->CreateShaderProgram();

		if ( l_return )
		{
			FlagCombination< ShaderTypeFlag > l_matrixUboShaderMask = ShaderTypeFlag::eVertex | ShaderTypeFlag::ePixel;
			ShaderModel l_model = GetEngine()->GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			l_return->CreateObject( ShaderType::eVertex );
			l_return->CreateObject( ShaderType::ePixel );
			l_return->SetSource( ShaderType::eVertex, l_model, p_renderPass.GetVertexShaderSource( p_textureFlags, p_programFlags, p_sceneFlags, p_invertNormals ) );
			l_return->SetSource( ShaderType::ePixel, l_model, p_renderPass.GetPixelShaderSource( p_textureFlags, p_programFlags, p_sceneFlags ) );
			auto l_geometry = p_renderPass.GetGeometryShaderSource( p_textureFlags, p_programFlags, p_sceneFlags );

			if ( !l_geometry.empty() )
			{
				AddFlag( l_matrixUboShaderMask, ShaderTypeFlag::eGeometry );
				l_return->CreateObject( ShaderType::eGeometry );
				l_return->SetSource( ShaderType::eGeometry, l_model, l_geometry );
			}

			CreateMatrixBuffer( *l_return, p_programFlags, l_matrixUboShaderMask );
			CreateSceneBuffer( *l_return, p_programFlags, ShaderTypeFlag::ePixel );
			CreatePassBuffer( *l_return, p_programFlags, ShaderTypeFlag::ePixel );
			CreateModelBuffer( *l_return, p_programFlags, ShaderTypeFlag::ePixel );
			CreateTextureVariables( *l_return, p_textureFlags, p_programFlags );

			if ( CheckFlag( p_programFlags, ProgramFlag::eSkinning )
				 || CheckFlag( p_programFlags, ProgramFlag::eMorphing ) )
			{
				CreateAnimationBuffer( *l_return, p_programFlags, ShaderTypeFlag::eVertex );
			}
		}

		return l_return;
	}

	void ShaderProgramCache::DoAddAutomaticProgram(
		ShaderProgramSPtr p_program,
		TextureChannels const & p_textureFlags,
		ProgramFlags const & p_programFlags,
		uint8_t p_sceneFlags,
		bool p_invertNormals )
	{
		uint64_t l_key = MakeKey( p_textureFlags, p_programFlags, p_sceneFlags, p_invertNormals );
		auto const & l_it = m_mapAutogenerated.find( l_key );

		if ( l_it == m_mapAutogenerated.end() )
		{
			m_mapAutogenerated.insert( { l_key, p_program } );
			DoAddProgram( p_program, true );
		}
	}

	ShaderProgramSPtr ShaderProgramCache::DoCreateBillboardProgram(
		RenderPass const & p_renderPass,
		TextureChannels const & p_textureFlags,
		ProgramFlags const & p_programFlags,
		uint8_t p_sceneFlags )const
	{
		auto & l_engine = *GetEngine();
		auto & l_renderSystem = *l_engine.GetRenderSystem();
		ShaderProgramSPtr l_return = GetEngine()->GetRenderSystem()->CreateShaderProgram();

		if ( l_return )
		{
			String l_strVtxShader;
			{
				using namespace GLSL;
				auto l_writer = l_renderSystem.CreateGlslWriter();

				// Shader inputs
				auto position = l_writer.GetAttribute< Vec4 >( ShaderProgram::Position );
				auto texture = l_writer.GetAttribute< Vec2 >( ShaderProgram::Texture );
				auto center = l_writer.GetAttribute< Vec3 >( cuT( "center" ) );
				auto gl_InstanceID( l_writer.GetBuiltin< Int >( cuT( "gl_InstanceID" ) ) );
				auto gl_VertexID( l_writer.GetBuiltin< Int >( cuT( "gl_VertexID" ) ) );
				UBO_MATRIX( l_writer );
				UBO_SCENE( l_writer );
				UBO_MODEL( l_writer );
				UBO_BILLBOARD( l_writer );

				// Shader outputs
				auto vtx_worldSpacePosition = l_writer.GetOutput< Vec3 >( cuT( "vtx_worldSpacePosition" ) );
				auto vtx_normal = l_writer.GetOutput< Vec3 >( cuT( "vtx_normal" ) );
				auto vtx_tangent = l_writer.GetOutput< Vec3 >( cuT( "vtx_tangent" ) );
				auto vtx_bitangent = l_writer.GetOutput< Vec3 >( cuT( "vtx_bitangent" ) );
				auto vtx_texture = l_writer.GetOutput< Vec3 >( cuT( "vtx_texture" ) );
				auto vtx_instance = l_writer.GetOutput< Int >( cuT( "vtx_instance" ) );
				auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

				l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
				{
					auto l_center = l_writer.GetLocale( cuT( "l_center" ), l_writer.Paren( c3d_mtxModel * vec4( center, 1.0 ) ).xyz() );
					auto l_toCamera = l_writer.GetLocale( cuT( "l_toCamera" ), c3d_v3CameraPosition - l_center );
					l_toCamera.y() = 0.0_f;
					l_toCamera = normalize( l_toCamera );
					auto l_right = l_writer.GetLocale( cuT( "l_right" ), vec3( c3d_mtxView[0][0], c3d_mtxView[1][0], c3d_mtxView[2][0] ) );
					auto l_up = l_writer.GetLocale( cuT( "l_up" ), vec3( c3d_mtxView[0][1], c3d_mtxView[1][1], c3d_mtxView[2][1] ) );

					if ( !CheckFlag( p_programFlags, ProgramFlag::eSpherical ) )
					{
						l_right = normalize( vec3( l_right.x(), 0.0, l_right.z() ) );
						l_up = vec3( 0.0_f, 1.0f, 0.0f );
					}

					vtx_normal = l_toCamera;
					vtx_tangent = l_up;
					vtx_bitangent = l_right;

					auto l_width = l_writer.GetLocale( cuT( "l_width" ), c3d_v2iDimensions.x() );
					auto l_height = l_writer.GetLocale( cuT( "l_height" ), c3d_v2iDimensions.y() );

					if ( CheckFlag( p_programFlags, ProgramFlag::eFixedSize ) )
					{
						l_width = c3d_v2iDimensions.x() / c3d_v2iWindowSize.x();
						l_height = c3d_v2iDimensions.y() / c3d_v2iWindowSize.y();
					}

					vtx_worldSpacePosition = center
						+ l_right * position.x() * l_width
						+ l_up * position.y() * l_height;

					vtx_texture = vec3( texture, 0.0 );
					vtx_instance = gl_InstanceID;
					auto l_wvPosition = l_writer.GetLocale( cuT( "l_wvPosition" ), l_writer.Paren( c3d_mtxView * vec4( vtx_worldSpacePosition, 1.0 ) ).xyz() );
					gl_Position = c3d_mtxProjection * vec4( l_wvPosition, 1.0 );
				} );

				l_strVtxShader = l_writer.Finalise();
			}

			String l_strPxlShader = p_renderPass.GetPixelShaderSource( p_textureFlags, p_programFlags, p_sceneFlags );

			auto l_model = l_renderSystem.GetGpuInformations().GetMaxShaderModel();
			l_return->CreateObject( ShaderType::eVertex );
			l_return->CreateObject( ShaderType::ePixel );
			l_return->SetSource( ShaderType::eVertex, l_model, l_strVtxShader );
			l_return->SetSource( ShaderType::ePixel, l_model, l_strPxlShader );

			CreateMatrixBuffer( *l_return, p_programFlags, ShaderTypeFlag::ePixel );
			CreateSceneBuffer( *l_return, p_programFlags, ShaderTypeFlag::eVertex | ShaderTypeFlag::ePixel );
			CreatePassBuffer( *l_return, p_programFlags, ShaderTypeFlag::ePixel );
			CreateModelBuffer( *l_return, p_programFlags, ShaderTypeFlag::ePixel );
			CreateTextureVariables( *l_return, p_textureFlags, p_programFlags );
			auto & l_billboardUbo = l_return->CreateFrameVariableBuffer( ShaderProgram::BufferBillboards, ShaderTypeFlag::eVertex );
			l_billboardUbo.CreateVariable< Point2iFrameVariable >( ShaderProgram::Dimensions );
			l_billboardUbo.CreateVariable< Point2iFrameVariable >( ShaderProgram::WindowSize );
		}

		return l_return;
	}

	void ShaderProgramCache::DoAddBillboardProgram(
		ShaderProgramSPtr p_program,
		TextureChannels const & p_textureFlags,
		ProgramFlags const & p_programFlags,
		uint8_t p_sceneFlags )
	{
		uint64_t l_key = MakeKey( p_textureFlags, p_programFlags, p_sceneFlags, false );
		auto const & l_it = m_mapBillboards.find( l_key );

		if ( l_it == m_mapBillboards.end() )
		{
			m_mapBillboards.insert( { l_key, p_program } );
			DoAddProgram( p_program, true );
		}
	}
}
