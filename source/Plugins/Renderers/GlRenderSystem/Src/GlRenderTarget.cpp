#include "GlRenderTarget.hpp"
#include "GlRenderSystem.hpp"
#include "GlShaderProgram.hpp"
#include "OpenGl.hpp"
#include "GlFrameBuffer.hpp"
#include "GlRenderBufferAttachment.hpp"
#include "GlTextureAttachment.hpp"

#include <Scene.hpp>
#include <Logger.hpp>

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

GlRenderTarget::GlRenderTarget( OpenGl & p_gl, GlRenderSystem * p_renderSystem, eTARGET_TYPE p_type )
	: RenderTarget( *p_renderSystem->GetOwner(), p_type )
	, m_gl( p_gl )
	, m_renderSystem( p_renderSystem )
{
}

GlRenderTarget::~GlRenderTarget()
{
}

RenderBufferAttachmentSPtr GlRenderTarget::CreateAttachment( RenderBufferSPtr p_pRenderBuffer )const
{
	return std::make_shared< GlRenderBufferAttachment >( m_gl, p_pRenderBuffer );
}

TextureAttachmentSPtr GlRenderTarget::CreateAttachment( DynamicTextureSPtr p_pTexture )const
{
	return std::make_shared< GlTextureAttachment >( m_gl, p_pTexture );
}

FrameBufferSPtr GlRenderTarget::CreateFrameBuffer()const
{
	return std::make_shared< GlFrameBuffer >( m_gl, *m_renderSystem->GetOwner() );
}
