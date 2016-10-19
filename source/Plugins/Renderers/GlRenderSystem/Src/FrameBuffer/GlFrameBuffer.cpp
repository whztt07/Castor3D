#include "FrameBuffer/GlFrameBuffer.hpp"

#include "Common/OpenGl.hpp"
#include "FrameBuffer/GlColourRenderBuffer.hpp"
#include "FrameBuffer/GlDepthStencilRenderBuffer.hpp"
#include "FrameBuffer/GlRenderBufferAttachment.hpp"
#include "FrameBuffer/GlTextureAttachment.hpp"
#include "FrameBuffer/GlCubeTextureFaceAttachment.hpp"
#include "Texture/GlTexture.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlFrameBuffer::GlFrameBuffer( OpenGl & p_gl, Engine & p_engine )
		: BindableType( p_gl,
						"GlFrameBuffer",
						std::bind( &OpenGl::GenFramebuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGl::DeleteFramebuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGl::IsFramebuffer, std::ref( p_gl ), std::placeholders::_1 ),
						[&p_gl, this]( uint32_t p_glName )
						{
							return p_gl.BindFramebuffer( m_bindingMode, p_glName );
						} )
		, FrameBuffer( p_engine )
	{
	}

	GlFrameBuffer::~GlFrameBuffer()
	{
	}

	bool GlFrameBuffer::Create()
	{
		return BindableType::Create( false );
	}

	void GlFrameBuffer::Destroy()
	{
		BindableType::Destroy();
	}

	void GlFrameBuffer::SetDrawBuffers( AttachArray const & p_attaches )const
	{
		bool l_return = false;

		if ( !p_attaches.empty() )
		{
			UIntArray l_arrayAttaches;
			l_arrayAttaches.reserve( p_attaches.size() );

			for ( auto l_attach : p_attaches )
			{
				AttachmentPoint l_eAttach = l_attach->GetAttachmentPoint();

				if ( l_eAttach == AttachmentPoint::Colour )
				{
					l_arrayAttaches.push_back( GetOpenGl().Get( l_eAttach ) + l_attach->GetAttachmentIndex() );
				}
			}

			if ( !l_arrayAttaches.empty() )
			{
				GetOpenGl().DrawBuffers( int( l_arrayAttaches.size() ), &l_arrayAttaches[0] );
			}
			else
			{
				GetOpenGl().DrawBuffer( eGL_BUFFER_NONE );
			}
		}
		else
		{
			GetOpenGl().DrawBuffer( eGL_BUFFER_NONE );
		}
	}

	void GlFrameBuffer::SetReadBuffer( AttachmentPoint p_eAttach, uint8_t p_index )const
	{
		auto l_it = std::find_if( m_attaches.begin(), m_attaches.end(), [p_eAttach]( FrameBufferAttachmentSPtr p_attach )
		{
			return p_attach->GetAttachmentPoint() == p_eAttach;
		} );

		if ( l_it != m_attaches.end() )
		{
			GetOpenGl().ReadBuffer( GetOpenGl().Get( GetOpenGl().Get( p_eAttach ) ) );
		}
	}

	bool GlFrameBuffer::IsComplete()const
	{
		return GetOpenGl().CheckFramebufferStatus( eGL_FRAMEBUFFER_MODE_DEFAULT ) == eGL_FRAMEBUFFER_COMPLETE;
	}

	bool GlFrameBuffer::DownloadBuffer( AttachmentPoint p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )
	{
		bool l_return = DoBind( FrameBufferTarget::Read );

		if ( l_return )
		{
			l_return = GetOpenGl().ReadBuffer( eGL_BUFFER( GetOpenGl().Get( GetOpenGl().Get( p_point ) ) + p_index ) );

			if ( l_return )
			{
				OpenGl::PixelFmt l_pxFmt = GetOpenGl().Get( p_buffer->format() );
				l_return = GetOpenGl().ReadPixels( Position(), p_buffer->dimensions(), l_pxFmt.Format, l_pxFmt.Type, p_buffer->ptr() );
			}

			DoUnbind();
		}

		return l_return;
	}

	ColourRenderBufferSPtr GlFrameBuffer::CreateColourRenderBuffer( PixelFormat p_format )
	{
		return std::make_shared< GlColourRenderBuffer >( GetOpenGl(), p_format );
	}

	DepthStencilRenderBufferSPtr GlFrameBuffer::CreateDepthStencilRenderBuffer( PixelFormat p_format )
	{
		return std::make_shared< GlDepthStencilRenderBuffer >( GetOpenGl(), p_format );
	}

	RenderBufferAttachmentSPtr GlFrameBuffer::CreateAttachment( RenderBufferSPtr p_renderBuffer )
	{
		return std::make_shared< GlRenderBufferAttachment >( GetOpenGl(), p_renderBuffer );
	}

	TextureAttachmentSPtr GlFrameBuffer::CreateAttachment( TextureLayoutSPtr p_texture )
	{
		return std::make_shared< GlTextureAttachment >( GetOpenGl(), p_texture );
	}

	TextureAttachmentSPtr GlFrameBuffer::CreateAttachment( TextureLayoutSPtr p_texture, CubeMapFace p_face )
	{
		return std::make_shared< GlCubeTextureFaceAttachment >( GetOpenGl(), p_texture, p_face );
	}

	void GlFrameBuffer::DoClear( uint32_t p_uiTargets )
	{
		GetOpenGl().ClearColor( m_redClear, m_greenClear, m_blueClear, m_alphaClear );
		GetOpenGl().ClearDepth( 1.0 );
		GetOpenGl().Clear( GetOpenGl().GetComponents( p_uiTargets ) );
	}

	bool GlFrameBuffer::DoBind( FrameBufferTarget p_target )const
	{
		m_bindingMode = GetOpenGl().Get( p_target );
		return BindableType::Bind();
	}

	void GlFrameBuffer::DoUnbind()const
	{
		BindableType::Unbind();
	}

	bool GlFrameBuffer::DoBlitInto( FrameBuffer const & p_buffer, Castor::Rectangle const & p_rect, uint32_t p_components )const
	{
		return GetOpenGl().BlitFramebuffer( p_rect, p_rect, GetOpenGl().GetComponents( p_components ), eGL_INTERPOLATION_MODE_NEAREST );
	}

	bool GlFrameBuffer::DoStretchInto( FrameBuffer const & p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_components, InterpolationMode p_interpolation )const
	{
		return GetOpenGl().BlitFramebuffer( p_rectSrc, p_rectDst, GetOpenGl().GetComponents( p_components ), GetOpenGl().Get( p_interpolation ) );
	}
}
