#include "MorphingUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

#include <Buffer/UniformBuffer.hpp>

using namespace castor;

namespace castor3d
{
	String const MorphingUbo::BufferMorphing = cuT( "Morphing" );
	String const MorphingUbo::Time = cuT( "c3d_time" );

	MorphingUbo::MorphingUbo( Engine & engine )
		: m_engine{ engine }
	{
	}

	MorphingUbo::~MorphingUbo()
	{
	}

	void MorphingUbo::initialise()
	{
		auto & device = *m_engine.getRenderSystem()->getCurrentDevice();
		m_ubo = renderer::makeUniformBuffer< Configuration >( device
			, 1u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible );
	}

	void MorphingUbo::cleanup()
	{
		m_ubo.reset();
	}

	void MorphingUbo::update( float time )const
	{
		auto & configuration = m_ubo->getData( 0u );
		configuration.time = time;
		m_ubo->upload();
	}
}
