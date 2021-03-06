#include "MouseEvent.hpp"

#include <Event/Frame/FrameListener.hpp>
#include <Scene/SceneNode.hpp>

#include <Math/Angle.hpp>

using namespace CastorShape;
using namespace Castor3D;
using namespace Castor;

//********************************************************************************************

MouseCameraEvent::MouseCameraEvent( SceneNode * p_node, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ )
	: FrameEvent( EventType::PostRender )
	, m_pNode( p_node )
	, m_rDeltaX( p_rDeltaX )
	, m_rDeltaY( p_rDeltaY )
	, m_rDeltaZ( p_rDeltaZ )
{
}

MouseCameraEvent::~MouseCameraEvent()
{
}

void MouseCameraEvent::Add( FrameEventUPtr && p_pThis, FrameListenerSPtr p_pListener, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ )
{
	MouseCameraEvent & l_pThis = static_cast< MouseCameraEvent & >( *p_pThis );
	bool l_bToAdd = l_pThis.m_rDeltaX == 0 && l_pThis.m_rDeltaY == 0 && l_pThis.m_rDeltaZ == 0;
	l_pThis.m_rDeltaX += p_rDeltaX;
	l_pThis.m_rDeltaY += p_rDeltaY;
	l_pThis.m_rDeltaZ += p_rDeltaZ;

	if ( l_bToAdd )
	{
		p_pListener->PostEvent( std::move( p_pThis ) );
	}
}

//********************************************************************************************

CameraRotateEvent::CameraRotateEvent( SceneNodeSPtr p_node, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ )
	: MouseCameraEvent( p_node.get(), p_rDeltaX, p_rDeltaY, p_rDeltaZ )
{
}

CameraRotateEvent::CameraRotateEvent( SceneNode * p_node, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ )
	: MouseCameraEvent( p_node, p_rDeltaX, p_rDeltaY, p_rDeltaZ )
{
}

CameraRotateEvent::~CameraRotateEvent()
{
}

bool CameraRotateEvent::Apply()
{
	if ( m_pNode )
	{
		m_pNode->Rotate( Quaternion( Angle::from_degrees( m_rDeltaZ ), Angle::from_degrees( m_rDeltaX ), Angle::from_degrees( m_rDeltaY ) ) );
	}

	m_rDeltaX = 0;
	m_rDeltaY = 0;
	m_rDeltaZ = 0;
	return true;
}

//********************************************************************************************

CameraTranslateEvent::CameraTranslateEvent( SceneNodeSPtr p_node, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ )
	:	MouseCameraEvent( p_node.get(), p_rDeltaX, p_rDeltaY, p_rDeltaZ )
{
}

CameraTranslateEvent::CameraTranslateEvent( SceneNode * p_node, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ )
	:	MouseCameraEvent( p_node, p_rDeltaX, p_rDeltaY, p_rDeltaZ )
{
}

CameraTranslateEvent::~CameraTranslateEvent()
{
}

bool CameraTranslateEvent::Apply()
{
	m_pNode->Translate( Point3r( m_rDeltaX, m_rDeltaY, m_rDeltaZ ) );
	m_rDeltaX = 0;
	m_rDeltaY = 0;
	m_rDeltaZ = 0;
	return true;
}

//********************************************************************************************
