﻿/*
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
#ifndef ___CU_FunctionTracker___
#define ___CU_FunctionTracker___

#include "PreciseTimer.hpp"
#include "NonCopyable.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Helper class, used to track entering and leaving a block
	\remark		Call the macro CASTOR_TRACK() at the beginning of a block to have a console output when entering or leaving that block
	\~french
	\brief		Classe permettant de tracer les entrées et sorties de bloc
	\remark		Appelez la macro CASTOR_TRACK() au début d'un bloc pour avoir une entrée dans la console en entrée et en sortie du bloc
	*/
	class BlockTracker : CuNonCopyable
	{
	private:
		String			m_strFile;
		String			m_strFunction;
		uint32_t const	m_uiLine;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_szFunction	Caller function
		 *\param[in]	p_szFile		Function file
		 *\param[in]	p_uiLine		Function line
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_szFunction	La fonction appelante
		 *\param[in]	p_szFile		Le fichier où se trouve la fonction
		 *\param[in]	p_uiLine		La ligne dans la fonction
		 */
		BlockTracker( char const * p_szFunction, char const * p_szFile, uint32_t p_uiLine);
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~BlockTracker();
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.5.0
	\date		24/04/2012
	\~english
	\brief		Helper class, used to time a block's execution time
	\remark		Call the macro CASTOR_TRACK() at the beginning of a block to have a console output when leaving that block
	\~french
	\brief		Classe permettant de mesurer le temps d'exécution d'un bloc
	\remark		Appelez la macro CASTOR_TRACK() au début d'un bloc pour avoir une entrée dans la console en sortie du bloc
	*/
	class BlockTimer : CuNonCopyable
	{
	private:
		String			m_strFile;
		String			m_strFunction;
		uint32_t const	m_uiLine;
		PreciseTimer	m_timer;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_szFunction	Caller function
		 *\param[in]	p_szFile		Function file
		 *\param[in]	p_uiLine		Function line
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_szFunction	La fonction appelante
		 *\param[in]	p_szFile		Le fichier où se trouve la fonction
		 *\param[in]	p_uiLine		La ligne dans la fonction
		 */
		BlockTimer( char const * p_szFunction, char const * p_szFile, uint32_t p_uiLine);
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~BlockTimer();
	};
}

#	define CASTOR_TRACK( fn )	Castor::BlockTracker l_tracker##__LINE__( __FUNCTION__, __FILE__, __LINE__ )
#	define CASTOR_TIME()		Castor::BlockTimer	 l_timer##__LINE__( __FUNCTION__, __FILE__, __LINE__ )

#endif
