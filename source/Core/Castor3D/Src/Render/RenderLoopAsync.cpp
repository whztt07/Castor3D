﻿#include "RenderLoopAsync.hpp"

#include "Engine.hpp"
#include "RenderSystem.hpp"

#include <Miscellaneous/PreciseTimer.hpp>
#include <Design/ScopeGuard.hpp>

using namespace Castor;

//*************************************************************************************************

namespace Castor3D
{
	static const char * CALL_RENDER_SYNC_FRAME = "Can't call RenderSyncFrame in threaded render loop";
	static const char * CALL_PAUSE_RENDERING = "Can't call Pause on a paused render loop";
	static const char * CALL_RESUME_RENDERING = "Can't call Resume on a non paused render loop";
	static const char * RLA_UNKNOWN_EXCEPTION = "Unknown exception";

	RenderLoopAsync::RenderLoopAsync( Engine & p_engine, uint32_t p_wantedFPS )
		: RenderLoop( p_engine, p_wantedFPS )
		, m_mainLoopThread( nullptr )
		, m_paused( false )
		, m_ended( false )
		, m_rendering( false )
		, m_createContext( false )
		, m_created( false )
		, m_interrupted( false )
	{
		m_mainLoopThread.reset( new std::thread( std::bind( &RenderLoopAsync::DoMainLoop, this ) ) );
	}

	RenderLoopAsync::~RenderLoopAsync()
	{
		// We wait for the main loop to end (it makes a final render to clean the render system)
		m_interrupted = true;
		m_mainLoopThread->join();
		m_mainLoopThread.reset();
	}

	bool RenderLoopAsync::IsCreated()const
	{
		return m_created;
	}

	bool RenderLoopAsync::IsRendering()const
	{
		return m_rendering;
	}

	bool RenderLoopAsync::IsInterrupted()const
	{
		return m_interrupted;
	}

	bool RenderLoopAsync::IsEnded()const
	{
		return m_ended;
	}

	bool RenderLoopAsync::IsPaused()const
	{
		return m_paused;
	}

	void RenderLoopAsync::UpdateVSync( bool p_enable )
	{
		if ( p_enable && m_savedTime )
		{
			m_frameTime = m_savedTime;
			m_savedTime = 0;
		}
		else if ( !m_savedTime )
		{
			m_savedTime = m_frameTime;
			m_frameTime = 1;
		}
	}

	void RenderLoopAsync::DoStartRendering()
	{
		m_rendering = true;
	}

	void RenderLoopAsync::DoRenderSyncFrame()
	{
		if ( !m_paused )
		{
			CASTOR_EXCEPTION( CALL_RENDER_SYNC_FRAME );
		}

		if ( m_rendering )
		{
			// trick pour éviter les problèmes de contexte dans des threads différents:
			// On enlève la pause le temps de dessiner une frame.
			m_paused = false;

			while ( !m_frameEnded )
			{
				System::Sleep( 1 );
			}

			m_paused = true;
		}
	}

	void RenderLoopAsync::DoPause()
	{
		if ( m_paused )
		{
			CASTOR_EXCEPTION( CALL_PAUSE_RENDERING );
		}

		m_paused = true;

		while ( !m_frameEnded )
		{
			System::Sleep( 5 );
		}
	}

	void RenderLoopAsync::DoResume()
	{
		if ( !m_paused )
		{
			CASTOR_EXCEPTION( CALL_RESUME_RENDERING );
		}

		m_paused = false;
	}

	void RenderLoopAsync::DoEndRendering()
	{
		m_rendering = false;

		while ( !IsEnded() )
		{
			System::Sleep( 5 );
		}
	}

	ContextSPtr RenderLoopAsync::DoCreateMainContext( RenderWindow & p_window )
	{
		ContextSPtr l_return;

		if ( !m_createContext )
		{
			DoSetWindow( &p_window );
			m_createContext = true;

			while ( !IsInterrupted() && !IsCreated() )
			{
				System::Sleep( 5 );
			}

			m_createContext = false;
			DoSetWindow( nullptr );
			l_return = m_renderSystem.GetMainContext();
		}

		return l_return;
	}

	void RenderLoopAsync::DoMainLoop()
	{
		PreciseTimer l_timer;
		m_frameEnded = true;
		auto l_scopeGuard{ make_scope_guard( [this]()
		{
			Cleanup();
			m_renderSystem.Cleanup();
		} ) };

		try
		{
			// Tant qu'on n'est pas interrompu, on continue
			while ( !IsInterrupted() )
			{
				// Tant qu'on n'a pas de contexte principal et qu'on ne nous a pas demandé de le créer, on attend.
				while ( !IsInterrupted() && !m_createContext && !IsCreated() )
				{
					System::Sleep( 10 );
				}

				if ( !IsInterrupted() && m_createContext && !IsCreated() )
				{
					// On nous a demandé de créer le contexte principal, on le crée
					ContextSPtr l_context = DoCreateContext( *DoGetWindow() );

					if ( l_context )
					{
						m_renderSystem.SetMainContext( l_context );
						m_created = true;
					}
					else
					{
						m_interrupted = true;
					}
				}

				// Tant qu'on n'a pas demandé le début du rendu, on attend.
				while ( !IsInterrupted() && !IsRendering() )
				{
					System::Sleep( 10 );
				}

				// Le rendu est en cours
				while ( !IsInterrupted() && IsRendering() && !IsPaused() )
				{
					m_frameEnded = false;
					uint32_t l_frameTime = GetFrameTime();
					l_timer.TimeS();
					DoRenderFrame();
					uint32_t l_timeDiff = uint32_t( l_timer.TimeMs() );
					m_frameEnded = true;

					if ( l_timeDiff < l_frameTime )
					{
						System::Sleep( l_frameTime - l_timeDiff );
					}
					else
					{
						// In order to let the CPU work, we sleep at least 1ms
						System::Sleep( 1 );
					}
				}

				m_ended = true;
			}
		}
		catch ( Castor::Exception & p_exc )
		{
			Logger::LogError( cuT( "RenderLoop - " ) + p_exc.GetFullDescription() );
			m_frameEnded = true;
			m_ended = true;
		}
		catch ( std::exception & p_exc )
		{
			Logger::LogError( std::string( "RenderLoop - " ) + p_exc.what() );
			m_frameEnded = true;
			m_ended = true;
		}
	}

	void RenderLoopAsync::DoSetWindow( RenderWindow * p_window )
	{
		auto l_lock = make_unique_lock( m_mutexWindow );
		m_window = p_window;
	}

	RenderWindow * RenderLoopAsync::DoGetWindow()const
	{
		auto l_lock = make_unique_lock( m_mutexWindow );
		return m_window;
	}
}
