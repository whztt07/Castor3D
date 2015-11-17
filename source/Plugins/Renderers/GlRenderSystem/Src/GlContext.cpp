#include "GlContext.hpp"

#if defined( _WIN32 )
#	include "GlMswContext.hpp"
#elif defined( __linux__ )
#	include "GlX11Context.hpp"
#endif

#include "GlRenderSystem.hpp"
#include "GlShaderSource.hpp"

#include <RenderWindow.hpp>
#include <ShaderProgram.hpp>
#include <GeometryBuffers.hpp>
#include <IndexBuffer.hpp>
#include <VertexBuffer.hpp>
#include <MatrixBuffer.hpp>

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	GlContext::GlContext( GlRenderSystem & p_renderSystem, OpenGl & p_gl )
		: Context( p_renderSystem, false )
		, m_pGlRenderSystem( &p_renderSystem )
		, m_gl( p_gl )
	{
		m_timerQueryId[0] = eGL_INVALID_INDEX;
		m_timerQueryId[1] = eGL_INVALID_INDEX;
		m_pImplementation = new GlContextImpl( m_gl, this );
	}

	GlContext::~GlContext()
	{
		delete m_pImplementation;
	}

	GlContextImpl * GlContext::GetImpl()
	{
		return m_pImplementation;
	}

	void GlContext::UpdateFullScreen( bool p_bVal )
	{
		if ( Context::m_window->GetVSync() )
		{
			GetImpl()->UpdateVSync( p_bVal );
		}
	}

	bool GlContext::DoInitialise()
	{
		using namespace GLSL;
		m_bInitialised = m_pImplementation->Initialise( m_window );

		if ( m_bInitialised )
		{
			String l_strVtxShader;
			{
				// Vertex shader
				GlslWriter l_writer( m_gl, eSHADER_TYPE_VERTEX );
				l_writer << Version() << Endl();

				UBO_MATRIX( l_writer );

				// Shader inputs
				ATTRIBUTE( l_writer, Vec2, vertex );
				ATTRIBUTE( l_writer, Vec2, texture );

				// Shader outputs
				OUT( l_writer, Vec2, vtx_texture );

				l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
				{
					vtx_texture = texture;
					BUILTIN( l_writer, Vec4, gl_Position ) = c3d_mtxProjection * vec4( vertex.x(), vertex.y(), 0.0, 1.0 );
				} );
				l_strVtxShader = l_writer.Finalise();
			}

			String l_strPxlShader;
			{
				GlslWriter l_writer( m_gl, eSHADER_TYPE_PIXEL );
				l_writer << Version() << Endl();

				// Shader inputs
				UNIFORM( l_writer, Sampler2D, c3d_mapDiffuse );
				IN( l_writer, Vec2, vtx_texture );

				// Shader outputs
				LAYOUT( l_writer, Vec4, plx_v4FragColor );

				l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
				{
					plx_v4FragColor = vec4( texture2D( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz(), 1.0 );
				} );
				l_strPxlShader = l_writer.Finalise();
			}

			ShaderProgramBaseSPtr l_program = m_renderTextureProgram.lock();
			l_program->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_2, l_strVtxShader );
			l_program->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_2, l_strPxlShader );
			l_program->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_3, l_strVtxShader );
			l_program->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_3, l_strPxlShader );
			l_program->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_4, l_strVtxShader );
			l_program->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_4, l_strPxlShader );

			m_pImplementation->SetCurrent();
			m_gl.GenQueries( 2, m_timerQueryId );
			m_pImplementation->EndCurrent();
		}

		return m_bInitialised;
	}

	void GlContext::DoCleanup()
	{
		m_gl.DeleteQueries( 2, m_timerQueryId );
		m_pImplementation->Cleanup();
	}

	void GlContext::DoSetCurrent()
	{
		GetImpl()->SetCurrent();
		m_gl.BeginQuery( eGL_QUERY_TIME_ELAPSED, m_timerQueryId[m_queryIndex] );
	}

	void GlContext::DoEndCurrent()
	{
		m_gl.EndQuery( eGL_QUERY_TIME_ELAPSED );
		uint64_t l_time = 0;
		m_queryIndex = 1 - m_queryIndex;
		m_gl.GetQueryObjectInfos( m_timerQueryId[m_queryIndex], eGL_QUERY_INFO_RESULT, &l_time );
		GetOwner()->IncGpuTime( std::chrono::nanoseconds( l_time ) );
		GetImpl()->EndCurrent();
	}

	void GlContext::DoSwapBuffers()
	{
		GetImpl()->SwapBuffers();
	}

	void GlContext::DoBind( Castor3D::eBUFFER p_eBuffer, Castor3D::eFRAMEBUFFER_TARGET p_eTarget )
	{
		if ( m_gl.HasFbo() )
		{
			m_gl.BindFramebuffer( m_gl.Get( p_eTarget ), 0 );
		}

		if ( p_eTarget == eFRAMEBUFFER_TARGET_DRAW )
		{
			m_gl.DrawBuffer( m_gl.Get( p_eBuffer ) );
		}
		else if ( p_eTarget == eFRAMEBUFFER_TARGET_READ )
		{
			m_gl.ReadBuffer( m_gl.Get( p_eBuffer ) );
		}
	}

	void GlContext::DoSetAlphaFunc( eALPHA_FUNC p_eFunc, uint8_t p_byValue )
	{
		m_gl.AlphaFunc( m_gl.Get( p_eFunc ), p_byValue / 255.0f );
	}

	void GlContext::DoCullFace( eFACE p_eCullFace )
	{
		if ( p_eCullFace == eFACE_NONE )
		{
			m_gl.Disable( eGL_TWEAK_CULL_FACE );
		}
		else
		{
			m_gl.Enable( eGL_TWEAK_CULL_FACE );
			m_gl.CullFace( m_gl.Get( p_eCullFace ) );
		}
	}
}
