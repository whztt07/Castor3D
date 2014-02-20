#include "Dx10RenderSystem/PrecompiledHeader.hpp"

#include "Dx10RenderSystem/DxCameraRenderer.hpp"
#include "Dx10RenderSystem/DxRenderSystem.hpp"

using namespace Castor3D;
using namespace Dx10Render;

DxCameraRenderer :: DxCameraRenderer( DxRenderSystem * p_pRenderSystem )
	:	CameraRenderer( p_pRenderSystem )
{
}

DxCameraRenderer :: ~DxCameraRenderer()
{
}

bool DxCameraRenderer :: Select( SceneSPtr CU_PARAM_UNUSED( p_scene ), Camera::eSELECTION_MODE CU_PARAM_UNUSED( p_mode ), int CU_PARAM_UNUSED( x ), int CU_PARAM_UNUSED( y ), stSELECT_RESULT & CU_PARAM_UNUSED( p_found ) )
{
/*
	int l_objectsFound = 0;
	int	l_viewportCoords[4] = {0};
	uint32_t l_selectBuffer[32] = {0};

	glSelectBuffer( 32, l_selectBuffer);
	CheckDxError( cuT( "Dx10CameraRenderer :: Select - glSelectBuffer"));
	glGetIntegerv( GL_VIEWPORT, l_viewportCoords);
	CheckDxError( cuT( "Dx10CameraRenderer :: Select - glGetIntegerv"));
	Pipeline::MatrixMode( Pipeline::eMatrixProjection);
	Pipeline::PushMatrix();

	glRenderMode( GL_SELECT);
	CheckDxError( cuT( "Dx10CameraRenderer :: Select - glRenderMode"));
	Pipeline::LoadIdentity();

	Pipeline::PickMatrix( real( x), real( l_viewportCoords[3] - y), real( 2), real( 2), l_viewportCoords);
	CheckDxError( cuT( "Dx10CameraRenderer :: Select - gluPickMatrix"));
	Pipeline::Perspective( m_target->GetViewport()->GetFOVY(), m_target->GetViewport()->GetRatio(),
		m_target->GetViewport()->GetNearView(), m_target->GetViewport()->GetFarView());
	Pipeline::MatrixMode( Pipeline::eMatrixModelView);
	p_scene->Render( eTriangles, 0.0);

	l_objectsFound = glRenderMode( GL_RENDER);

	Pipeline::MatrixMode( Pipeline::eMatrixProjection);
	Pipeline::PopMatrix();

	Pipeline::MatrixMode( Pipeline::eMatrixModelView);

	if (l_objectsFound > 0)
	{		
		uint32_t l_lowestDepth = l_selectBuffer[1];
		int l_selectedObject = l_selectBuffer[3];

		for (int i = 1; i < l_objectsFound; i++)
		{
			if (l_selectBuffer[(i * 4) + 1] < l_lowestDepth)
			{
				l_lowestDepth = l_selectBuffer[(i * 4) + 1];
				l_selectedObject = l_selectBuffer[(i * 4) + 3];
			}
		}

		return true;
	}
*/
	return false;
}

void DxCameraRenderer :: Render()
{
}

void DxCameraRenderer :: EndRender()
{
}