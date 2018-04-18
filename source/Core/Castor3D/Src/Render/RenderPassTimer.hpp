/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderPassTimer_H___
#define ___C3D_RenderPassTimer_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/Named.hpp>
#include <Miscellaneous/PreciseTimer.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		24/07/2017
	\~english
	\brief		Class that holds data needed to compute a render pass times.
	\~french
	\brief		Classe englobant les données nécessaires au calcul des temps d'une passe de rendu.
	*/
	class RenderPassTimer
		: public castor::Named
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	category	The render pass category.
		 *\param[in]	name		The timer name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	category	La catégorie de la passe de rendu.
		 *\param[in]	name		Le nom du timer.
		 */
		C3D_API RenderPassTimer( Engine & engine
			, castor::String const & category
			, castor::String const & name );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderPassTimer();
		/**
		 *\~english
		 *\brief		Starts the CPU timer, resets GPU time.
		 *\~french
		 *\brief		Démarre le timer CPU, réinitialise le temps GPU.
		 */
		C3D_API void start();
		/**
		 *\~english
		 *\brief		Retrieves GPU time and adds it to current time.
		 *\~french
		 *\brief
		 *\brief		Récupère le temps GPU et l'ajoute au temps actuel.
		 */
		C3D_API void step();
		/**
		 *\~english
		 *\brief		Stops the CPU timer.
		 *\~french
		 *\brief
		 *\brief		Arrête le timer CPU.
		 */
		C3D_API void stop();
		/**
		 *\~english
		 *\brief		Reset the timer's times.
		 *\~french
		 *\brief
		 *\brief		Réinitialise les temps du timer.
		 */
		C3D_API void reset();
		/**
		 *\~english
		 *\return		The CPU time.
		 *\~french
		 *\return		Le temps CPU.
		 */
		inline castor::Nanoseconds getCpuTime()const
		{
			return m_cpuTime;
		}
		/**
		 *\~english
		 *\return		The CPU time.
		 *\~french
		 *\return		Le temps CPU.
		 */
		inline castor::Nanoseconds getGpuTime()const
		{
			return m_gpuTime;
		}
		/**
		 *\~english
		 *\return		The render pass category.
		 *\~french
		 *\return		La categorie de la passe de rendu.
		 */
		inline castor::String const & getCategory()const
		{
			return m_category;
		}
		/**
		 *\~english
		 *\return		The query pool.
		 *\~french
		 *\return		Le pool de requêtes.
		 */
		inline renderer::QueryPool const & getQuery()const
		{
			REQUIRE( m_timerQuery );
			return *m_timerQuery;
		}

	private:
		//!\~english	The engine.
		//!\~french		Le moteur.
		Engine & m_engine;
		//!\~english	The render pass category.
		//!\~french		La categorie de la passe de rendu.
		castor::String m_category;
		//!\~english	The CPU timer.
		//!\~french		Le timer CPU.
		castor::PreciseTimer m_cpuTimer;
		//!\~english	The CPU time.
		//!\~french		Le temps CPU.
		castor::Nanoseconds m_cpuTime;
		//!\~english	The GPU time.
		//!\~french		Le temps GPU.
		castor::Nanoseconds m_gpuTime;
		//!\~english	The GPU time elapsed queries.
		//!\~french		Les requêtes GPU de temps écoulé.
		renderer::QueryPoolPtr m_timerQuery;
	};
}

#endif
