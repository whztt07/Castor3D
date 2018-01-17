/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_CommandPoolCreateFlag_HPP___
#define ___Renderer_CommandPoolCreateFlag_HPP___
#pragma once

#include <Design/FlagCombination.hpp>

namespace renderer
{
	/**
	*\brief
	*	Enumération des utilisation de tampon de commandes.
	*/
	enum class CommandPoolCreateFlag
		: uint32_t
	{
		eTransient = 0x00000001,
		eResetCommandBuffer = 0x00000002,
	};
	IMPLEMENT_FLAGS( CommandPoolCreateFlag )
}

#endif