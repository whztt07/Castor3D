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
#ifndef ___GL_ONE_FRAME_VARIABLE_H___
#define ___GL_ONE_FRAME_VARIABLE_H___

#include <OneFrameVariable.hpp>

#include "GlFrameVariableBase.hpp"

namespace GlRender
{
	template< typename T >
	class GlOneFrameVariable
		: public Castor3D::OneFrameVariable< T >
		, public GlFrameVariableBase
	{
	public:
		GlOneFrameVariable( OpenGl & p_gl, uint32_t p_uiOcc, GlShaderProgram * p_pProgram = NULL );
		GlOneFrameVariable( OpenGl & p_gl, Castor3D::OneFrameVariable< T > * p_pVariable );
		virtual ~GlOneFrameVariable();

		virtual bool Initialise();
		virtual void Cleanup();
		virtual void Bind();
		virtual void Unbind() {}
	};
}

#include "GlOneFrameVariable.inl"

#endif