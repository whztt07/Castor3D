/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_TARGET_CACHE_H___
#define ___C3D_TARGET_CACHE_H___

#include "Cache/Cache.hpp"

#include "HDR/ToneMappingFactory.hpp"
#include "PostEffect/PostEffectFactory.hpp"
#include "Render/RenderTarget.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to get an object type name.
	\~french
	\brief		Structure permettant de récupérer le nom du type d'un objet.
	*/
	template<>
	struct CachedObjectNamer< RenderTarget >
	{
		C3D_API static const Castor::String Name;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		RenderTarget cache.
	\~french
	\brief		Cache de RenderTarget.
	*/
	class RenderTargetCache
		: public Castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API RenderTargetCache( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderTargetCache();
		/**
		 *\~english
		 *\brief		Creates a render target of given type
		 *\param[in]	p_type	The render target type
		 *\return		The render target
		 *\~french
		 *\brief		Crée une cible de rendu du type voulu
		 *\param[in]	p_type	Le type de cible de rendu
		 *\return		La cible de rendu
		 */
		C3D_API RenderTargetSPtr Add( eTARGET_TYPE p_type );
		/**
		 *\~english
		 *\brief		Removes a render target from the render loop
		 *\param[in]	p_target	The render target
		 *\~french
		 *\brief		Enlève une cible de rendu de la boucle de rendu
		 *\param[in]	p_target	La cible de rendu
		 */
		C3D_API void Remove( RenderTargetSPtr p_target );
		/**
		 *\~english
		 *\brief			Renders into the render targets.
		 *\param[in]		p_time		The current frame time.
		 *\param[in,out]	p_vtxCount	Receives the total vertex count.
		 *\param[in]		p_fceCount	Receives the total faces count.
		 *\param[in]		p_objCount	Receives the total objects count.
		 *\~french
		 *\brief			Dessine dans les cibles de rendu.
		 *\param[in]		p_time		Le temps d dessin actuel.
		 *\param[in,out]	p_vtxCount	Reçoit le nombre de sommets dessinés.
		 *\param[in,out]	p_fceCount	Reçoit le nombre de faces dessinées.
		 *\param[in,out]	p_objCount	Reçoit le nombre d'objets dessinés.
		 */
		C3D_API void Render( uint32_t & p_time, uint32_t & p_vtxCount, uint32_t & p_fceCount, uint32_t & p_objCount );
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\~french
		 *\brief		Vide la collection.
		 */
		C3D_API void Clear ();
		/**
		 *\~english
		 *\return		The ToneMapping factory.
		 *\~french
		 *\return		La fabrique de ToneMapping.
		 */
		inline ToneMappingFactory const & GetToneMappingFactory()const
		{
			return m_toneMappingFactory;
		}
		/**
		 *\~english
		 *\return		The ToneMapping factory.
		 *\~french
		 *\return		La fabrique de ToneMapping.
		 */
		inline ToneMappingFactory & GetToneMappingFactory()
		{
			return m_toneMappingFactory;
		}
		/**
		 *\~english
		 *\return		The PostEffect factory.
		 *\~french
		 *\return		La fabrique de PostEffect.
		 */
		inline PostEffectFactory const & GetPostEffectFactory()const
		{
			return m_postEffectFactory;
		}
		/**
		 *\~english
		 *\return		The PostEffect factory.
		 *\~french
		 *\return		La fabrique de PostEffect.
		 */
		inline PostEffectFactory & GetPostEffectFactory()
		{
			return m_postEffectFactory;
		}
		/**
		 *\~english
		 *\brief		Locks the collection mutex
		 *\~french
		 *\brief		Locke le mutex de la collection
		 */
		inline void lock()const
		{
			m_mutex.lock();
		}
		/**
		 *\~english
		 *\brief		Unlocks the collection mutex
		 *\~french
		 *\brief		Délocke le mutex de la collection
		 */
		inline void unlock()const
		{
			m_mutex.unlock();
		}
		/**
		*\~english
		*\param[in]	p_renderSystem	The RenderSystem.
		*\~french
		*\param[in]	p_renderSystem	Le RenderSystem.
		*/
		inline void SetRenderSystem( RenderSystem * p_renderSystem )
		{
			m_renderSystem = p_renderSystem;
		}
		/**
		*\~english
		*\return		The RenderSystem.
		*\~french
		*\return		Le RenderSystem.
		*/
		inline RenderSystem * SetRenderSystem()const
		{
			return m_renderSystem;
		}

	private:
		DECLARE_VECTOR( RenderTargetSPtr, RenderTarget );
		DECLARE_ARRAY( RenderTargetArray, eTARGET_TYPE_COUNT, TargetType );
		//!\~english	The RenderSystem.
		//!\~french		Le RenderSystem.
		RenderSystem * m_renderSystem{ nullptr };
		//!\~english	The render targets sorted by target type.
		//!\~french		Les cibles de rendu, triées par type de cible de rendu.
		TargetTypeArray m_renderTargets;
		//!\~english	The mutex protecting the render targets array.
		//!\~french		Le mutex protégeant le tableau de cibles de rendu.
		mutable std::mutex m_mutex;
		//!\~english	The tone mapping factory.
		//!\~french		La fabrique de mappage de tons.
		ToneMappingFactory m_toneMappingFactory;
		//!\~english	The post effect factory.
		//!\~french		La fabrique d'effets post rendu.
		PostEffectFactory m_postEffectFactory;
	};
}

#endif
