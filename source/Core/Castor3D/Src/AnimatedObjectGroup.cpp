﻿#include "AnimatedObjectGroup.hpp"
#include "AnimatedObject.hpp"
#include "Animation.hpp"
#include "MovableObject.hpp"
#include "Mesh.hpp"
#include "Skeleton.hpp"

#include <Logger.hpp>

using namespace Castor;

#ifdef GetObject
#	undef GetObject
#endif

namespace Castor3D
{
	AnimatedObjectGroup::BinaryLoader::BinaryLoader()
		:	Loader< AnimatedObjectGroup, eFILE_TYPE_BINARY, BinaryFile >( File::eOPEN_MODE_DUMMY )
	{
	}

	bool AnimatedObjectGroup::BinaryLoader::operator()( AnimatedObjectGroup & p_group, BinaryFile & p_file, Scene * p_scene )
	{
		m_pScene = p_scene;
		return operator()( p_group, p_file );
	}

	bool AnimatedObjectGroup::BinaryLoader::operator()( AnimatedObjectGroup & p_group, BinaryFile & p_file )
	{
		Collection< AnimatedObjectGroup, String > l_collection;
		bool l_bResult = false;

		if ( ! l_bResult || l_collection.has( p_group.GetName() ) )
		{
			Logger::LogInfo( cuT( "Can't add AnimatedObjectGroup [" ) + p_group.GetName() + cuT( "]" ) );
		}
		else
		{
			Logger::LogInfo( cuT( "AnimatedObjectGroup [" ) + p_group.GetName() +  + cuT( "] added" ) );
		}

		return l_bResult;
	}

	//*************************************************************************************************

