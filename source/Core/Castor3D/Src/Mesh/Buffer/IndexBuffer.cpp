#include "IndexBuffer.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	IndexBuffer::IndexBuffer( Engine & p_engine )
		: CpuBuffer< uint32_t >( p_engine )
	{
	}

	IndexBuffer::~IndexBuffer()
	{
	}

	bool IndexBuffer::Initialise( BufferAccessType p_type, BufferAccessNature p_nature )
	{
		if ( !m_gpuBuffer )
		{
			m_gpuBuffer = GetEngine()->GetRenderSystem()->CreateIndexBuffer();
		}

		bool l_return = m_gpuBuffer != nullptr;

		if ( l_return )
		{
			l_return = DoInitialise( p_type, p_nature );
		}

		return l_return;
	}

	void IndexBuffer::Cleanup()
	{
		DoCleanup();
	}
}
