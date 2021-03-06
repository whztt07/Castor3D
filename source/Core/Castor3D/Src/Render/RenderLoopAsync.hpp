/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RENDER_LOOP_ASYNC_H___
#define ___C3D_RENDER_LOOP_ASYNC_H___

#include "RenderLoop.hpp"

#include <atomic>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Main System
	\remark		Holds the render windows, the plug-ins, the render drivers...
	\~french
	\brief		Moteur principal
	\remark		Contient les fenêtres de rendu, les plug-ins, drivers de rendu...
	*/
	class RenderLoopAsync
		: public RenderLoop
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	p_wantedFPS		The wanted FPS count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	p_wantedFPS		Le nombre voulu du FPS.
		 */
		C3D_API RenderLoopAsync( Engine & engine, uint32_t p_wantedFPS = 100 );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderLoopAsync();
		/**
		 *\~english
		 *\brief		Retrieves the context creation status.
		 *\remarks		Thread-safe.
		 *\return		\p true if created.
		 *\~french
		 *\brief		Récupère le statut de création du contexte de rendu.
		 *\remarks		Thread-safe.
		 *\return		\p true si créé.
		 */
		C3D_API bool isCreated()const;
		/**
		 *\~english
		 *\brief		Retrieves the render start status.
		 *\remarks		Thread-safe.
		 *\return		\p true if started.
		 *\~french
		 *\brief		Récupère le statut de début de rendu.
		 *\remarks		Thread-safe.
		 *\return		\p true si démarré.
		 */
		C3D_API bool isRendering()const;
		/**
		 *\~english
		 *\brief		Retrieves the render paused status.
		 *\remarks		Thread-safe.
		 *\return		\p true if paused.
		 *\~french
		 *\brief		Récupère le statut de rendu en pause.
		 *\remarks		Thread-safe.
		 *\return		\p true si en pause.
		 */
		C3D_API bool isPaused()const;
		/**
		 *\~english
		 *\brief		Thread-safe
		 *\return		\p true if the render loop is interrupted.
		 *\~french
		 *\brief		Thread-safe.
		 *\return		\p true si la boucle de rendu est interrompue.
		 */
		C3D_API bool isInterrupted()const;
		/**
		 *\~english
		 *\brief		Retrieves the render loop end status.
		 *\remarks		Thread-safe.
		 *\return		\p true if created.
		 *\~french
		 *\brief		Récupère le statut de fin de la boucle de rendu.
		 *\remarks		Thread-safe.
		 *\return		\p true si créé.
		 */
		C3D_API bool isEnded()const;
		/**
		 *\~english
		 *\brief		Updates the V-Sync status.
		 *\param[in]	p_enable	The status.
		 *\~french
		 *\brief		Met à jour le statut de synchronisation verticale.
		 *\param[in]	p_enable	Le statut.
		 */
		C3D_API void updateVSync( bool p_enable )override;
		/**
		 *\copydoc		castor3d::RenderLoop::beginRendering
		 */
		C3D_API void beginRendering()override;
		/**
		 *\copydoc		castor3d::RenderLoop::renderSyncFrame
		 */
		C3D_API void renderSyncFrame()override;
		/**
		 *\copydoc		castor3d::RenderLoop::pause
		 */
		C3D_API void pause()override;
		/**
		 *\copydoc		castor3d::RenderLoop::resume
		 */
		C3D_API void resume()override;
		/**
		 *\copydoc		castor3d::RenderLoop::endRendering
		 */
		C3D_API void endRendering()override;

	private:
		/**
		 *\copydoc		castor3d::RenderLoop::doCreateMainContext
		 */
		C3D_API ContextSPtr doCreateMainContext( RenderWindow & p_window )override;
		/**
		 *\~english
		 *\brief		The threaded render loop.
		 *\remarks		The main context is created here, since OpenGL needs each thread to have its context.
		 *\~french
		 *\brief		La boucle de rendu threadée.
		 *\remarks		Le contexte principal est créé ici, car OpenGL demande à chaque thread d'avoir son contexte.
		 */
		C3D_API void doMainLoop();
		C3D_API void doSetWindow( RenderWindow * p_window );
		C3D_API RenderWindow * doGetWindow()const;

	private:
		//!\~english The main loop, in case of threaded rendering	\~french La boucle principale, au cas où on fait du rendu threadé
		std::unique_ptr< std::thread > m_mainLoopThread;
		//!\~english Tells if render has ended, by any reason.	\~french Dit si le rendu est terminé, quelle qu'en soit la raison.
		std::atomic_bool m_ended;
		//!\~english Tells if render is running.	\~french Dit si le rendu est en cours.
		std::atomic_bool m_rendering;
		//!\~english Tells the frame render is ended.	\~french Dit si le rendu de la frame courante est terminé.
		std::atomic_bool m_frameEnded;
		//!\~english Tells if render is paused.	\~french Dit si le rendu est en pause.
		std::atomic_bool m_paused;
		//!\~english Tells if render context is to be created.	\~french Dit si le contexte de rendu est à créer.
		std::atomic_bool m_createContext;
		//!\~english Tells if render context is created.	\~french Dit si le contexte de rendu est créé.
		std::atomic_bool m_created;
		//!\~english Tells if the loop is interrupted.	\~french Dit si la boucle est interrompue.
		std::atomic_bool m_interrupted;
		//!\~english The mutex, to make the main loop thread-safe	\~french Le mutex utilisé pour que la boucle principale soit thread-safe
		mutable std::mutex m_mutexWindow;
		//!\~english The render window used to initalise the main rendering context	\~french La render window utilisée pour initialiser le contexte de rendu principal
		RenderWindowRPtr m_window;
		//!\~english The saved frame time, if V-Sync is disabled.	\~french Le temps par frame sauvegardé, si la synchronisation verticale est désactivée.
		castor::Milliseconds m_savedTime{ 0 };
	};
}

#endif
