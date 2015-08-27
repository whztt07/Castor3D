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
#ifndef ___C3D_COLOUR_RENDER_BUFFER_H___
#define ___C3D_COLOUR_RENDER_BUFFER_H___

#include "RenderBuffer.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Render buffer receiving colour component
	\~french
	\brief		Tampon de rendu recevant une composante de couleur
	*/
	class C3D_API ColourRenderBuffer
		:	public RenderBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_ePixelFormat	The buffer pixel format
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_ePixelFormat	Le format des pixels du tampon
		 */
		ColourRenderBuffer( Castor::ePIXEL_FORMAT p_eFormat );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~ColourRenderBuffer();
	};
}

#endif