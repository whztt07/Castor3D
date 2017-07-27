#include "MorphingUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	MorphingUbo::MorphingUbo( Engine & engine )
		: m_ubo{ ShaderProgram::BufferMorphing
			, *engine.GetRenderSystem()
			, MorphingUbo::BindingPoint }
		, m_time{ *m_ubo.CreateUniform< UniformType::eFloat >( ShaderProgram::Time ) }
	{
	}

	MorphingUbo::~MorphingUbo()
	{
	}

	void MorphingUbo::Update( float p_time )const
	{
		m_time.SetValue( p_time );
		m_ubo.Update();
		m_ubo.BindTo( MorphingUbo::BindingPoint );
	}
}
