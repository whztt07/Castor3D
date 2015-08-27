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
#ifndef ___DX11_PIPELINE_H___
#define ___DX11_PIPELINE_H___

#include "Dx11RenderSystemPrerequisites.hpp"

#include <Pipeline.hpp>

namespace Dx11Render
{
	class DxPipelineImplHlsl
		:	public Castor3D::IPipelineImpl
	{
	public:
		DxPipelineImplHlsl( DxPipeline * p_pPipeline );
		virtual ~DxPipelineImplHlsl();

		virtual Castor3D::eMTXMODE MatrixMode( Castor3D::eMTXMODE p_eMode );
		virtual bool LoadIdentity();
		virtual bool PushMatrix();
		virtual bool PopMatrix();
		virtual bool MultMatrix( Castor::Matrix4x4r const & p_matrix );
		virtual bool MultMatrix( real const * p_matrix );
		virtual bool Perspective( Castor::Angle const & p_aFOVY, real p_rRatio, real p_rNear, real p_rFar );
		virtual bool Frustum( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar );
		virtual bool Ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar );
		virtual void ApplyViewport( int p_iWindowWidth, int p_iWindowHeight );

	private:
		void DoApplyMatrix();

	private:
		DxPipeline * m_pDxPipeline;
		D3D11_VIEWPORT m_viewport;
	};

	class DxPipeline
		:	public Castor3D::Pipeline
	{
	protected:
		DxPipelineImplHlsl * m_pPipelineImplHlsl;
		DxRenderSystem * m_pDxRenderSystem;

	public:
		DxPipeline( DxRenderSystem * p_pRenderSystem );
		~DxPipeline();

		virtual void Initialise();
		virtual void UpdateFunctions( Castor3D::ShaderProgramBase * p_pProgram );

		DxRenderSystem * GetDxRenderSystem()const
		{
			return m_pDxRenderSystem;
		}
	};
}

#endif