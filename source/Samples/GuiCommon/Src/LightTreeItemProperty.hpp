/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___GUICOMMON_LIGHT_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_LIGHT_TREE_ITEM_PROPERTY_H___

#include "TreeItemProperty.hpp"

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Geometry helper class to communicate between Scene objects or Materials lists and wxPropertiesHolder
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de sc�ne, ou la liste de mat�riaux, et wxPropertiesHolder, pour les g�om�tries
	*/
	class wxLightTreeItemProperty
		: public wxTreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pGeometry	 The target geometry
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pGeometry	La g�om�trie cible
		 */
		wxLightTreeItemProperty( Castor3D::LightSPtr p_light );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~wxLightTreeItemProperty();
		/**
		 *\~english
		 *\brief		Creates and fills the overlay properties, in the given wxPropertyGrid
		 *\param[in]	p_grid	The target wxPropertyGrid
		 *\~french
		 *\brief		Construit et remplit les propri�t�s de l'incrustation, dans la wxPropertyGrid donn�e
		 *\param[in]	p_grid	La wxPropertyGrid cible
		 */
		virtual void CreateProperties( wxPropertyGrid * p_grid );
		/**
		 *\~english
		 *\brief		Call when a property grid property is changed
		 *\param[in]	p_event	The event
		 *\~french
		 *\brief		Appel�e lorsqu'une propri�t� est chang�e
		 *\param[in]	p_event	L'�v�nement
		 */
		virtual void OnPropertyChange( wxPropertyGridEvent & p_event );
		/**
		 *\~english
		 *\brief		Retrieves the geometry
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re la g�om�trie
		 *\return		La valeur
		 */
		inline Castor3D::LightSPtr GetLight()
		{
			return m_light.lock();
		}

	private:
		void DoCreateDirectionalLightProperties( wxPropertyGrid * p_grid, Castor3D::DirectionalLightSPtr p_light );
		void DoCreatePointLightProperties( wxPropertyGrid * p_grid, Castor3D::PointLightSPtr p_light );
		void DoCreateSpotLightProperties( wxPropertyGrid * p_grid, Castor3D::SpotLightSPtr p_light );
		void OnDirectionalLightPropertyChanged( wxPropertyGridEvent & p_event );
		void OnPointLightPropertyChanged( wxPropertyGridEvent & p_event );
		void OnSpotLightPropertyChanged( wxPropertyGridEvent & p_event );

	private:
		Castor3D::LightWPtr m_light;
	};
}

#endif
