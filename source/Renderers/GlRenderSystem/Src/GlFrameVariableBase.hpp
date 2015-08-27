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
#ifndef ___GL_FRAME_VARIABLE_BASE_H___
#define ___GL_FRAME_VARIABLE_BASE_H___

#include "GlRenderSystemPrerequisites.hpp"

#include "GlShaderProgram.hpp"

namespace GlRender
{
	class GlFrameVariableBase
		: public Castor::NonCopyable
	{
	public:
		GlFrameVariableBase( OpenGl & p_gl, uint32_t * p_uiProgram );
		virtual ~GlFrameVariableBase();
		virtual void Bind() = 0;

		inline uint32_t GetGlIndex() const
		{
			return m_iGlIndex;
		}

	protected:
		void GetVariableLocation( char const * p_pVarName );
		template <typename Type> void DoBind( Type const * p_pValue, uint32_t p_uiOcc );
		template <typename Type, int Count> void DoBind( Type const * p_pValue, uint32_t p_uiOcc );
		template <typename Type, int Rows, int Columns> void DoBind( Type const * p_pValue, uint32_t p_uiOcc );

	protected:
		uint32_t m_iGlIndex;
		uint32_t * m_uiParentProgram;
		bool m_bPresentInProgram;
		OpenGl & m_gl;
	};
}

#include "GlFrameVariableBase.inl"

#endif