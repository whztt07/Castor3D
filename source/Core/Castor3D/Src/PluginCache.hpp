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
#ifndef ___C3D_PLUGIN_CACHE_H___
#define ___C3D_PLUGIN_CACHE_H___

#include "Cache/Cache.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to specialise a cache behaviour.
	\remarks	Specialisation for Plugin.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache.
	\remarks	Spécialisation pour Plugin.
	*/
	template< typename KeyType >
	struct CacheTraits< Plugin, KeyType >
	{
		C3D_API static const Castor::String Name;
		using Producer = std::function< std::shared_ptr< Plugin >( KeyType const &, ePLUGIN_TYPE, Castor::DynamicLibrarySPtr ) >;
		using Merger = std::function< void( CacheBase< Plugin, KeyType > const &
											, Castor::Collection< Plugin, KeyType > &
											, std::shared_ptr< Plugin > ) >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Plug-ins cache.
	\~french
	\brief		Cache de plug-ins.
	*/
	template<>
	class Cache< Plugin, Castor::String >
		: public CacheBase< Plugin, Castor::String >
	{
	public:
		using MyCacheType = CacheBase< Plugin, Castor::String >;
		using Element = typename MyCacheType::Element;
		using Key = typename MyCacheType::Key;
		using Collection = typename MyCacheType::Collection;
		using ElementPtr = typename MyCacheType::ElementPtr;
		using Producer = typename MyCacheType::Producer;
		using Initialiser = typename MyCacheType::Initialiser;
		using Cleaner = typename MyCacheType::Cleaner;
		using Merger = typename MyCacheType::Merger;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The owner.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le propriétaire.
		 */
		C3D_API Cache( Engine & p_engine
					   , Producer && p_produce
					   , Initialiser && p_initialise = Initialiser{}
					   , Cleaner && p_clean = Cleaner{}
					   , Merger && p_merge = Merger{} );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~Cache();
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\~french
		 *\brief		Vide la collection.
		 */
		C3D_API void Clear();
		/**
		 *\~english
		 *\brief		Loads a plug-in, given the plug-in name (ex: libGlRenderSystem.dll => GlRenderSystem)
		 *\param[in]	p_pluginName	The plug-in name
		 *\param[in]	p_pathFolder	Optional path where the plug-in may be looked for
		 *\return		The loaded plug-in, \p nullptr if name was incorrect or path was not valid
		 *\~french
		 *\brief		Charge un plug-in, selon son nom (ex : libGlRenderSystem.dll => GlRenderSystem)
		 *\param[in]	p_pluginName	Le nom du plug-in
		 *\param[in]	p_pathFolder	Un chemin optionnel, pour y trouver le plug-in
		 *\return		Le plug-in chargé, \p nullptr si non trouvé (nom incorrect ou non trouvé dans le chemin donné ou le chemin principal)
		 */
		C3D_API PluginSPtr LoadPlugin( Castor::String const & p_pluginName, Castor::Path const & p_pathFolder )throw();
		/**
		 *\~english
		 *\brief		Loads a plug-in, given the plug-in file's full path
		 *\param[in]	p_fileFullPath	The plug-in file path
		 *\return		The loaded plug-in, \p nullptr if path was incorrect or if it wasn't a valid plug-in
		 *\~french
		 *\brief		Charge un plug-in dont le chemin est donné
		 *\param[in]	p_fileFullPath	Le chemin du plug-in
		 *\return		Le plug-in chargé, \p nullptr si le chemin était incorrect ou s'il ne représentait pas un plug-in valide
		 */
		C3D_API PluginSPtr LoadPlugin( Castor::Path const & p_fileFullPath )throw();
		/**
		 *\~english
		 *\brief		Retrieves the plug-ins of given type
		 *\param[in]	p_type	The plug-ins type
		 *\return		\p nullptr if not found
		 *\~french
		 *\brief		Récupère les plug-ins du type donné
		 *\param[in]	p_type	Le type de plu-ins
		 *\return		\p nullptr si non trouvé
		 */
		C3D_API PluginStrMap GetPlugins( ePLUGIN_TYPE p_type );
		/**
		 *\~english
		 *\brief		Loads all the plug-ins located in working folder
		 *\param[in]	p_folder	The plug-ins' folder
		 *\~french
		 *\brief		Charge tous les plug-ins d'un dossier donné
		 *\param[in]	p_folder	Le dossier
		 */
		C3D_API void LoadAllPlugins( Castor::Path const & p_folder );

	private:
		PluginSPtr DoLoadPlugin( Castor::Path const & p_pathFile );

	private:
		//!\~english	The loaded shared libraries map.
		//!\~french		La map des shared libraries chargées.
		DynamicLibraryPtrPathMapArray m_libraries;
		//!\~english	The mutex protecting the loaded shared libraries map.
		//!\~french		Le mutex protégeant la map des shared libraries chargées.
		std::recursive_mutex m_mutexLibraries;
		//!\~english	The loaded plug-ins map.
		//!\~french		La map des plug-ins chargés.
		PluginStrMapArray m_loadedPlugins;
		//!\~english	The mutex protecting the loaded plug-ins map.
		//!\~french		Le mutex protégeant la map des plug-ins chargés.
		std::recursive_mutex m_mutexLoadedPlugins;
		//!\~english	The loaded plug-ins map, sorted by plug-in type.
		//!\~french		La map des plug-ins chargés, triés par type de plug-in.
		PluginTypePathMap m_loadedPluginTypes;
		//!\~english	The mutex protecting the loaded plug-ins map sorted by type.
		//!\~french		Le mutex protégeant la map de plug-ins chargés triés par type.
		std::recursive_mutex m_mutexLoadedPluginTypes;
	};
	using PluginCache = Cache< Plugin, Castor::String >;
	DECLARE_SMART_PTR( PluginCache );
}

#endif
