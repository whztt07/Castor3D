﻿#include "SceneNode.hpp"
#include "Engine.hpp"
#include "MovableObject.hpp"
#include "Scene.hpp"
#include "Ray.hpp"
#include "Geometry.hpp"
#include "RenderSystem.hpp"
#include "Pipeline.hpp"
#include "Vertex.hpp"
#include "Mesh.hpp"

#include <Logger.hpp>
#include <TransformationMatrix.hpp>

using namespace Castor;

namespace Castor3D
{
	SceneNode::TextLoader::TextLoader( File::eENCODING_MODE p_eEncodingMode )
		:	Loader< SceneNode, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
	{
	}

	bool SceneNode::TextLoader::operator()( SceneNode const & p_node, TextFile & p_file )
	{
		bool l_bReturn = false;
		Logger::LogInfo( cuT( "Writing Node " ) + p_node.GetName() );

		if ( p_node.GetName() != cuT( "RootNode" ) )
		{
			l_bReturn = p_file.WriteText( cuT( "\tscene_node \"" ) + p_node.GetName() + cuT( "\"\n\t{\n" ) ) > 0;

			if ( l_bReturn && p_node.GetParent() )
			{
				l_bReturn = p_file.WriteText( cuT( "\t\tparent \"" ) + p_node.GetParent()->GetName() + cuT( "\"\n" ) ) > 0;
			}

			if ( l_bReturn )
			{
				l_bReturn = p_file.Print( 256, cuT( "\t\torientation " ) ) > 0 && Quaternion::TextLoader()( p_node.GetOrientation(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
			}

			if ( l_bReturn )
			{
				l_bReturn = p_file.Print( 256, cuT( "\t\tposition " ) ) > 0 && Point3r::TextLoader()( p_node.GetPosition(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
			}

			if ( l_bReturn )
			{
				l_bReturn = p_file.Print( 256, cuT( "\t\tscale " ) ) > 0 && Point3r::TextLoader()( p_node.GetScale(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
			}

			if ( l_bReturn )
			{
				l_bReturn = p_file.WriteText( cuT( "\n\t}\n" ) ) > 0;
			}
		}

		if ( l_bReturn )
		{
			Logger::LogInfo( cuT( "Writing Childs" ) );

			for ( SceneNode::node_const_iterator l_it = p_node.ChildsBegin(); l_it != p_node.ChildsEnd() && l_bReturn; ++l_it )
			{
				SceneNodeSPtr l_node = l_it->second.lock();

				if ( l_node )
				{
					l_bReturn = SceneNode::TextLoader()( *l_node, p_file );
				}
			}
		}

		if ( l_bReturn )
		{
			Logger::LogInfo( cuT( "Childs Written" ) );
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	SceneNode::BinaryParser::BinaryParser( Path const & p_path )
		:	Castor3D::BinaryParser< SceneNode >( p_path )
	{
	}

	bool SceneNode::BinaryParser::Fill( SceneNode const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_SCENE_NODE );
		Point3f l_scale;
		Point3f l_position;

		if ( p_obj.GetName() != cuT( "RootNode" ) )
		{
			l_bReturn = DoFillChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, l_chunk );

			if ( l_bReturn )
			{
				l_bReturn = DoFillChunk( p_obj.GetOrientation(), eCHUNK_TYPE_NODE_ORIENTATION, l_chunk );
			}

			if ( l_bReturn )
			{
				l_position = p_obj.GetPosition();
				l_bReturn = DoFillChunk( l_position, eCHUNK_TYPE_NODE_POSITION, l_chunk );
			}

			if ( l_bReturn )
			{
				l_scale = p_obj.GetScale();
				l_bReturn = DoFillChunk( l_scale, eCHUNK_TYPE_NODE_SCALE, l_chunk );
			}
		}

		if ( l_bReturn )
		{
			for ( SceneNode::node_const_iterator l_it = p_obj.ChildsBegin(); l_it != p_obj.ChildsEnd() && l_bReturn; ++l_it )
			{
				SceneNodeSPtr l_node = l_it->second.lock();

				if ( l_node )
				{
					l_bReturn = SceneNode::BinaryParser( m_path ).Fill( *l_node, l_chunk );
				}
			}
		}

		if ( l_bReturn )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_bReturn;
	}

	bool SceneNode::BinaryParser::Parse( SceneNode & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		Quaternion l_orientation;
		Point3f l_scale;
		Point3f l_position;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_bReturn = p_chunk.GetSubChunk( l_chunk );

			if ( l_bReturn )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_NODE_ORIENTATION:
					l_bReturn = DoParseChunk( l_orientation, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetOrientation( l_orientation );
					}

					break;

				case eCHUNK_TYPE_NODE_POSITION:
					l_bReturn = DoParseChunk( l_position, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetPosition( l_position );
					}

					break;

				case eCHUNK_TYPE_NODE_SCALE:
					l_bReturn = DoParseChunk( l_scale, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetScale( l_scale );
					}

					break;

				case eCHUNK_TYPE_SCENE_NODE:
				{
					BinaryChunk l_chunkNode;
					String l_name;
					l_bReturn = l_chunk.GetSubChunk( l_chunkNode );

					if ( l_bReturn )
					{
						switch ( l_chunkNode.GetChunkType() )
						{
						case eCHUNK_TYPE_NAME:
							l_bReturn = DoParseChunk( l_name, l_chunkNode );
							break;

						default:
							l_bReturn = false;
							break;
						}

						if ( l_bReturn )
						{
							SceneNodeSPtr l_node = p_obj.GetScene()->CreateSceneNode( l_name, p_obj.shared_from_this() );
							l_bReturn = SceneNode::BinaryParser( m_path ).Parse( *l_node, l_chunk );
						}
					}
				}
				break;

				default:
					l_bReturn = false;
					break;
				}
			}

			if ( !l_bReturn )
			{
				p_chunk.EndParse();
			}
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	unsigned long long SceneNode::Count = 0;

	SceneNode::SceneNode()
		:	m_bVisible( true )
		,	m_ptScale( 1.0, 1.0, 1.0 )
		,	m_ptPosition( 0.0, 0.0, 0.0 )
		,	m_bDisplayable( false )
		,	m_bMtxChanged( true )
		,	m_bDerivedMtxChanged( true )
	{
		if ( m_strName.empty() )
		{
			m_strName = cuT( "SceneNode_%d" );
			m_strName += str_utils::to_string( Count );
		}

		Count++;
	}

	SceneNode::SceneNode( SceneSPtr p_pScene, String const & p_name )
		:	m_strName( p_name )
		,	m_bVisible( true )
		,	m_ptScale( 1.0, 1.0, 1.0 )
		,	m_ptPosition( 0.0, 0.0, 0.0 )
		,	m_bDisplayable( p_name == cuT( "RootNode" ) )
		,	m_pScene( p_pScene )
		,	m_bMtxChanged( true )
	{
		if ( m_strName.empty() )
		{
			m_strName = cuT( "SceneNode_%d" );
			m_strName += str_utils::to_string( Count );
		}

		Count++;
	}

	SceneNode::~SceneNode()
	{
		SceneNodeSPtr l_parent = GetParent();

		if ( l_parent )
		{
			l_parent->DetachChild( shared_from_this() );
		}

		if ( m_mapChilds.size() > 0 )
		{
			SceneNodePtrStrMap::iterator l_it = m_mapChilds.begin();

			while ( l_it != m_mapChilds.end() )
			{
				SceneNodeSPtr l_node = l_it->second.lock();

				if ( l_node )
				{
					l_node->Detach();
				}

				++l_it;
			}

			m_mapChilds.clear();
		}

		Count--;
	}

	void SceneNode::AttachObject( MovableObjectSPtr p_pObject )
	{
		if ( p_pObject )
		{
			p_pObject->Detach();
			m_mapAttachedObjects[p_pObject->GetName()] = p_pObject;
			p_pObject->AttachTo( shared_from_this() );
		}
	}

	void SceneNode::DetachObject( MovableObjectSPtr p_pObject )
	{
		if ( p_pObject )
		{
			MovableObjectPtrStrMap::iterator l_it = m_mapAttachedObjects.find( p_pObject->GetName() );

			if ( l_it != m_mapAttachedObjects.end() )
			{
				m_mapAttachedObjects.erase( l_it );
				p_pObject->AttachTo( nullptr );
			}
		}
	}

	void SceneNode::AttachTo( SceneNodeSPtr p_parent )
	{
		SceneNodeSPtr l_parent = GetParent();

		if ( l_parent )
		{
			l_parent->DetachChild( shared_from_this() );
			l_parent.reset();
		}

		m_pParent = p_parent;

		if ( p_parent )
		{
			m_bDisplayable = p_parent->m_bDisplayable;
			p_parent->AddChild( shared_from_this() );
			m_bMtxChanged = true;
		}
	}

	void SceneNode::Detach()
	{
		SceneNodeSPtr l_parent = GetParent();

		if ( l_parent )
		{
			m_bDisplayable = false;
			m_pParent.reset();
			l_parent->DetachChild( shared_from_this() );
			m_bMtxChanged = true;
		}
	}

	bool SceneNode::HasChild( String const & p_name )
	{
		bool l_bFound = false;

		if ( m_mapChilds.find( p_name ) == m_mapChilds.end() )
		{
			SceneNodePtrStrMap::iterator l_it = m_mapChilds.begin();

			while ( l_it != m_mapChilds.end() && ! l_bFound )
			{
				SceneNodeSPtr l_node = l_it->second.lock();

				if ( l_node && l_node->HasChild( p_name ) )
				{
					l_bFound = true;
				}

				++l_it;
			}
		}

		return l_bFound;
	}

	void SceneNode::AddChild( SceneNodeSPtr p_child )
	{
		String l_name = p_child->GetName();

		if ( m_mapChilds.find( l_name ) == m_mapChilds.end() )
		{
			m_mapChilds.insert( std::make_pair( l_name, p_child ) );
		}
		else
		{
			//Logger::LogInfo( m_strName + cuT( " - Can't add SceneNode ") + l_name + cuT( " - Already in childs"));
		}
	}

	void SceneNode::DetachChild( SceneNodeSPtr p_child )
	{

		if ( p_child )
		{
			DetachChild( p_child->GetName() );
		}
		else
		{
			Logger::LogInfo( m_strName + cuT( " - Can't remove SceneNode - Null pointer given"));
		}
	}

	void SceneNode::DetachChild( String const & p_childName )
	{
		SceneNodePtrStrMap::iterator l_it = m_mapChilds.find( p_childName );

		if ( l_it != m_mapChilds.end() )
		{
			SceneNodeSPtr l_current = l_it->second.lock();
			m_mapChilds.erase( l_it );

			if ( l_current )
			{
				l_current->Detach();
			}
		}
		else
		{
			Logger::LogInfo( m_strName + cuT( " - Can't remove SceneNode " ) + p_childName + cuT( " - Not in childs" ) );
		}
	}

	void SceneNode::DetachAllChilds()
	{
		SceneNodePtrStrMap::iterator l_it = m_mapChilds.begin();

		while ( l_it != m_mapChilds.end() )
		{
			SceneNodeSPtr l_current = l_it->second.lock();
			m_mapChilds.erase( l_it );

			if ( l_current )
			{
				l_current->Detach();
			}

			l_it = m_mapChilds.begin();
		}

		m_mapChilds.clear();
	}

	void SceneNode::Yaw( Angle const & p_angle )
	{
		Rotate( Quaternion( Point3r( 0.0, 1.0, 0.0 ), p_angle ) );
	}

	void SceneNode::Pitch( Angle const & p_angle )
	{
		Rotate( Quaternion( Point3r( 1.0, 0.0, 0.0 ), p_angle ) );
	}

	void SceneNode::Roll( Angle const & p_angle )
	{
		Rotate( Quaternion( Point3r( 0.0, 0.0, 1.0 ), p_angle ) );
	}

	void SceneNode::Rotate( Quaternion const & p_orientation )
	{
		SetOrientation( m_qOrientation * p_orientation );
	}

	void SceneNode::Translate( Point3r const & p_position )
	{
		SetPosition( m_ptPosition + p_position );
	}

	void SceneNode::Scale( Point3r const & p_scale )
	{
		SetScale( m_ptScale * p_scale );
	}

	void SceneNode::SetOrientation( Quaternion const & p_orientation )
	{
		m_qOrientation = p_orientation;
		DoUpdateChildsDerivedTransform();
		m_bMtxChanged = true;
	}

	void SceneNode::SetPosition( Point3r const & p_position )
	{
		m_ptPosition = p_position;
		DoUpdateChildsDerivedTransform();
		m_bMtxChanged = true;
	}

	void SceneNode::SetScale( Point3r const & p_scale )
	{
		m_ptScale = p_scale;
		DoUpdateChildsDerivedTransform();
		m_bMtxChanged = true;
	}

	void SceneNode::CreateBuffers( uint32_t & p_nbFaces, uint32_t & p_nbVertex )const
	{
		for ( MovableObjectPtrStrMap::const_iterator l_it = m_mapAttachedObjects.begin(); l_it != m_mapAttachedObjects.end(); ++l_it )
		{
			MovableObjectSPtr l_current = l_it->second.lock();

			if ( l_current && l_current->GetType() == eMOVABLE_TYPE_GEOMETRY )
			{
				std::static_pointer_cast< Geometry >( l_current )->CreateBuffers( p_nbFaces, p_nbVertex );
			}
		}

		for ( SceneNodePtrStrMap::const_iterator l_it = m_mapChilds.begin(); l_it != m_mapChilds.end(); ++l_it )
		{
			SceneNodeSPtr l_current = l_it->second.lock();

			if ( l_current )
			{
				l_current->CreateBuffers( p_nbFaces, p_nbVertex );
			}
		}
	}

	GeometrySPtr SceneNode::GetNearestGeometry( Ray * p_pRay, real & p_fDistance, FaceSPtr * p_ppFace, SubmeshSPtr * p_ppSubmesh )
	{
		GeometrySPtr l_pReturn = nullptr;
		real l_fDistance;

		for ( MovableObjectPtrStrMap::iterator l_it = m_mapAttachedObjects.begin(); l_it != m_mapAttachedObjects.end(); ++l_it )
		{
			MovableObjectSPtr l_current = l_it->second.lock();

			if ( l_current && l_current->GetType() == eMOVABLE_TYPE_GEOMETRY )
			{
				if ( ( l_fDistance = p_pRay->Intersects( std::static_pointer_cast< Geometry >( l_current ), p_ppFace, p_ppSubmesh ) ) >= 0.0 && l_fDistance < p_fDistance )
				{
					p_fDistance = l_fDistance;
					l_pReturn = std::static_pointer_cast< Geometry >( l_it->second.lock() );
				}
			}
		}

		GeometrySPtr l_pTmp;

		for ( SceneNodePtrStrMap::iterator l_it = m_mapChilds.begin(); l_it != m_mapChilds.end(); ++l_it )
		{
			SceneNodeSPtr l_current = l_it->second.lock();

			if ( l_current )
			{
				l_pTmp = l_current->GetNearestGeometry( p_pRay, p_fDistance, p_ppFace, p_ppSubmesh );

				if ( l_pTmp )
				{
					l_pReturn = l_pTmp;
				}
			}
		}

		return l_pReturn;
	}

	Point3r SceneNode::GetDerivedPosition()
	{
		Point3r l_ptReturn;
		MtxUtils::get_translate( GetDerivedTransformationMatrix(), l_ptReturn );
		return l_ptReturn;
	}

	Quaternion SceneNode::GetDerivedOrientation()
	{
		Quaternion l_qReturn;
		MtxUtils::get_rotate( GetDerivedTransformationMatrix(), l_qReturn );
		return l_qReturn;
	}

	Point3r SceneNode::GetDerivedScale()
	{
		Point3r l_ptReturn;
		MtxUtils::get_scale( GetDerivedTransformationMatrix(), l_ptReturn );
		return l_ptReturn;
	}

	void SceneNode::DoComputeMatrix()
	{
		Matrix4x4r l_mtxScale, l_mtxRotate, l_mtxTranslate;

		if ( m_bMtxChanged )
		{
			m_bDerivedMtxChanged = true;
			MtxUtils::set_transform_rh( m_mtxMatrix, m_ptPosition, m_ptScale, m_qOrientation );
			m_bMtxChanged = false;
		}

		if ( m_bDerivedMtxChanged )
		{
			SceneNodeSPtr l_parent = GetParent();

			if ( l_parent )
			{
				m_mtxDerivedMatrix = l_parent->GetDerivedTransformationMatrix() * m_mtxMatrix;
			}
			else
			{
				m_mtxDerivedMatrix = m_mtxMatrix;
			}

			m_bDerivedMtxChanged = false;
		}
	}

	void SceneNode::DoUpdateChildsDerivedTransform()
	{
		for ( node_iterator l_it = m_mapChilds.begin(); l_it != m_mapChilds.end(); ++l_it )
		{
			SceneNodeSPtr l_current = l_it->second.lock();

			if ( l_current )
			{
				l_current->DoUpdateChildsDerivedTransform();
				l_current->m_bDerivedMtxChanged = true;
			}
		}
	}
}