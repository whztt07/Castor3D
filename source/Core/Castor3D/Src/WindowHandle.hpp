﻿/*
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
#ifndef ___C3D_WINDOW_HANDLE_H___
#define ___C3D_WINDOW_HANDLE_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.1
	\date		09/01/2012
	\see		WindowHandle.hpp
	\~english
	\brief		Platform dependant window handle implementation
	\~english
	\brief		Implémentation d'un handle de fenêtre, dépendant de l'OS
	*/
	class C3D_API IWindowHandle
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		IWindowHandle() {}

	public:
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_copy	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_copy	L'objet à copier
		 */
		IWindowHandle( IWindowHandle const & CU_PARAM_UNUSED( p_copy ) ) {}
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_copy	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_copy	L'objet à déplacer
		 */
		IWindowHandle( IWindowHandle && CU_PARAM_UNUSED( p_copy ) ) {}
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_copy	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_copy	L'objet à copier
		 *\return		Une référence sur cet objet
		 */
		IWindowHandle & operator =( IWindowHandle const & CU_PARAM_UNUSED( p_copy ) )
		{
			return * this;
		}
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_copy	The object to move
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_copy	L'objet à déplacer
		 *\return		Une référence sur cet objet
		 */
		IWindowHandle & operator =( IWindowHandle && CU_PARAM_UNUSED( p_copy ) )
		{
			return * this;
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~IWindowHandle() {}
		/**
		 *\~english
		 *\brief		bool cast operator
		 *\~french
		 *\brief		opérateur de conversion en bool
		 */
		virtual operator bool() = 0;
	};
	/*!
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/01/2012
	\~english
	\brief Platform independant window handle implementation
	\~english
	\brief Implémentation d'un handle de fenêtre, indépendant de l'OS
	*/
	class C3D_API WindowHandle
	{
	private:
		IWindowHandleSPtr m_pHandle;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		WindowHandle();
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pHandle	The allocated handle
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pHandle	Le handle alloué
		 */
		WindowHandle( IWindowHandleSPtr p_pHandle );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_copy	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_copy	L'objet à copier
		 */
		WindowHandle( WindowHandle const & p_copy );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_copy	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_copy	L'objet à déplacer
		 */
		WindowHandle( WindowHandle && p_copy );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_copy	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_copy	L'objet à copier
		 *\return		Une référence sur cet objet
		 */
		WindowHandle & operator =( WindowHandle const & p_copy );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_copy	The object to move
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_copy	L'objet à déplacer
		 *\return		Une référence sur cet objet
		 */
		WindowHandle & operator =( WindowHandle && p_copy );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~WindowHandle();
		/**
		 *\~english
		 *\brief		bool cast operator
		 *\~french
		 *\brief		opérateur de conversion en bool
		 */
		operator bool();
		/**
		 *\~english
		 *\brief		Retrieves the window handle, casted in the wanted format (HWND, GLXDrawable, ...)
		 *\return		The window handle
		 *\~french
		 *\brief		Récupère le handle de la fenêtre, casté dans le format voulu (HWND, GLXDrawable, ...)
		 *\return		Le handle de la fenêtre
		 */
		template< class T >
		inline std::shared_ptr< T > GetInternal()const
		{
			return std::static_pointer_cast< T >( m_pHandle );
		}
	};
}

#endif