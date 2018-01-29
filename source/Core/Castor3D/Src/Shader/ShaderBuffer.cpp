#include "ShaderBuffer.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

#include <Buffer/Buffer.hpp>
#include <Core/Device.hpp>
#include <Descriptor/DescriptorSet.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		renderer::BufferBasePtr doGetSsbo( Engine & engine
			, uint32_t size )
		{
			renderer::BufferBasePtr result;

			if ( engine.getRenderSystem()->getGpuInformations().hasFeature( GpuFeature::eShaderStorageBuffers ) )
			{
				result = engine.getRenderSystem()->getCurrentDevice()->createBuffer( size
					, renderer::BufferTarget::eStorageBuffer
					, renderer::MemoryPropertyFlag::eHostVisible );
			}

			return result;
		}

		renderer::UniformBufferBasePtr doGetTbo( Engine & engine
			, uint32_t size
			, bool isTbo )
		{
			renderer::UniformBufferBasePtr result;

			if ( isTbo )
			{
				result = engine.getRenderSystem()->getCurrentDevice()->createUniformBuffer( 1u
					, size
					, renderer::BufferTarget::eUniformTexelBuffer
					, renderer::MemoryPropertyFlag::eHostVisible );
			}

			return result;
		}
	}

	//*********************************************************************************************

	ShaderBuffer::ShaderBuffer( Engine & engine
		, uint32_t size )
		: m_ssbo{ doGetSsbo( engine, size ) }
		, m_tbo{ doGetTbo( engine, size, m_ssbo == nullptr ) }
		, m_data( size_t( size ), uint8_t( 0 ) )
	{
	}

	ShaderBuffer::~ShaderBuffer()
	{
		m_ssbo.reset();
		m_tbo.reset();
	}

	void ShaderBuffer::update()
	{
		if ( m_ssbo )
		{
			if ( uint8_t * buffer = m_ssbo->lock( 0u
				, uint32_t( m_data.size() )
				, renderer::MemoryMapFlag::eWrite ) )
			{
				std::memcpy( buffer, m_data.data(), m_data.size() );
				m_ssbo->unlock( uint32_t( m_data.size() ), true );
			}
		}
		else
		{
			if ( uint8_t * buffer = m_tbo->getBuffer().lock( 0u
				, uint32_t( m_data.size() )
				, renderer::MemoryMapFlag::eWrite ) )
			{
				std::memcpy( buffer, m_data.data(), m_data.size() );
				m_tbo->getBuffer().unlock( uint32_t( m_data.size() ), true );
			}
		}
	}

	renderer::DescriptorSetLayoutBinding ShaderBuffer::createLayoutBinding( uint32_t index )const
	{
		if ( m_ssbo )
		{
			return { index, renderer::DescriptorType::eStorageBuffer, renderer::ShaderStageFlag::eFragment };
		}
		else
		{
			return { index, renderer::DescriptorType::eUniformTexelBuffer, renderer::ShaderStageFlag::eFragment };
		}
	}

	renderer::DescriptorSetBinding const & ShaderBuffer::createBinding( renderer::DescriptorSet & descriptorSet
		, renderer::DescriptorSetLayoutBinding const & binding )const
	{
		if ( m_ssbo )
		{
			return descriptorSet.createBinding( binding
				, *m_ssbo
				, 0u
				, uint32_t( m_data.size() ) );
		}
		else
		{
			return descriptorSet.createBinding( binding
				, *m_tbo
				, 0u
				, uint32_t( m_data.size() ) );
		}
	}
}
