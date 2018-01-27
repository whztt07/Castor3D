#include "IblTextures.hpp"

#include "Engine.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Buffer/UniformBuffer.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/Scissor.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <Pipeline/Viewport.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassState.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>
#include <RenderPass/TextureAttachment.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace
	{
		TextureUnit doCreatePrefilteredBrdf( Engine & engine )
		{
			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, renderer::TextureType::e2D
				, renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled
				, renderer::MemoryPropertyFlag::eDeviceLocal
				, PixelFormat::eRGB32F
				, Size{ 512u, 512u } );
			SamplerSPtr sampler;
			auto name = cuT( "IblTexturesBRDF" );

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().create( name );
				sampler->setMinFilter( renderer::Filter::eLinear );
				sampler->setMagFilter( renderer::Filter::eLinear );
				sampler->setWrapS( renderer::WrapMode::eClampToEdge );
				sampler->setWrapT( renderer::WrapMode::eClampToEdge );
				sampler->setWrapR( renderer::WrapMode::eClampToEdge );
				engine.getSamplerCache().add( name, sampler );
			}

			sampler->initialise();
			TextureUnit result{ engine };
			result.setTexture( texture );
			result.setSampler( sampler );
			result.initialise();
			return result;
		}
	}

	//************************************************************************************************

	IblTextures::IblTextures( Scene & scene
		, TextureLayout const & source )
		: OwnedBy< Scene >{ scene }
		, m_prefilteredBrdf{ doCreatePrefilteredBrdf( *scene.getEngine() ) }
		, m_radianceComputer{ *scene.getEngine(), Size{ 512, 512 }, source.getTexture() }
		, m_environmentPrefilter{ *scene.getEngine(), Size{ 128u, 128u }, source.getTexture() }
	{
		BrdfPrefilter filter{ *scene.getEngine()
			, m_prefilteredBrdf.getTexture()->getDimensions()
			, m_prefilteredBrdf.getTexture()->getView() };
		filter.render();
	}

	IblTextures::~IblTextures()
	{
		m_prefilteredBrdf.cleanup();
	}

	void IblTextures::update()
	{
		m_radianceComputer.render();
		m_environmentPrefilter.render();
	}

	void IblTextures::debugDisplay( Size const & renderSize )const
	{
		int width = int( m_prefilteredBrdf.getTexture()->getWidth() );
		int height = int( m_prefilteredBrdf.getTexture()->getHeight() );
		int left = 0u;
		int top = renderSize.getHeight() - height;
		auto size = Size( width, height );
		//auto & context = *getScene()->getEngine()->getRenderSystem()->getCurrentContext();
		//context.renderTexture( Position{ left, top }
		//	, size
		//	, *m_prefilteredBrdf.getTexture() );
		//left += 512;
		//context.renderTextureCube( Position{ left, top }
		//	, Size( width / 4, height / 4u )
		//	, *m_prefilteredEnvironment.getTexture() );
		//left += 512;
		//context.renderTextureCube( Position{ left, top }
		//	, Size( width / 4, height / 4u )
		//	, *m_radianceTexture.getTexture() );
	}
}
