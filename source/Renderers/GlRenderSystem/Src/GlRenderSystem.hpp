/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GL_RENDER_SYSTEM_H___
#define ___GL_RENDER_SYSTEM_H___

#include "GlRenderSystemPrerequisites.hpp"
#include "GlPipeline.hpp"
#include "GlContext.hpp"

#include <RenderSystem.hpp>

namespace GlRender
{
	class GlRenderSystem
		: public Castor3D::RenderSystem
	{
	public:
		GlRenderSystem( Castor3D::Engine * p_pEngine );
		virtual ~GlRenderSystem();

		void CheckShaderSupport();	//!< Checks the different shader types support.
		bool InitOpenGlExtensions();	//!< Initialize OpenGL Extensions

		virtual void Initialise( Castor::String const & p_strExtensions );
		virtual void Delete();
		virtual bool CheckSupport( Castor3D::eSHADER_MODEL p_eProfile );
		virtual Castor3D::ContextSPtr CreateContext();
		virtual Castor3D::GeometryBuffersSPtr CreateGeometryBuffers( Castor3D::VertexBufferUPtr p_pVertexBuffer, Castor3D::IndexBufferUPtr p_pIndexBuffer, Castor3D::MatrixBufferUPtr p_pMatrixBuffer );
		virtual Castor3D::DepthStencilStateSPtr CreateDepthStencilState();
		virtual Castor3D::RasteriserStateSPtr CreateRasteriserState();
		virtual Castor3D::BlendStateSPtr CreateBlendState();
		virtual Castor3D::SamplerSPtr CreateSampler( Castor::String const & p_name );
		virtual Castor3D::RenderTargetSPtr CreateRenderTarget( Castor3D::eTARGET_TYPE p_eType );
		virtual Castor3D::RenderWindowSPtr CreateRenderWindow();
		virtual Castor3D::ShaderProgramBaseSPtr CreateGlslShaderProgram();
		virtual Castor3D::ShaderProgramBaseSPtr CreateShaderProgram();
		virtual Castor3D::OverlayRendererSPtr CreateOverlayRenderer();
		virtual std::shared_ptr< Castor3D::GpuBuffer< uint32_t > > CreateIndexBuffer( Castor3D::CpuBuffer<uint32_t> * p_pBuffer );
		virtual std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > CreateVertexBuffer( Castor3D::BufferDeclaration const & p_declaration, Castor3D::CpuBuffer< uint8_t > * p_pBuffer );
		virtual std::shared_ptr< Castor3D::GpuBuffer< real > > CreateMatrixBuffer( Castor3D::CpuBuffer< real > * p_pBuffer );
		virtual std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > CreateTextureBuffer( Castor3D::CpuBuffer<uint8_t > * p_pBuffer );
		virtual Castor3D::StaticTextureSPtr CreateStaticTexture();
		virtual Castor3D::DynamicTextureSPtr CreateDynamicTexture();
		virtual Castor3D::FrameVariableBufferSPtr CreateFrameVariableBuffer( Castor::String const & p_strName );
		virtual Castor3D::BillboardListSPtr CreateBillboardsList( Castor3D::SceneSPtr p_pScene );

		virtual Castor3D::ShaderProgramBaseSPtr DoCreateHlslShaderProgram()
		{
			return nullptr;
		}

		virtual	bool NeedsMatrixTransposition()const
		{
			return false;
		}

		virtual bool SupportsDepthBuffer()const
		{
			return true;
		}

		inline bool UseVertexBufferObjects()
		{
			return m_useVertexBufferObjects;
		}

		inline bool IsExtensionInit()
		{
			return m_extensionsInit;
		}

		inline int GetOpenGlMajor()
		{
			return m_iOpenGlMajor;
		}

		inline int GetOpenGlMinor()
		{
			return m_iOpenGlMinor;
		}

		inline void SetOpenGlVersion( int p_iMajor, int p_iMinor )
		{
			m_iOpenGlMajor = p_iMajor;
			m_iOpenGlMinor = p_iMinor;
		}

	private:
		virtual void DoInitialise();
		virtual void DoCleanup();

	protected:
		bool m_useVertexBufferObjects;
		bool m_extensionsInit;
		int m_iOpenGlMajor;
		int m_iOpenGlMinor;
		Castor::String m_strExtensions;
		OpenGl m_gl;
	};
}

#endif