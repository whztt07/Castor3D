#include "Gl3VertexBufferObject.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	Gl3VertexBufferObject::Gl3VertexBufferObject( GlRenderSystem & p_renderSystem, OpenGl & p_gl, BufferDeclaration const & p_declaration, HardwareBufferPtr p_pBuffer )
		: GlVertexBufferObject( p_renderSystem, p_gl, p_declaration, p_pBuffer )
	{
	}

	Gl3VertexBufferObject::~Gl3VertexBufferObject()
	{
	}

	bool Gl3VertexBufferObject::Bind()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();
		bool l_return = l_pBuffer && l_pBuffer->IsAssigned();

		if ( l_return )
		{
			l_return = DoBind();
		}

		if ( l_return )
		{
			l_return = DoAttributesBind();
		}

		return l_return;
	}

	void Gl3VertexBufferObject::Unbind()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer && l_pBuffer->IsAssigned() )
		{
			DoAttributesUnbind();
		}
	}
}
