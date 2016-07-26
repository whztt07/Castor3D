#include "Camera.hpp"

#include "Engine.hpp"

#include "Scene.hpp"
#include "SceneNode.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/Viewport.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/PointFrameVariable.hpp"
#include "Shader/ShaderProgram.hpp"

#include <Graphics/CubeBox.hpp>
#include <Math/TransformationMatrix.hpp>

using namespace Castor;

namespace Castor3D
{
	Camera::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< Camera >{ p_tabs }
	{
	}

	bool Camera::TextWriter::operator()( Camera const & p_camera, TextFile & p_file )
	{
		Logger::LogInfo( m_tabs + cuT( "Writing Camera " ) + p_camera.GetName() );
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "camera \"" ) + p_camera.GetName() + cuT( "\"\n" ) ) > 0
			&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		Castor::TextWriter< Camera >::CheckError( l_return, "Camera name" );

		if ( l_return )
		{
			l_return = MovableObject::TextWriter{ m_tabs }( p_camera, p_file );
		}

		if ( l_return )
		{
			l_return = Viewport::TextWriter( m_tabs + cuT( "\t" ) )( p_camera.GetViewport(), p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	namespace
	{
		Point3r GetVertexP( Point3r const & p_min, Point3r const & p_max, Point3r const & p_normal )
		{
			Point3r l_return{ p_min };

			if ( p_normal[0] >= 0 )
			{
				l_return[0] = p_max[0];
			}

			if ( p_normal[1] >= 0 )
			{
				l_return[1] = p_max[1];
			}

			if ( p_normal[2] >= 0 )
			{
				l_return[2] = p_max[2];
			}

			return l_return;
		}

		Point3r GetVertexN( Point3r const & p_min, Point3r const & p_max, Point3r const & p_normal )
		{
			Point3r l_return{ p_max };

			if ( p_normal[0] >= 0 )
			{
				l_return[0] = p_min[0];
			}

			if ( p_normal[1] >= 0 )
			{
				l_return[1] = p_min[1];
			}

			if ( p_normal[2] >= 0 )
			{
				l_return[2] = p_min[2];
			}

			return l_return;
		}
	}

	//*************************************************************************************************

	Camera::Camera( String const & p_name, Scene & p_scene, SceneNodeSPtr p_node, Viewport const & p_viewport )
		: MovableObject{ p_name, p_scene, eMOVABLE_TYPE_CAMERA, p_node }
		, m_viewport{ p_viewport }
	{
		p_scene.GetEngine()->PostEvent( MakeInitialiseEvent( m_viewport ) );
	}

	Camera::Camera( String const & p_name, Scene & p_scene, SceneNodeSPtr p_node )
		: Camera{ p_name, p_scene, p_node, Viewport{ *p_scene.GetEngine() } }
	{
	}

	Camera::~Camera()
	{
	}

	void Camera::ResetOrientation()
	{
		SceneNodeSPtr l_node = GetParent();

		if ( l_node )
		{
			l_node->SetOrientation( Quaternion::identity() );
		}
	}

	void Camera::ResetPosition()
	{
		SceneNodeSPtr l_node = GetParent();

		if ( l_node )
		{
			l_node->SetPosition( Point3r( 0, 0, 0 ) );
		}
	}

	void Camera::Render( Pipeline & p_pipeline )
	{
		bool l_modified = m_viewport.Render( p_pipeline );
		SceneNodeSPtr l_node = GetParent();

		if ( l_node )
		{
			if ( l_modified || l_node->IsModified() )
			{
				auto const & l_position = l_node->GetDerivedPosition();
				auto const & l_orientation = l_node->GetDerivedOrientation();
				Point3r l_right, l_up, l_lookat;
				l_orientation.to_axes( l_right, l_up, l_lookat );
				point::normalise( l_up );
				point::normalise( l_right );
				point::normalise( l_lookat );

				// Update frustum
				// Retrieve near and far planes' dimensions
				real l_farHeight{ 0.0_r };
				real l_farWidth{ 0.0_r };
				real l_nearHeight{ 0.0_r };
				real l_nearWidth{ 0.0_r };

				if ( m_viewport.GetType() == eVIEWPORT_TYPE_ORTHO )
				{
					l_nearHeight = m_viewport.GetBottom() - m_viewport.GetTop();
					l_nearWidth = m_viewport.GetRight() - m_viewport.GetLeft();
					l_farHeight = l_nearHeight;
					l_farWidth = l_nearWidth;
				}
				else
				{
					real l_tan = real( tan( m_viewport.GetFovY().radians() / 2 ) );
					l_nearHeight = 2.0_r * l_tan * m_viewport.GetNear();
					l_nearWidth = l_nearHeight * m_viewport.GetRatio();
					l_farHeight = 2.0_r * l_tan * m_viewport.GetFar();
					l_farWidth = l_farHeight * m_viewport.GetRatio();
				}

				// Compute planes' points
				// N => Near, F => Far, C => Center, T => Top, L => Left, R => Right, B => Bottom
				Point3r l_nc{ l_position + l_lookat * m_viewport.GetNear() };
				Point3r l_ntl{ l_nc + ( l_up * l_nearHeight / 2 ) - ( l_right * l_nearWidth / 2 ) };
				Point3r l_nbl{ l_nc - ( l_up * l_nearHeight / 2 ) - ( l_right * l_nearWidth / 2 ) };
				Point3r l_ntr{ l_nc + ( l_up * l_nearHeight / 2 ) + ( l_right * l_nearWidth / 2 ) };
				Point3r l_nbr{ l_nc - ( l_up * l_nearHeight / 2 ) + ( l_right * l_nearWidth / 2 ) };
				Point3r l_fc{ l_position + l_lookat * m_viewport.GetFar() };
				Point3r l_ftl{ l_fc + ( l_up * l_farHeight / 2 ) - ( l_right * l_farWidth / 2 ) };
				Point3r l_fbl{ l_fc - ( l_up * l_farHeight / 2 ) - ( l_right * l_farWidth / 2 ) };
				Point3r l_ftr{ l_fc + ( l_up * l_farHeight / 2 ) + ( l_right * l_farWidth / 2 ) };
				Point3r l_fbr{ l_fc - ( l_up * l_farHeight / 2 ) + ( l_right * l_farWidth / 2 ) };

				// Fill planes
				m_planes[size_t( FrustumPlane::Near )].Set( l_nbl, l_ntl, l_ntr );
				m_planes[size_t( FrustumPlane::Far )].Set( l_fbr, l_ftr, l_ftl );
				m_planes[size_t( FrustumPlane::Left )].Set( l_fbl, l_ftl, l_ntl );
				m_planes[size_t( FrustumPlane::Right )].Set( l_nbr, l_ntr, l_ftr );
				m_planes[size_t( FrustumPlane::Top )].Set( l_ntl, l_ftl, l_ftr );
				m_planes[size_t( FrustumPlane::Bottom )].Set( l_nbr, l_fbr, l_fbl );

				// Update view matrix
				// Rotation
				Matrix4x4r l_rotate { 1.0_r };
				auto & l_col0 = l_rotate[0];
				auto & l_col1 = l_rotate[1];
				auto & l_col2 = l_rotate[2];
				l_col0[0] = l_right[0];
				l_col0[1] = l_up[0];
				l_col0[2] = l_lookat[0];
				l_col0[3] = 0.0_r;
				l_col1[0] = l_right[1];
				l_col1[1] = l_up[1];
				l_col1[2] = l_lookat[1];
				l_col1[3] = 0.0_r;
				l_col2[0] = l_right[2];
				l_col2[1] = l_up[2];
				l_col2[2] = l_lookat[2];
				l_col2[3] = 0.0_r;

				// Translation
				Matrix4x4r l_translate { 1.0_r };
				auto & l_col3 = l_translate[3];
				l_col3[0] = l_position[0];
				l_col3[1] = l_position[1];
				l_col3[2] = l_position[2];
				l_col3[3] = 1.0_r;

				m_view = l_rotate * l_translate;
			}

			p_pipeline.SetViewMatrix( m_view );
		}

		GetScene()->GetEngine()->GetRenderSystem()->SetCurrentCamera( this );
	}

	void Camera::EndRender()
	{
		GetScene()->GetEngine()->GetRenderSystem()->SetCurrentCamera( nullptr );
	}

	void Camera::Resize( uint32_t p_width, uint32_t p_height )
	{
		Resize( Size( p_width, p_height ) );
	}

	void Camera::Resize( Size const & p_size )
	{
		m_viewport.Resize( p_size );
	}

	eVIEWPORT_TYPE Camera::GetViewportType()const
	{
		return m_viewport.GetType();
	}

	uint32_t Camera::GetWidth()const
	{
		return m_viewport.GetWidth();
	}

	uint32_t Camera::GetHeight()const
	{
		return m_viewport.GetHeight();
	}

	void Camera::SetViewportType( eVIEWPORT_TYPE val )
	{
		m_viewport.UpdateType( val );
	}

	bool Camera::IsVisible( CubeBox const & p_box, Matrix4x4r const & p_transformations )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		Point3r l_corners[8];
		l_corners[0] = p_box.GetMin();
		l_corners[1] = p_box.GetMax();

		// Express object box in world coordinates
		l_corners[2] = p_transformations * Point3r{ l_corners[0][0], l_corners[1][1], l_corners[0][2] };
		l_corners[3] = p_transformations * Point3r{ l_corners[1][0], l_corners[1][1], l_corners[0][2] };
		l_corners[4] = p_transformations * Point3r{ l_corners[1][0], l_corners[0][1], l_corners[0][2] };
		l_corners[5] = p_transformations * Point3r{ l_corners[0][0], l_corners[1][1], l_corners[1][2] };
		l_corners[6] = p_transformations * Point3r{ l_corners[0][0], l_corners[0][1], l_corners[1][2] };
		l_corners[7] = p_transformations * Point3r{ l_corners[1][0], l_corners[0][1], l_corners[1][2] };
		l_corners[0] = p_transformations * l_corners[0];
		l_corners[1] = p_transformations * l_corners[1];

		// Retrieve axis aligned box boundaries
		Point3r l_min( l_corners[0] );
		Point3r l_max( l_corners[1] );

		for( int j = 0; j < 8; ++j )
		{
			l_min[0] = std::min( l_corners[j][0], l_min[0] );
			l_min[1] = std::min( l_corners[j][1], l_min[1] );
			l_min[2] = std::min( l_corners[j][2], l_min[2] );

			l_max[0] = std::max( l_corners[j][0], l_max[0] );
			l_max[1] = std::max( l_corners[j][1], l_max[1] );
			l_max[2] = std::max( l_corners[j][2], l_max[2] );
		}

		Intersection l_return{ Intersection::In };

		for ( size_t i{ 0u }; i < size_t( FrustumPlane::Count ) && l_return != Intersection::Out; ++i )
		{
			if ( m_planes[i].Distance( GetVertexP( l_min, l_max, m_planes[i].GetNormal() ) ) < 0 )
			{
				// The positive vertex outside?
				l_return = Intersection::Out;
			}
			else if ( m_planes[i].Distance( GetVertexN( l_min, l_max, m_planes[i].GetNormal() ) ) < 0 )
			{
				// The negative vertex outside?
				l_return = Intersection::Intersect;
			}
		}

		return true;// l_return != Intersection::Out;
	}

	bool Camera::IsVisible( Castor::SphereBox const & p_box, Castor::Matrix4x4r const & m_transformations )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		Intersection l_return{ Intersection::In };

		for ( size_t i{ 0u }; i < size_t( FrustumPlane::Count ) && l_return != Intersection::Out; ++i )
		{
			float l_distance = m_planes[i].Distance( p_box.GetCenter() + Point3r{ m_transformations[3][0], m_transformations[3][1], m_transformations[3][2] } );

			if ( l_distance < -p_box.GetRadius() )
			{
				l_return = Intersection::Out;
			}
			else if ( l_distance < p_box.GetRadius() )
			{
				l_return = Intersection::Intersect;
			}
		}

		return true;//l_return != Intersection::Out;
	}

	bool Camera::IsVisible( Point3r const & p_point )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		auto l_it = std::find_if( m_planes.begin(), m_planes.end(), [&p_point]( auto const & p_plane )
		{
			return p_plane.Distance( p_point ) < 0;
		} );

		return true;//l_it == m_planes.end();
	}

	void Camera::FillShader( FrameVariableBuffer const & p_sceneBuffer )const
	{
		Point3r l_position = GetParent()->GetDerivedPosition();
		Point3rFrameVariableSPtr l_cameraPos;
		p_sceneBuffer.GetVariable( ShaderProgram::CameraPos, l_cameraPos );

		if ( l_cameraPos )
		{
			l_cameraPos->SetValue( l_position );
		}
	}
}
