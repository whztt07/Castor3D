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
#ifndef ___GL_UPLOAD_PIXEL_BUFFER_H___
#define ___GL_UPLOAD_PIXEL_BUFFER_H___

#include "GlGpuIoBuffer.hpp"

#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/StaticTexture.hpp>
#include <Texture/DynamicTexture.hpp>

namespace GlRender
{
	/*!
	\author		Sylvain DOREMUS
	\brief		Unpack (upload to VRAM) buffer implementation
	*/
	class GlUploadPixelBuffer
		: public GlGpuIoBuffer
	{
	public:
		GlUploadPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_renderSystem, uint8_t * p_pixels, uint32_t p_pixelsSize );
		GlUploadPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_renderSystem, Castor::PxBufferBaseSPtr p_pixels );
		virtual ~GlUploadPixelBuffer();

		virtual bool Initialise();
	};
}

#endif
