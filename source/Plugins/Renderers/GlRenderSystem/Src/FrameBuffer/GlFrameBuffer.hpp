/*
See LICENSE file in root folder
*/
#ifndef ___GL_FRAME_BUFFER_H___
#define ___GL_FRAME_BUFFER_H___

#include "GlRenderSystemPrerequisites.hpp"

#include "Common/GlBindable.hpp"

#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>

namespace GlRender
{
	class GlFrameBuffer
		: public castor3d::FrameBuffer
		, private Bindable< std::function< void( int, uint32_t * ) >
			, std::function< void( int, uint32_t const * ) >
			, std::function< void( uint32_t ) > >
	{
		using BindableType = Bindable< std::function< void( int, uint32_t * ) >
			, std::function< void( int, uint32_t const * ) >
			, std::function< void( uint32_t ) > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\para[in]		p_gl		The OpenGL APIs.
		 *\para[in]		engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\para[in]		p_gl		Les APIs OpenGL.
		 *\para[in]		engine	Le moteur.
		 */
		GlFrameBuffer( OpenGl & p_gl, castor3d::Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlFrameBuffer();
		/**
		 *\copydoc		castor3d::FrameBuffer::initialise
		 */
		bool initialise()override;
		/**
		 *\copydoc		castor3d::FrameBuffer::cleanup
		 */
		void cleanup()override;
		/**
		 *\copydoc		castor3d::FrameBuffer::setDrawBuffers
		 */
		void setDrawBuffers( AttachArray const & p_attaches )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::setReadBuffer
		 */
		void setReadBuffer( castor3d::AttachmentPoint p_eAttach, uint8_t p_index )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::isComplete
		 */
		bool isComplete()const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::downloadBuffer
		 */
		void downloadBuffer( castor3d::AttachmentPoint p_point, uint8_t p_index, castor::PxBufferBaseSPtr p_buffer )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::createColourRenderBuffer
		 */
		castor3d::ColourRenderBufferSPtr createColourRenderBuffer( castor::PixelFormat p_format )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::createDepthStencilRenderBuffer
		 */
		castor3d::DepthStencilRenderBufferSPtr createDepthStencilRenderBuffer( castor::PixelFormat p_format )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::createAttachment
		 */
		castor3d::RenderBufferAttachmentSPtr createAttachment( castor3d::RenderBufferSPtr p_renderBuffer )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::createAttachment
		 */
		castor3d::TextureAttachmentSPtr createAttachment( castor3d::TextureLayoutSPtr p_texture )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::createAttachment
		 */
		castor3d::TextureAttachmentSPtr createAttachment( castor3d::TextureLayoutSPtr texture
			, uint32_t mipLevel )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::createAttachment
		 */
		castor3d::TextureAttachmentSPtr createAttachment( castor3d::TextureLayoutSPtr p_texture, castor3d::CubeMapFace p_face )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::createAttachment
		 */
		castor3d::TextureAttachmentSPtr createAttachment( castor3d::TextureLayoutSPtr p_texture, castor3d::CubeMapFace p_face, uint32_t p_mipLevel )const override;

	public:
		using BindableType::getGlName;
		using BindableType::getOpenGl;
		using BindableType::isValid;

	private:
		/**
		 *\copydoc		castor3d::FrameBuffer::doBind
		 */
		void doBind( castor3d::FrameBufferTarget p_target )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::doUnbind
		 */
		void doUnbind()const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::doBlitInto
		 */
		void doBlitInto(
			castor3d::FrameBuffer const & p_buffer,
			castor::Rectangle const & p_rect,
			castor::FlagCombination< castor3d::BufferComponent > const & p_components )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::doStretchInto
		 */
		void doStretchInto(
			castor3d::FrameBuffer const & p_buffer,
			castor::Rectangle const & p_rectSrc,
			castor::Rectangle const & p_rectDst,
			castor::FlagCombination< castor3d::BufferComponent > const & p_components,
			castor3d::InterpolationMode p_interpolation )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::doClear
		 */
		void doClear( castor3d::BufferComponents p_targets )const override;

	private:
		mutable GlFrameBufferMode m_bindingMode;
	};
}

#endif