	AnimatedObjectGroup::TextLoader::TextLoader( File::eENCODING_MODE p_encodingMode )
		:	Loader< AnimatedObjectGroup, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_encodingMode )
	{
	}

	bool AnimatedObjectGroup::TextLoader::operator()( AnimatedObjectGroup const & p_group, TextFile & p_file )
	{
		bool l_return = p_file.WriteText( cuT( "animated_object_group " ) + p_group.GetName() + cuT( "\n{\n" ) ) > 0;

		for ( StrSet::const_iterator l_it = p_group.AnimationsBegin(); l_it != p_group.AnimationsEnd() && l_return; ++l_it )
		{
			l_return = p_file.WriteText( cuT( "\tanimation " ) + *l_it + cuT( "\n" ) ) > 0;
		}

		for ( AnimatedObjectPtrStrMap::const_iterator l_it = p_group.ObjectsBegin(); l_it != p_group.ObjectsEnd() && l_return; ++l_it )
		{
			l_return = p_file.WriteText( cuT( "\tanimated_object " ) + l_it->first + cuT( "\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "}\n\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	AnimatedObjectGroup::AnimatedObjectGroup()
		: Named( cuT( "" ) )
	{
		m_timer.TimeS();
	}

	AnimatedObjectGroup::AnimatedObjectGroup( AnimatedObjectGroup const & p_src )
		: Named( p_src.GetName() )
		, m_pScene( p_src.m_pScene )
		, m_setAnimations( p_src.m_setAnimations )
		, m_mapObjects( p_src.m_mapObjects )
	{
		m_timer.TimeS();
	}

	AnimatedObjectGroup::AnimatedObjectGroup( SceneSPtr p_scene, String const & p_name )
		: Named( p_name )
		, m_pScene( p_scene )
	{
		m_timer.TimeS();
	}

	AnimatedObjectGroup::~AnimatedObjectGroup()
	{
		m_mapObjects.clear();
		m_setAnimations.clear();
	}

	AnimatedObjectSPtr AnimatedObjectGroup::CreateObject( String const & p_name )
	{
		AnimatedObjectSPtr l_return;

		if ( m_mapObjects.find( p_name ) == m_mapObjects.end() )
		{
			l_return = std::make_shared< AnimatedObject >( p_name );
			m_mapObjects.insert( std::make_pair( p_name, l_return ) );
		}

		return l_return;
	}

	bool AnimatedObjectGroup::AddObject( AnimatedObjectSPtr p_object )
	{
		bool l_return = p_object && m_mapObjects.find( p_object->GetName() ) == m_mapObjects.end();

		if ( l_return )
		{
			m_mapObjects.insert( std::make_pair( p_object->GetName(), p_object ) );
		}

		return l_return;
	}

	void AnimatedObjectGroup::AddAnimation( String const & p_name )
	{
		if ( m_setAnimations.find( p_name ) == m_setAnimations.end() )
		{
			m_setAnimations.insert( p_name );
		}
	}

	void AnimatedObjectGroup::Update()
	{
		real l_rTslf = real( m_timer.TimeS() );
		std::for_each( m_mapObjects.begin(), m_mapObjects.end(), [&]( std::pair< String, AnimatedObjectSPtr > p_pair )
		{
			p_pair.second->Update( l_rTslf );
		} );
	}

	void AnimatedObjectGroup::SetAnimationLooped( Castor::String const & p_name, bool p_bLooped )
	{
		if ( m_setAnimations.find( p_name ) != m_setAnimations.end() )
		{
			std::for_each( m_mapObjects.begin(), m_mapObjects.end(), [&]( std::pair< String, AnimatedObjectSPtr > p_pair )
			{
				AnimationSPtr l_pAnim = p_pair.second->GetAnimation( p_name );

				if ( l_pAnim )
				{
					l_pAnim->SetLooped( p_bLooped );
				}
			} );
		}
	}

	void AnimatedObjectGroup::StartAnimation( String const & p_name )
	{
		StrSet::iterator l_it = m_setAnimations.find( p_name );

		if ( l_it != m_setAnimations.end() )
		{
			std::for_each( m_mapObjects.begin(), m_mapObjects.end(), [&]( std::pair< String, AnimatedObjectSPtr > p_pair )
			{
				p_pair.second->StartAnimation( p_name );
			} );
		}
	}

	void AnimatedObjectGroup::StopAnimation( String const & p_name )
	{
		StrSet::iterator l_it = m_setAnimations.find( p_name );

		if ( l_it != m_setAnimations.end() )
		{
			std::for_each( m_mapObjects.begin(), m_mapObjects.end(), [&]( std::pair< String, AnimatedObjectSPtr > p_pair )
			{
				p_pair.second->StopAnimation( p_name );
			} );
		}
	}

	void AnimatedObjectGroup::PauseAnimation( String const & p_name )
	{
		StrSet::iterator l_it = m_setAnimations.find( p_name );

		if ( l_it != m_setAnimations.end() )
		{
			std::for_each( m_mapObjects.begin(), m_mapObjects.end(), [&]( std::pair< String, AnimatedObjectSPtr > p_pair )
			{
				p_pair.second->PauseAnimation( p_name );
			} );
		}
	}

	void AnimatedObjectGroup::StartAllAnimations()
	{
		std::for_each( m_mapObjects.begin(), m_mapObjects.end(), [&]( std::pair< String, AnimatedObjectSPtr > p_pair )
		{
			p_pair.second->StartAllAnimations();
		} );
	}

	void AnimatedObjectGroup::StopAllAnimations()
	{
		std::for_each( m_mapObjects.begin(), m_mapObjects.end(), [&]( std::pair< String, AnimatedObjectSPtr > p_pair )
		{
			p_pair.second->StopAllAnimations();
		} );
	}

	void AnimatedObjectGroup::PauseAllAnimations()
	{
		std::for_each( m_mapObjects.begin(), m_mapObjects.end(), [&]( std::pair< String, AnimatedObjectSPtr > p_pair )
		{
			p_pair.second->PauseAllAnimations();
		} );
	}
}
