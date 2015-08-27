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
#ifndef ___CASTOR_UNICITY_EXCEPTION___
#define ___CASTOR_UNICITY_EXCEPTION___

#include "Exception.hpp"
#include "Resource.hpp"
#include "TextFile.hpp"
#include "BinaryFile.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Unicity exception
	\~french
	\brief		Exception d'unicité
	*/
	class UnicityException
		: public Castor::Exception
	{
	public:
		UnicityException( std::string const & p_description, char const * p_file, char const * p_function, uint32_t p_line )
			: Exception( p_description, p_file, p_function, p_line )
		{
		}
	};
	//!\~english Helper macro to use UnicityException	\~french Macro pour faciliter l'utilisation de UnicityException
#	define UNICITY_ERROR( p_text ) throw UnicityException( std::string( "Duplicate instance for Unique class " ) + p_text, __FILE__, __FUNCTION__, __LINE__)
}

#endif