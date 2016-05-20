﻿#include "TextureAttachment.hpp"

#include "FrameBuffer.hpp"

#include "Texture/DynamicTexture.hpp"
#include "Texture/TextureImage.hpp"

using namespace Castor;

namespace Castor3D
{
	TextureAttachment::TextureAttachment( DynamicTextureSPtr p_texture )
		: FrameBufferAttachment( eATTACHMENT_TYPE_TEXTURE )
		, m_texture( p_texture )
		, m_target( eTEXTURE_TYPE_COUNT )
		, m_layer( 0 )
	{
	}

	TextureAttachment::~TextureAttachment()
	{
	}

	PxBufferBaseSPtr TextureAttachment::GetBuffer()const
	{
		return GetTexture()->GetImage().GetBuffer();
	}
}
