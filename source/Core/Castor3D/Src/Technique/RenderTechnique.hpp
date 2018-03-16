/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTechnique_H___
#define ___C3D_RenderTechnique_H___

#include "HDR/ToneMapping.hpp"
#include "Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Render/RenderInfo.hpp"
#include "ShadowMap/ShadowMap.hpp"
#include "Technique/DepthPass.hpp"
#include "Technique/Opaque/DeferredRendering.hpp"
#include "Technique/Transparent/WeightedBlendRendering.hpp"
#include "Texture/TextureUnit.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>
#include <Graphics/Rectangle.hpp>

namespace castor3d
{
	class RenderTechniquePass;
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Render technique base class
	\remarks	A render technique is the description of a way to render a render target
	\~french
	\brief		Classe de base d'une technique de rendu
	\remarks	Une technique de rendu est la description d'une manière de rendre une cible de rendu
	*/
	class RenderTechnique
		: public castor::OwnedBy< Engine >
		, public castor::Named
	{
		friend class RenderTechniquePass;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name			The technique name.
		 *\param[in]	renderTarget	The render target for this technique.
		 *\param[in]	renderSystem	The render system.
		 *\param[in]	parameters		The technique parameters.
		 *\param[in]	config			The SSAO configuration.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name			Le nom de la technique.
		 *\param[in]	renderTarget	La render target pour cette technique.
		 *\param[in]	renderSystem	Le render system.
		 *\param[in]	parameters		Les paramètres de la technique.
		 *\param[in]	config			La configuration du SSAO.
		 */
		C3D_API RenderTechnique( castor::String const & name
			, RenderTarget & renderTarget
			, RenderSystem & renderSystem
			, Parameters const & parameters
			, SsaoConfig const & config );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~RenderTechnique();
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\param[in]	index		The base texture index.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\param[in]	index		L'index de texture de base.
		 *\return		\p true if ok.
		 */
		C3D_API bool initialise( uint32_t & index );
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Update function.
		 *\remarks		Gather the render queues, for further update.
		 *\param[out]	queues	Receives the render queues needed for the rendering of the frame.
		 *\~french
		 *\brief		Fonction de mise à jour.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[out]	queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 */
		C3D_API void update( RenderQueueArray & queues );
		/**
		 *\~english
		 *\brief		Render function
		 *\param[in]	jitter		The jittering value.
		 *\param[out]	velocity	Receives the velocity render.
		 *\param[out]	info		Receives the render informations.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[out]	jitter		La valeur de jittering.
		 *\param[out]	velocity	Reçoit le rendu des vélocités.
		 *\param[out]	info		Reçoit les informations de rendu.
		 */
		C3D_API void render( castor::Point2r const & jitter
			, renderer::Semaphore const & waitSemaphore
			, RenderInfo & info );
		/**
		 *\~english
		 *\brief		Writes the technique into a text file.
		 *\param[in]	file	The file.
		 *\~french
		 *\brief		Ecrit la technique dans un fichier texte.
		 *\param[in]	file	Le fichier.
		 */
		C3D_API bool writeInto( castor::TextFile & file );
		/**
		 *\~english
		 *\brief		Displays debug dumps.
		 *\~french
		 *\brief		Affiche les dumps de debug.
		 */
		C3D_API void debugDisplay( castor::Size const & size )const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline castor::Size const & getSize()const
		{
			return m_size;
		}

		inline TextureLayout const & getResult()const
		{
			REQUIRE( m_colourTexture );
			return *m_colourTexture;
		}

		inline TextureLayout const & getDepth()const
		{
			REQUIRE( m_depthBuffer );
			return *m_depthBuffer;
		}

		inline RenderTechniquePass const & getOpaquePass()const
		{
			REQUIRE( m_opaquePass );
			return *m_opaquePass;
		}

		inline RenderTechniquePass const & getTransparentPass()const
		{
			REQUIRE( m_transparentPass );
			return *m_transparentPass;
		}

		inline renderer::Semaphore const & getSemaphore()const
		{
			REQUIRE( m_signalFinished );
			return *m_signalFinished;
		}

		inline bool isMultisampling()const
		{
			return false;
		}
		/**@}*/

	public:
		using ShadowMapArray = std::vector< ShadowMapUPtr >;

	private:
		void doInitialiseShadowMaps();
		void doCleanupShadowMaps();
		void doUpdateShadowMaps( RenderQueueArray & queues );
		void doUpdateParticles( RenderInfo & info );
		renderer::Semaphore const * doRenderShadowMaps( renderer::Semaphore const & semaphore );
		renderer::Semaphore const * doRenderEnvironmentMaps( renderer::Semaphore const & semaphore );
		renderer::Semaphore const * doRenderOpaque( castor::Point2r const & jitter
			, RenderInfo & info
			, renderer::Semaphore const & semaphore );
		renderer::Semaphore const * doRenderTransparent( castor::Point2r const & jitter
			, RenderInfo & info
			, renderer::Semaphore const & semaphore );
		renderer::Semaphore const * doApplyPostEffects( renderer::Semaphore const & semaphore );

	private:
		bool m_initialised;
		RenderTarget & m_renderTarget;
		RenderSystem & m_renderSystem;
		castor::Size m_size;
		TextureLayoutSPtr m_colourTexture;
		TextureLayoutSPtr m_depthBuffer;
		std::unique_ptr< RenderTechniquePass > m_opaquePass;
		std::unique_ptr< RenderTechniquePass > m_transparentPass;
		SsaoConfig m_ssaoConfig;
		std::unique_ptr< DeferredRendering > m_deferredRendering;
		std::unique_ptr< WeightedBlendRendering > m_weightedBlendRendering;
		RenderPassTimerSPtr m_particleTimer;
		RenderPassTimerSPtr m_postFxTimer;
		ShadowMapArray m_directionalShadowMaps;
		ShadowMapArray m_pointShadowMaps;
		ShadowMapArray m_spotShadowMaps;
		ShadowMapLightTypeArray m_activeShadowMaps;
		renderer::SemaphorePtr m_signalFinished;
	};
}

#endif
