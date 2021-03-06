﻿/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CACHE_VIEW_H___
#define ___C3D_CACHE_VIEW_H___

#include "Cache.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		14/03/2016
	\version	0.8.0
	\~english
	\brief		View on a resource cache.
	\remarks	Allows deletion of elements created through the view, and only those.
	\~french
	\brief		Vue sur un cache de ressources.
	\remarks	Permet de supprimer tous les éléments créés via la vue et uniquement ceux là.
	*/
	template< typename ResourceType, typename CacheType, EventType EventType >
	class CacheView
		: public castor::Named
	{
	private:
		using Initialiser = ElementInitialiser< ResourceType >;
		using Cleaner = ElementCleaner< ResourceType >;
		using ResourcePtr = std::shared_ptr< ResourceType >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_name			The view name.
		 *\param[in]	p_initialise	The element initialiser.
		 *\param[in]	p_clean			The element cleaner.
		 *\param[in]	cache			The viewed cache.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_name			The view name.
		 *\param[in]	p_initialise	L'initialiseur d'objet.
		 *\param[in]	p_clean			Le nettoyeur d'objet.
		 *\param[in]	cache			The viewed cache.
		 */
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name	Le nom de la vue.
		 *\param[in]	cache	Le cache vu.
		 */
		inline CacheView( castor::String const & p_name
			, Initialiser && p_initialise
			, Cleaner && p_clean
			, CacheType & cache );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		inline ~CacheView();
		/**
		 *\~english
		 *\brief		Creates an element with the given informations.
		 *\param[in]	p_name		The element name.
		 *\param[in]	p_params	The parameters forwarded to the viewed cache.
		 *\return		The created element.
		 *\~french.=
		 *\brief		Crée un élément avec les informations données.
		 *\param[in]	p_name		Le nom de l'élément.
		 *\param[in]	p_params	Les paramètres transmis au cache vu.
		 *\return		L'élément créé.
		 */
		template< typename ... Params >
		inline ResourcePtr add( castor::String const & p_name, Params && ... p_params );
		/**
		 *\~english
		 *\brief		adds an already created an element.
		 *\param[in]	p_name		The element name.
		 *\param[in]	p_element	The element.
		 *\~french
		 *\brief		Ajoute un élément déjà créé.
		 *\param[in]	p_name		Le nom d'élément.
		 *\param[in]	p_element	L'élément.
		 */
		inline ResourcePtr add( castor::String const & p_name, ResourcePtr p_element );
		/**
		 *\~english
		 *\brief		Removes from the cache the elements created through this view.
		 *\~french
		 *\brief		Supprime du cache les éléments créés via cette vue.
		 */
		inline void clear();
		/**
		 *\~english
		 *\return		\p true if the view is empty.
		 *\~french
		 *\return		\p true si la vue est vide.
		 */
		inline bool isEmpty()const;
		/**
		 *\~english
		 *\param[in]	p_name	The object name.
		 *\return		\p true if an element with given name exists.
		 *\~french
		 *\param[in]	p_name	Le nom d'objet.
		 *\return		\p true Si un élément avec le nom donné existe.
		 */
		inline bool has( castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Looks for an element with given name.
		 *\param[in]	p_name	The object name.
		 *\return		The found element, nullptr if not found.
		 *\~french
		 *\brief		Cherche un élément par son nom.
		 *\param[in]	p_name	Le nom d'objet.
		 *\return		L'élément trouvé, nullptr si non trouvé.
		 */
		inline ResourcePtr find( castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Removes an object, given a name.
		 *\param[in]	p_name	The object name.
		 *\~french
		 *\brief		Retire un objet à partir d'un nom.
		 *\param[in]	p_name	Le nom d'objet.
		 */
		inline void remove( castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Returns an iterator to the first element of the collection
		 *\return		The iterator
		 *\~french
		 *\brief		Renvoie un itérateur sur le premier élément de la collection
		 *\return		L'itérateur
		 */
		inline auto begin()
		{
			return m_createdElements.begin();
		}
		/**
		 *\~english
		 *\brief		Returns an constant iterator to the first element of the collection
		 *\return		The iterator
		 *\~french
		 *\brief		Renvoie un itérateur constant sur le premier élément de la collection
		 *\return		L'itérateur
		 */
		inline auto begin()const
		{
			return m_createdElements.begin();
		}
		/**
		 *\~english
		 *\brief		Returns an iterator to the after last element of the collection
		 *\return		The iterator
		 *\~french
		 *\brief		Renvoie un itérateur sur l'après dernier élément de la collection
		 *\return		L'itérateur
		 */
		inline auto end()
		{
			return m_createdElements.end();
		}
		/**
		 *\~english
		 *\brief		Returns an constant iterator to the after last element of the collection
		 *\return		The iterator
		 *\~french
		 *\brief		Renvoie un itérateur constant sur l'après dernier élément de la collection
		 *\return		L'itérateur
		 */
		inline auto end()const
		{
			return m_createdElements.end();
		}

	private:
		//!\~english	The viewed cache.
		//!\~french		Le cache vu.
		CacheType & m_cache;
		//!\~english	The created elements names.
		//!\~french		Les noms des éléments créés.
		std::set< castor::String > m_createdElements;
		//!\~english	The element initialiser.
		//!\~french		L'initaliseur d'éléments.
		Initialiser m_initialise;
		//!\~english	The element cleaner.
		//!\~french		Le nettoyeur d'éléments.
		Cleaner m_clean;
		//!\~english	The elements that are being cleaned.
		//!\~french		Les éléments en cours de nettoyage.
		std::vector< ResourcePtr > m_cleaning;
	};
}

#include "CacheView.inl"

#endif
