﻿/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ColourBlendState_HPP___
#define ___Renderer_ColourBlendState_HPP___
#pragma once

#include "Pipeline/ColourBlendStateAttachment.hpp"

namespace renderer
{
	/**
	*\brief
	*	Etat de mélange.
	*/
	class ColourBlendState
	{
	public:
		ColourBlendState( bool logicOpEnable = false
			, LogicOp logicOp = LogicOp::eCopy
			, Vec4 const & blendConstants = Vec4{ 0.0f, 0.0f, 0.0f, 0.0f } );
		/**
		*\return
		*	Un ColourBlendState avec une attache couleur par défaut.
		*/
		static ColourBlendState const & createDefault();
		/**
		*\brief
		*	Ajoute une attache à l'état.
		*\param[in] attachment
		*	L'attache.
		*/
		void addAttachment( ColourBlendStateAttachment const & attachment );
		/**
		*\return
		*	Le statut d'activation de l'opérateur logique.
		*/
		inline bool isLogicOpEnabled()const
		{
			return m_logicOpEnable;
		}
		/**
		*\return
		*	L'opérateur logique.
		*/
		inline LogicOp getLogicOp()const
		{
			return m_logicOp;
		}
		/**
		*\return
		*	Les constantes de mélange.
		*/
		inline Vec4 const & getBlendConstants()const
		{
			return m_blendConstants;
		}
		/**
		*\return
		*	Le début du tableau d'attaches.
		*/
		inline ColourBlendStateAttachmentArray::const_iterator begin()const
		{
			return m_attachs.begin();
		}
		/**
		*\return
		*	La fin du tableau d'attaches.
		*/
		inline ColourBlendStateAttachmentArray::const_iterator end()const
		{
			return m_attachs.end();
		}

	private:
		bool const m_logicOpEnable;
		LogicOp const m_logicOp;
		Vec4 const m_blendConstants;
		ColourBlendStateAttachmentArray m_attachs;
	};
}

#endif