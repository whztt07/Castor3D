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
#ifndef ___GL_RASTERISER_STATE_H___
#define ___GL_RASTERISER_STATE_H___

#include "GlHolder.hpp"

#include <State/RasteriserState.hpp>

namespace GlRender
{
	class GlRasteriserState
		: public Castor3D::RasteriserState
		, public Holder
	{
	public:
		GlRasteriserState( GlRenderSystem * p_renderSystem, OpenGl & p_gl );
		virtual ~GlRasteriserState();
		/**
		 *\copydoc Castor3D::RasteriserState::Initialise
		 */
		virtual bool Initialise()
		{
			return true;
		}
		/**
		 *\copydoc Castor3D::RasteriserState::Cleanup
		 */
		virtual void Cleanup()
		{
		}
		/**
		 *\copydoc Castor3D::RasteriserState::Apply
		 */
		virtual bool Apply();

	private:
		/**
		 *\copydoc Castor3D::RasteriserState::DoCreateCurrent
		 */
		virtual Castor3D::RasteriserStateSPtr DoCreateCurrent();

	private:
		GlRenderSystem * m_renderSystem;
	};
}

#endif
