﻿/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___CASTOR_LOADER_H___
#define ___CASTOR_LOADER_H___

#include "Exception.hpp"
#include "Resource.hpp"
#include "File.hpp"
#include "LoaderException.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		24/05/2016
	\~english
	\brief		Helper structure to find File type (BinaryFile or TextFile) from eFILE_TYPE.
	\~french
	\brief		Structure d'aide pour trouver le type de File (BinaryFile ou TextFile) à partir d'un eFILE_TYPE.
	*/
	template< eFILE_TYPE FT >
	struct LoaderFileTyper;
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		24/05/2016
	\~english
	\brief		Helper structure to find File type (BinaryFile or TextFile) from eFILE_TYPE.
	\remarks	Specialisation for eFILE_TYPE_TEXT.
	\~french
	\brief		Structure d'aide pour trouver le type de File (BinaryFile ou TextFile) à partir d'un eFILE_TYPE.
	\remarks	Spécialisation pour eFILE_TYPE_TEXT.
	*/
	template<>
	struct LoaderFileTyper< eFILE_TYPE_TEXT >
	{
		using FileType = TextFile;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		24/05/2016
	\~english
	\brief		Helper structure to find File type (BinaryFile or TextFile) from eFILE_TYPE.
	\remarks	Specialisation for eFILE_TYPE_BINARY.
	\~french
	\brief		Structure d'aide pour trouver le type de File (BinaryFile ou TextFile) à partir d'un eFILE_TYPE.
	\remarks	Spécialisation pour eFILE_TYPE_BINARY.
	*/
	template<>
	struct LoaderFileTyper< eFILE_TYPE_BINARY >
	{
		using FileType = BinaryFile;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Main resource loader class.
	\remarks	Holds the two functions needed for a resource loader : Load and Save.
	\~french
	\brief		Classe de base pour les loaders de ressource.
	\remarks	Contient les 2 fonctions nécessaire a un loader : Load et Save.
	*/
	template< class T, eFILE_TYPE FT >
	class Loader
		: public Castor::NonCopyable
	{
	protected:
		using FileType = typename LoaderFileTyper< FT >::FileType;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Loader() = default;
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~Loader() = default;
		/**
		 *\~english
		 *\brief			Reads a resource from a file.
		 *\param[in,out]	p_object	The object to read.
		 *\param[in,out]	p_file		The file where to read the object.
		 *\~french
		 *\brief			Lit une ressource à partir d'un fichier.
		 *\param[in,out]	p_object	L'objet à lire.
		 *\param[in,out]	p_file		Le fichier où lire l'objet.
		 */
		virtual bool operator()( T & p_object, FileType & p_file )
		{
			LOADER_ERROR( "Import not supported by the loader registered for this type" );
		}
		/**
		 *\~english
		 *\brief			Writes a resource to a file.
		 *\param[in]		p_object	The object to write.
		 *\param[in,out]	p_file		The file where to write the object.
		 *\~french
		 *\brief			Ecrit une ressource dans un fichier.
		 *\param[in]		p_object	L'objet à écrire.
		 *\param[in,out]	p_file		Le fichier où écrire l'objet.
		 */
		virtual bool operator()( T const & p_object, FileType & p_file )
		{
			LOADER_ERROR( "Export not supported by the loader registered for this type" );
		}
	};
}

#endif
