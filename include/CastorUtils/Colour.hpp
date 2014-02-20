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
#ifndef ___CU_Colour___
#	define ___CU_Colour___

#	include "Point.hpp"
#	include "Loader.hpp"

#	undef RGB

namespace Castor
{
	class ColourAttribute;
	class Colour;
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\todo		Remove accessors, create traits
	\~english
	\brief		Defines a colour component (R, G, B or A) to be used in Castor::Colour.
	\remark		Holds conversion operators to be converted either into float or uint8_t, with corresponding operations
				<br />A colour component value is a floating number between 0.0 and 1.0
	\~french
	\brief		Représente une composante de couleur (R, V, B ou A) pour l'utilisation dans Castor::Colour
	\remark		Définit les opérateurs de conversion en float ou uint8_t, avec les opérations correspondantes
				<br />La valeur d'une composante de couleur est un nombre flottant compris entre 0.0 et 1.0
	*/
	class ColourComponent
	{
	private:
		friend class std::array< ColourComponent, 4 >;
		float * m_pfComponent;
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_object	The object to copy
		 *\~french
		 *\brief		Constructeur par recopie
		 *\param[in]	p_object	L'objet à copier
		 */
		ColourComponent()	: m_pfComponent( NULL ){}

	public:
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_fComponent	The component value
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_fComponent	La valeur de la composante
		 */
		ColourComponent( float * p_pfComponent )	: m_pfComponent( p_pfComponent	){}
		/**
		 *\~english
		 *\brief		Assignment operator from float
		 *\param[in]	p_fComponent	The component value, must be between 0.0f and 1.0f
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un float
		 *\param[in]	p_fComponent	La valeur de la composante, doit être comprise entre 0.0f et 1.0f
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( float p_fComponent )
		{
			*m_pfComponent = p_fComponent;
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from double
		 *\param[in]	p_dComponent	The component value, must be between 0.0 and 1.0
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un double
		 *\param[in]	p_dComponent	La valeur de la composante, doit être comprise entre 0.0 et 1.0
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( double p_dComponent )
		{
			*m_pfComponent = float( p_dComponent );
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from long double
		 *\param[in]	p_dComponent	The component value, must be between 0.0 and 1.0
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un long double
		 *\param[in]	p_dComponent	La valeur de la composante, doit être comprise entre 0.0 et 1.0
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( long double p_ldComponent )
		{
			*m_pfComponent = float( p_ldComponent );
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int8_t
		 *\param[in]	p_cComponent	The component value
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int8_t
		 *\param[in]	p_cComponent	La valeur de la composante
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( int8_t p_i8Component )
		{
			*m_pfComponent = float( uint8_t( p_i8Component ) ) / 255.0f;
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint8_t
		 *\param[in]	p_byComponent	The component value
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint8_t
		 *\param[in]	p_byComponent	La valeur de la composante
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( uint8_t p_ui8Component )
		{
			*m_pfComponent = float( p_ui8Component ) / 255.0f;
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int16_t
		 *\param[in]	p_iComponent	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int16_t
		 *\param[in]	p_iComponent	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( int16_t p_i16Component )
		{
			*m_pfComponent = float( p_i16Component ) / 255.0f;
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint16_t
		 *\param[in]	p_uiComponent	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint16_t
		 *\param[in]	p_uiComponent	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( uint16_t p_ui16Component )
		{
			*m_pfComponent = float( p_ui16Component ) / 255.0f;
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int32_t
		 *\param[in]	p_iComponent	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int32_t
		 *\param[in]	p_iComponent	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( int32_t p_i32Component )
		{
			*m_pfComponent = float( p_i32Component ) / 255.0f;
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint32_t
		 *\param[in]	p_uiComponent	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint32_t
		 *\param[in]	p_uiComponent	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( uint32_t p_ui32Component )
		{
			*m_pfComponent = float( p_ui32Component ) / 255.0f;
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int64_t
		 *\param[in]	p_llComponent	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int64_t
		 *\param[in]	p_llComponent	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( int64_t p_i64Component )
		{
			*m_pfComponent = float( p_i64Component ) / 255.0f;
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint64_t
		 *\param[in]	p_llComponent	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint64_t
		 *\param[in]	p_llComponent	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( uint64_t p_ui64Component )
		{
			*m_pfComponent = float( p_ui64Component ) / 255.0f;
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		int8_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int8_t
		 */
		inline int8_t & convert_to( int8_t & p_value )const { return p_value = int8_t( value() * 255.0f ); }
		/**
		 *\~english
		 *\brief		uint8_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint8_t
		 */
		inline uint8_t & convert_to( uint8_t & p_value )const { return p_value = uint8_t( value() * 255.0f ); }
		/**
		 *\~english
		 *\brief		int16_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int16_t
		 */
		inline int16_t & convert_to( int16_t & p_value )const { return p_value = int16_t( value() * 255.0f ); }
		/**
		 *\~english
		 *\brief		uint16_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint16_t
		 */
		inline uint16_t & convert_to( uint16_t & p_value )const { return p_value = uint16_t( value() * 255.0f ); }
		/**
		 *\~english
		 *\brief		int32_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int32_t
		 */
		inline int32_t & convert_to( int32_t & p_value )const { return p_value = int32_t( value() * 255.0f ); }
		/**
		 *\~english
		 *\brief		uint32_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint32_t
		 */
		inline uint32_t & convert_to( uint32_t & p_value )const { return p_value = uint32_t( value() * 255.0f ); }
		/**
		 *\~english
		 *\brief		int64_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int64_t
		 */
		inline int64_t & convert_to( int64_t & p_value )const { return p_value = int64_t( value() * 255.0f ); }
		/**
		 *\~english
		 *\brief		uint64_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint64_t
		 */
		inline uint64_t & convert_to( uint64_t & p_value )const { return p_value = uint64_t( value() * 255.0f ); }
		/**
		 *\~english
		 *\brief		float conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers float
		 */
		inline float & convert_to( float & p_value )const { return p_value = value(); }
		/**
		 *\~english
		 *\brief		float conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers float
		 */
		inline double & convert_to( double & p_value )const { return p_value = value(); }
		/**
		 *\~english
		 *\brief		float conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers float
		 */
		inline long double & convert_to( long double & p_value )const { return p_value = value(); }
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\remark		Clamps the result
		 *\param[in]	p_component	The value to add
		 *\return		Result of this + p_component
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\remark		Clampe le résultat
		 *\param[in]	p_component	La valeur à ajouter
		 *\return		Resultat de this + p_component
		 */
		ColourComponent & operator +=( ColourComponent && p_component )
		{
			*m_pfComponent += p_component.value();
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\remark		Clamps the result
		 *\param[in]	p_component	The value to substract
		 *\return		Result of this - p_component
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\remark		Clampe le résultat
		 *\param[in]	p_component	La valeur à soustraire
		 *\return		Resultat de this - p_component
		 */
		ColourComponent & operator -=( ColourComponent && p_component )
		{
			*m_pfComponent -= p_component.value();
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\remark		Clamps the result
		 *\param[in]	p_component	The value to multiply
		 *\return		Result of this * p_component
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\remark		Clampe le résultat
		 *\param[in]	p_component	La valeur à multiplier
		 *\return		Resultat de this * p_component
		 */
		ColourComponent & operator *=( ColourComponent && p_component )
		{
			*m_pfComponent *= p_component.value();
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\remark		Clamps the result
		 *\param[in]	p_component	The value to divide
		 *\return		Result of this / p_component
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\remark		Clampe le résultat
		 *\param[in]	p_component	La valeur à diviser
		 *\return		Resultat de this / p_component
		 */
		ColourComponent & operator /=( ColourComponent && p_component )
		{
			*m_pfComponent /= p_component.value();
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_scalar	The value to add
		 *\return		Result of this + p_scalar
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_scalar	La valeur à ajouter
		 *\return		Resultat de this + p_scalar
		 */
		template< typename T >
		ColourComponent & operator +=( T && p_scalar )
		{
			float l_fValue = 0;
			ColourComponent l_component( &l_fValue );
			l_component = p_scalar;
			*m_pfComponent += l_component.value();
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_scalar	The value to substract
		 *\return		Result of this - p_scalar
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_scalar	La valeur à soustraire
		 *\return		Resultat de this - p_scalar
		 */
		template< typename T >
		ColourComponent & operator -=( T && p_scalar )
		{
			float l_fValue = 0;
			ColourComponent l_component( &l_fValue );
			l_component = p_scalar;
			*m_pfComponent -= l_component.value();
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_scalar	The value to multiply
		 *\return		Result of this * p_scalar
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_scalar	La valeur à multiplier
		 *\return		Resultat de this * p_scalar
		 */
		template< typename T >
		ColourComponent & operator *=( T && p_scalar )
		{
			float l_fValue = 0;
			ColourComponent l_component( &l_fValue );
			l_component = p_scalar;
			*m_pfComponent *= l_component.value();
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_scalar	The value to divide
		 *\return		Result of this / p_scalar
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	p_scalar	La valeur à diviser
		 *\return		Resultat de this / p_scalar
		 */
		template< typename T >
		ColourComponent & operator /=( T && p_scalar )
		{
			float l_fValue = 0;
			ColourComponent l_component( &l_fValue );
			l_component = p_scalar;
			*m_pfComponent /= l_component.value();
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Retrieves the component value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur de la composante
		 *\return		La valeur
		 */
		inline float const & value()const { return *m_pfComponent; }

	private:
		void DoClamp()
		{
			if( value() < 0 )
			{
				*m_pfComponent = 0;
			}
			else if( value() > 1 )
			{
				*m_pfComponent = 1;
			}
		}
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_cpnA,p_clrB	The components to compare
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	p_cpnA,p_cpnB	Les composantes à comparer
	 */
	bool operator ==( ColourComponent const & p_cpnA, ColourComponent const & p_cpnB );
	/**
	 *\~english
	 *\brief		Inequality operator
	 *\param[in]	p_cpnA,p_cpnB	The components to compare
	 *\~french
	 *\brief		Opérateur de différence
	 *\param[in]	p_cpnA,p_cpnB	Les composantes à comparer
	 */
	bool operator !=( ColourComponent const & p_cpnA, ColourComponent const & p_cpnB );
	/**
	 *\~english
	 *\brief		Addition assignment operator
	 *\param[in]	p_scalar	The value to add
	 *\return		Result of this + p_scalar
	 *\~french
	 *\brief		Opérateur d'affectation par addition
	 *\param[in]	p_scalar	La valeur à ajouter
	 *\return		Resultat de this + p_scalar
	 */
	template< typename T >
	float operator +( ColourComponent const & p_cpnt, T && p_scalar )
	{
		float l_fValue;
		p_cpnt.convert_to( l_fValue );
		ColourComponent l_cpnt( &l_fValue );
		l_cpnt += p_scalar;
		return l_fValue;
	}
	/**
	 *\~english
	 *\brief		Substraction assignment operator
	 *\param[in]	p_scalar	The value to substract
	 *\return		Result of this - p_scalar
	 *\~french
	 *\brief		Opérateur d'affectation par soustraction
	 *\param[in]	p_scalar	La valeur à soustraire
	 *\return		Resultat de this - p_scalar
	 */
	template< typename T >
	float operator -( ColourComponent const & p_cpnt, T && p_scalar )
	{
		float l_fValue;
		p_cpnt.convert_to( l_fValue );
		ColourComponent l_cpnt( &l_fValue );
		l_cpnt -= p_scalar;
		return l_fValue;
	}
	/**
	 *\~english
	 *\brief		Division assignment operator
	 *\param[in]	p_scalar	The value to divide
	 *\return		Result of this / p_scalar
	 *\~french
	 *\brief		Opérateur d'affectation par division
	 *\param[in]	p_scalar	La valeur à diviser
	 *\return		Resultat de this / p_scalar
	 */
	template< typename T >
	float operator /( ColourComponent const & p_cpnt, T && p_scalar )
	{
		float l_fValue;
		p_cpnt.convert_to( l_fValue );
		ColourComponent l_cpnt( &l_fValue );
		l_cpnt /= p_scalar;
		return l_fValue;
	}
	/**
	 *\~english
	 *\brief		Multiplication assignment operator
	 *\param[in]	p_scalar	The value to multiply
	 *\return		Result of this * p_scalar
	 *\~french
	 *\brief		Opérateur d'affectation par multiplication
	 *\param[in]	p_scalar	La valeur à multiplier
	 *\return		Resultat de this * p_scalar
	 */
	template< typename T >
	float operator *( ColourComponent const & p_cpnt, T && p_scalar )
	{
		float l_fValue;
		p_cpnt.convert_to( l_fValue );
		ColourComponent l_cpnt( &l_fValue );
		l_cpnt *= p_scalar;
		return l_fValue;
	}
	/**
	 *\~english
	 *\brief		Substraction assignment operator
	 *\param[in]	p_scalar	The value to substract
	 *\return		Result of this - p_scalar
	 *\~french
	 *\brief		Opérateur d'affectation par soustraction
	 *\param[in]	p_scalar	La valeur à soustraire
	 *\return		Resultat de this - p_scalar
	 */
	float operator -( ColourComponent const & p_cpnt, ColourComponent const & p_scalar );
	/**
	 *\~english
	 *\brief		Addition assignment operator
	 *\param[in]	p_scalar	The value to add
	 *\return		Result of this + p_scalar
	 *\~french
	 *\brief		Opérateur d'affectation par addition
	 *\param[in]	p_scalar	La valeur à ajouter
	 *\return		Resultat de this + p_scalar
	 */
	float operator +( ColourComponent const & p_cpnt, ColourComponent const & p_scalar );
	/**
	 *\~english
	 *\brief		Multiplication assignment operator
	 *\param[in]	p_scalar	The value to multiply
	 *\return		Result of this * p_scalar
	 *\~french
	 *\brief		Opérateur d'affectation par multiplication
	 *\param[in]	p_scalar	La valeur à multiplier
	 *\return		Resultat de this * p_scalar
	 */
	float operator *( ColourComponent const & p_cpnt, ColourComponent const & p_scalar );
	/**
	 *\~english
	 *\brief		Division assignment operator
	 *\param[in]	p_scalar	The value to divide
	 *\return		Result of this / p_scalar
	 *\~french
	 *\brief		Opérateur d'affectation par division
	 *\param[in]	p_scalar	La valeur à diviser
	 *\return		Resultat de this / p_scalar
	 */
	float operator /( ColourComponent const & p_cpnt, ColourComponent const & p_scalar );
	/*!
	\author		Sylvain DOREMUS
	\date		14/08/2010
	\~english
	\brief		RGBA colour representation
	\remark		Predefines numerous colours
				<br />Allows conversion to different colour formats (RGB, ARGB, BGR, ...) and different data types (long, float, char, ...)
	\~french
	\brief		Représentation d'une couleur RGBA
	\remark		Prédéfinit differentes couleurs
				<br />Permet la conversion entre différents types de couleurs (RGB, ARGB, BGR, ...) et différents types de données (long, float, char, ...)
	*/
	class Colour
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		Colour binary loader
		\~french
		\brief		Loader de Colour à partir de données binaires
		*/
		class BinaryLoader : public Castor::Loader< Colour, eFILE_TYPE_BINARY, BinaryFile >, CuNonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			BinaryLoader();
			/**
			 *\~english
			 *\brief			Reads a colour from a binary file
			 *\param[in,out]	p_file		The file from where we read the colour
			 *\param[in,out]	p_colour	The colour to read
			 *\~french
			 *\brief			Lit une couleur à partir d'un fichier binaire
			 *\param[in,out]	p_file		Le fichier dans lequel on lit la couleur
			 *\param[in,out]	p_colour	La couleur à lire
			 */
			virtual bool operator ()( Colour & p_colour, BinaryFile & p_file );
			/**
			 *\~english
			 *\brief			Writes a colour into a binary file
			 *\param[in,out]	p_file		The file into which colour is written
			 *\param[in]		p_colour	The colour to write
			 *\~french
			 *\brief			Ecrit une couleur dans un fichier binaire
			 *\param[in,out]	p_file		Le fichier dans lequel on écrit la couleur
			 *\param[in]		p_colour	La couleur à écrire
			 */
			virtual bool operator ()( Colour const & p_colour, BinaryFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		Colour text loader
		\~french
		\brief		Loader de Colour à partir d'un texte
		*/
		class TextLoader : public Castor::Loader< Colour, eFILE_TYPE_TEXT, TextFile >, CuNonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( File::eENCODING_MODE p_eEncodingMode=File::eENCODING_MODE_ASCII );
			/**
			 *\~english
			 *\brief			Reads a colour from a text file
			 *\param[in,out]	p_file		The file from where we read the colour
			 *\param[in,out]	p_colour	The colour to read
			 *\~french
			 *\brief			Lit une couleur à partir d'un fichier texte
			 *\param[in,out]	p_file		Le fichier dans lequel on lit la couleur
			 *\param[in,out]	p_colour	La couleur à lire
			 */
			virtual bool operator ()( Colour & p_colour, TextFile & p_file );
			/**
			 *\~english
			 *\brief			Writes a colour into a text file
			 *\param[in,out]	p_file		The file into which colour is written
			 *\param[in]		p_colour	The colour to write
			 *\~french
			 *\brief			Ecrit une couleur dans un fichier texte
			 *\param[in,out]	p_file		Le fichier dans lequel on écrit la couleur
			 *\param[in]		p_colour	La couleur à écrire
			 */
			virtual bool operator ()( Colour const & p_colour, TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Predefined colours enumeration
		\~french
		\brief		Enumération de couleurs prédéfinies
		*/
		typedef enum ePREDEFINED  CASTOR_TYPE( uint32_t )
		{	ePREDEFINED_TRANSPARENT_BLACK		= 0x00000000,	ePREDEFINED_LOWALPHA_BLACK		= 0x00000040,	ePREDEFINED_MEDALPHA_BLACK		= 0x0000007F,	ePREDEFINED_HIGHALPHA_BLACK			= 0x000000BF,	ePREDEFINED_FULLALPHA_BLACK			= 0x000000FF
		,	ePREDEFINED_TRANSPARENT_DARKBLUE	= 0x00007F00,	ePREDEFINED_LOWALPHA_DARKBLUE	= 0x00007F40,	ePREDEFINED_MEDALPHA_DARKBLUE	= 0x00007F7F,	ePREDEFINED_HIGHALPHA_DARKBLUE		= 0x00007FBF,	ePREDEFINED_FULLALPHA_DARKBLUE		= 0x00007FFF
		,	ePREDEFINED_TRANSPARENT_BLUE		= 0x0000FF00,	ePREDEFINED_LOWALPHA_BLUE		= 0x0000FF40,	ePREDEFINED_MEDALPHA_BLUE		= 0x0000FF7F,	ePREDEFINED_HIGHALPHA_BLUE			= 0x0000FFBF,	ePREDEFINED_FULLALPHA_BLUE			= 0x0000FFFF
		,	ePREDEFINED_TRANSPARENT_DARKGREEN	= 0x007F0000,	ePREDEFINED_LOWALPHA_DARKGREEN	= 0x007F0040,	ePREDEFINED_MEDALPHA_DARKGREEN	= 0x007F007F,	ePREDEFINED_HIGHALPHA_DARKGREEN		= 0x007F00BF,	ePREDEFINED_FULLALPHA_DARKGREEN		= 0x007F00FF
		,	ePREDEFINED_TRANSPARENT_GREEN		= 0x00FF0000,	ePREDEFINED_LOWALPHA_GREEN		= 0x00FF0040,	ePREDEFINED_MEDALPHA_GREEN		= 0x00FF007F,	ePREDEFINED_HIGHALPHA_GREEN			= 0x00FF00BF,	ePREDEFINED_FULLALPHA_GREEN			= 0x00FF00FF
		,	ePREDEFINED_TRANSPARENT_DARKRED		= 0x7F000000,	ePREDEFINED_LOWALPHA_DARKRED	= 0x7F000040,	ePREDEFINED_MEDALPHA_DARKRED	= 0x7F00007F,	ePREDEFINED_HIGHALPHA_DARKRED		= 0x7F0000BF,	ePREDEFINED_FULLALPHA_DARKRED		= 0x7F0000FF
		,	ePREDEFINED_TRANSPARENT_RED			= 0xFF000000,	ePREDEFINED_LOWALPHA_RED		= 0xFF000040,	ePREDEFINED_MEDALPHA_RED		= 0xFF00007F,	ePREDEFINED_HIGHALPHA_RED			= 0xFF0000BF,	ePREDEFINED_FULLALPHA_RED			= 0xFF0000FF
		,	ePREDEFINED_TRANSPARENT_LIGHTBLUE	= 0x7F7FFF00,	ePREDEFINED_LOWALPHA_LIGHTBLUE	= 0x7F7FFF40,	ePREDEFINED_MEDALPHA_LIGHTBLUE	= 0x7F7FFF7F,	ePREDEFINED_HIGHALPHA_LIGHTBLUE		= 0x7F7FFFBF,	ePREDEFINED_FULLALPHA_LIGHTBLUE		= 0x7F7FFFFF
		,	ePREDEFINED_TRANSPARENT_LIGHTGREEN	= 0x7FFF7F00,	ePREDEFINED_LOWALPHA_LIGHTGREEN	= 0x7FFF7F40,	ePREDEFINED_MEDALPHA_LIGHTGREEN	= 0x7FFF7F7F,	ePREDEFINED_HIGHALPHA_LIGHTGREEN	= 0x7FFF7FBF,	ePREDEFINED_FULLALPHA_LIGHTGREEN	= 0x7FFF7FFF
		,	ePREDEFINED_TRANSPARENT_LIGHTRED	= 0xFF7F7F00,	ePREDEFINED_LOWALPHA_LIGHTRED	= 0xFF7F7F40,	ePREDEFINED_MEDALPHA_LIGHTRED	= 0xFF7F7F7F,	ePREDEFINED_HIGHALPHA_LIGHTRED		= 0xFF7F7FBF,	ePREDEFINED_FULLALPHA_LIGHTRED		= 0xFF7F7FFF
		,	ePREDEFINED_TRANSPARENT_WHITE		= 0xFFFFFF00,	ePREDEFINED_LOWALPHA_WHITE		= 0xFFFFFF40,	ePREDEFINED_MEDALPHA_WHITE		= 0xFFFFFF7F,	ePREDEFINED_HIGHALPHA_WHITE			= 0xFFFFFFBF,	ePREDEFINED_FULLALPHA_WHITE			= 0xFFFFFFFF
		}	ePREDEFINED;
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Colour components enumeration
		\~french
		\brief		Enumération des composantes d'une couleur
		*/
		typedef enum eCOMPONENT  CASTOR_TYPE( uint8_t )
		{	eCOMPONENT_RED		= 0	//!<\~english	Red component	\~french	Composante rouge
		,	eCOMPONENT_GREEN	= 1	//!<\~english	Green component	\~french	Composante verte
		,	eCOMPONENT_BLUE		= 2	//!<\~english	Blue component	\~french	Composante bleue
		,	eCOMPONENT_ALPHA	= 3	//!<\~english	Alpha component	\~french	Composante alpha
		,	eCOMPONENT_COUNT	= 4
		}	eCOMPONENT;

	private:
		friend void swap( Colour & p_c1, Colour & p_c2 );
		DECLARE_ARRAY( float, eCOMPONENT_COUNT, Float4 );
		DECLARE_ARRAY( ColourComponent, eCOMPONENT_COUNT, ColourComponent );
		ColourComponentArray m_arrayComponents;
		Float4Array m_arrayValues;
		DECLARE_INVARIANT_BLOCK()

	public:
		/**
		 *\~english
		 *\brief		Default Constructor
		 *\~french
		 *\brief		Constructeur par défaut
		 */
		Colour();
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\~french
		 *\brief		Constructeur par copie
		 */
		Colour( Colour const & p_colour );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\~french
		 *\brief		Constructeur par déplacement
		 */
		Colour( Colour && p_colour );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_copy	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_copy	L'objet à copier
		 *\return		Une référence sur cet objet
		 */
		Colour & operator =( Colour const & p_colour );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_colour	Colour to copy
		 *\return		Reference to this colour
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_colour	Couleur à copier
		 *\return		Référence sur cette couleur
		 */
		Colour & operator =( Colour && p_colour );
		/**
		 *\~english
		 *\brief		Constructor from components
		 *\param[in]	p_r, p_g, p_b, p_a	The colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir des composantes
		 *\param[in]	p_r, p_g, p_b, p_a	Les composantes de la couleur
		 *\return		La Colour construite
		 */
		template< typename T1, typename T2, typename T3, typename T4 >
		static Colour from_components( T1 && p_r, T2 && p_g, T3 && p_b, T4 && p_a )
		{
			Colour l_clrReturn;
			l_clrReturn.m_arrayComponents[eCOMPONENT_RED]	= p_r;
			l_clrReturn.m_arrayComponents[eCOMPONENT_GREEN]	= p_g;
			l_clrReturn.m_arrayComponents[eCOMPONENT_BLUE]	= p_b;
			l_clrReturn.m_arrayComponents[eCOMPONENT_ALPHA]	= p_a;
			return l_clrReturn;
		}
		/**
		 *\~english
		 *\brief		Constructor from a predefined colour
		 *\param[in]	p_ePredef
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'une couleur prédéfinie
		 *\param[in]	p_ePredef
		 *\return		La Colour construite
		 */
		static Colour from_predef( ePREDEFINED p_ePredef ) { return from_rgba( (uint32_t)p_ePredef ); }
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGB components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGB
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		static Colour from_rgb( Point3ub const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGR components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGR
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		static Colour from_bgr( Point3ub const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGBA components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGBA
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		static Colour from_rgba( Point4ub const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGRA components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGRA
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		static Colour from_bgra( Point4ub const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ARGB components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ARGB
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		static Colour from_argb( Point4ub const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ABGR components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ABGR
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		static Colour from_abgr( Point4ub const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGB components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGB
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		static Colour from_rgb( Point3f const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGR components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGR
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		static Colour from_bgr( Point3f const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGBA components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGBA
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		static Colour from_rgba( Point4f const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ARGB components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ARGB
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		static Colour from_argb( Point4f const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ABGR components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ABGR
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		static Colour from_abgr( Point4f const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGRA components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGRA
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		static Colour from_bgra( Point4f const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing RGB components
		 *\param[in]	p_colour	The uint32_t containing the colour (0x00RRGGBB)
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes RGB
		 *\param[in]	p_colour	L'uint32_t contenant la couleur (0x00RRGGBB)
		 *\return		La Colour construite
		 */
		static Colour from_rgb( uint32_t p_colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing BGR components
		 *\param[in]	p_colour	The uint32_t containing the colour (0x00BBGGRR)
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes BGR
		 *\param[in]	p_colour	L'uint32_t contenant la couleur (0x00BBGGRR)
		 *\return		La Colour construite
		 */
		static Colour from_bgr( uint32_t p_colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing ARGB components
		 *\param[in]	p_colour	The uint32_t containing the colour (0xAARRGGBB)
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes ARGB
		 *\param[in]	p_colour	L'uint32_t contenant la couleur (0xAARRGGBB)
		 *\return		La Colour construite
		 */
		static Colour from_argb( uint32_t p_colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing RGBA components
		 *\param[in]	p_colour	The uint32_t containing the colour (0xRRGGBBAA)
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes RGBA
		 *\param[in]	p_colour	L'uint32_t contenant la couleur (0xRRGGBBAA)
		 *\return		La Colour construite
		 */
		static Colour from_rgba( uint32_t p_colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing ABGR components
		 *\param[in]	p_colour	The uint32_t containing the colour (0xAABBGGRR)
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes ABGR
		 *\param[in]	p_colour	L'uint32_t contenant la couleur (0xAABBGGRR)
		 *\return		La Colour construite
		 */
		static Colour from_abgr( uint32_t p_colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing BGRA components
		 *\param[in]	p_colour	The uint32_t containing the colour (0xBBGGRRAA)
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes BGRA
		 *\param[in]	p_colour	L'uint32_t contenant la couleur (0xBBGGRRAA)
		 *\return		La Colour construite
		 */
		static Colour from_bgra( uint32_t p_colour );
		/**
		 *\~english
		 *\brief		Stores this colour's components into a point in RGB form
		 *\param[out]	p_ptResult	Receives the colour components
		 *\~french
		 *\brief		Stoque les composantes de cette couleur dans un point, au format RGB
		 *\param[out]	p_ptResult	Reçoit les composantes de la couleur
		 */
		void to_rgb( Point3ub & p_ptResult )const;
		/**
		 *\~english
		 *\brief		Stores this colour's components into a point in BGR form
		 *\param[out]	p_ptResult	Receives the colour components
		 *\~french
		 *\brief		Stoque les composantes de cette couleur dans un point, au format BGR
		 *\param[out]	p_ptResult	Reçoit les composantes de la couleur
		 */
		void to_bgr( Point3ub & p_ptResult )const;
		/**
		 *\~english
		 *\brief		Stores this colour's components into a point in RGBA form
		 *\param[out]	p_ptResult	Receives the colour components
		 *\~french
		 *\brief		Stoque les composantes de cette couleur dans un point, au format RGBA
		 *\param[out]	p_ptResult	Reçoit les composantes de la couleur
		 */
		void to_rgba( Point4ub & p_ptResult )const;
		/**
		 *\~english
		 *\brief		Stores this colour's components into a point in BGRA form
		 *\param[out]	p_ptResult	Receives the colour components
		 *\~french
		 *\brief		Stoque les composantes de cette couleur dans un point, au format BGRA
		 *\param[out]	p_ptResult	Reçoit les composantes de la couleur
		 */
		void to_bgra( Point4ub & p_ptResult )const;
		/**
		 *\~english
		 *\brief		Stores this colour's components into a point in ARGB form
		 *\param[out]	p_ptResult	Receives the colour components
		 *\~french
		 *\brief		Stoque les composantes de cette couleur dans un point, au format ARGB
		 *\param[out]	p_ptResult	Reçoit les composantes de la couleur
		 */
		void to_argb( Point4ub & p_ptResult )const;
		/**
		 *\~english
		 *\brief		Stores this colour's components into a point in ABGR form
		 *\param[out]	p_ptResult	Receives the colour components
		 *\~french
		 *\brief		Stoque les composantes de cette couleur dans un point, au format ABGR
		 *\param[out]	p_ptResult	Reçoit les composantes de la couleur
		 */
		void to_abgr( Point4ub & p_ptResult )const;
		/**
		 *\~english
		 *\brief		Stores this colour's components into a point in RGB form
		 *\param[out]	p_ptResult	Receives the colour components
		 *\~french
		 *\brief		Stoque les composantes de cette couleur dans un point, au format RGB
		 *\param[out]	p_ptResult	Reçoit les composantes de la couleur
		 */
		void to_rgb( Point3f & p_ptResult )const;
		/**
		 *\~english
		 *\brief		Stores this colour's components into a point in BGR form
		 *\param[out]	p_ptResult	Receives the colour components
		 *\~french
		 *\brief		Stoque les composantes de cette couleur dans un point, au format BGR
		 *\param[out]	p_ptResult	Reçoit les composantes de la couleur
		 */
		void to_bgr( Point3f & p_ptResult )const;
		/**
		 *\~english
		 *\brief		Stores this colour's components into a point in RGBA form
		 *\param[out]	p_ptResult	Receives the colour components
		 *\~french
		 *\brief		Stoque les composantes de cette couleur dans un point, au format RGBA
		 *\param[out]	p_ptResult	Reçoit les composantes de la couleur
		 */
		void to_rgba( Point4f & p_ptResult )const;
		/**
		 *\~english
		 *\brief		Stores this colour's components into a point in ARGB form
		 *\param[out]	p_ptResult	Receives the colour components
		 *\~french
		 *\brief		Stoque les composantes de cette couleur dans un point, au format ARGB
		 *\param[out]	p_ptResult	Reçoit les composantes de la couleur
		 */
		void to_argb( Point4f & p_ptResult )const;
		/**
		 *\~english
		 *\brief		Stores this colour's components into a point in ABGR form
		 *\param[out]	p_ptResult	Receives the colour components
		 *\~french
		 *\brief		Stoque les composantes de cette couleur dans un point, au format ABGR
		 *\param[out]	p_ptResult	Reçoit les composantes de la couleur
		 */
		void to_abgr( Point4f & p_ptResult )const;
		/**
		 *\~english
		 *\brief		Stores this colour's components into a point in BGRA form
		 *\param[out]	p_ptResult	Receives the colour components
		 *\~french
		 *\brief		Stoque les composantes de cette couleur dans un point, au format BGRA
		 *\param[out]	p_ptResult	Reçoit les composantes de la couleur
		 */
		void to_bgra( Point4f & p_ptResult )const;
		/**
		 *\~english
		 *\brief		Compiles the components into an uint32_t in the RGB format (0x00RRGGBB)
		 *\return		The compiled colour
		 *\~french
		 *\brief		Compile les composantes de la couleur dans un uint32_t, au format RGB (0x00RRGGBB)
		 *\return		La couleur compilée
		 */
		uint32_t to_rgb()const;
		/**
		 *\~english
		 *\brief		Compiles the components into an uint32_t in the BGR format (0x00BBGGRR)
		 *\return		The compiled colour
		 *\~french
		 *\brief		Compile les composantes de la couleur dans un uint32_t, au format BGR (0x00BBGGRR)
		 *\return		La couleur compilée
		 */
		uint32_t to_bgr()const;
		/**
		 *\~english
		 *\brief		Compiles the components into an uint32_t in the ARGB format (0xAARRGGBB)
		 *\return		The compiled colour
		 *\~french
		 *\brief		Compile les composantes de la couleur dans un uint32_t, au format ARGB (0xAARRGGBB)
		 *\return		La couleur compilée
		 */
		uint32_t to_argb()const;
		/**
		 *\~english
		 *\brief		Compiles the components into an uint32_t in the RGBA format (0xRRGGBBAA)
		 *\return		The compiled colour
		 *\~french
		 *\brief		Compile les composantes de la couleur dans un uint32_t, au format RGBA (0xRRGGBBAA)
		 *\return		La couleur compilée
		 */
		uint32_t to_rgba()const;
		/**
		 *\~english
		 *\brief		Compiles the components into an uint32_t in the ABGR format (0xAABBGGRR)
		 *\return		The compiled colour
		 *\~french
		 *\brief		Compile les composantes de la couleur dans un uint32_t, au format ABGR (0xAABBGGRR)
		 *\return		La couleur compilée
		 */
		uint32_t to_abgr()const;
		/**
		 *\~english
		 *\brief		Compiles the components into an uint32_t in the BGRA format (0xBBGGRRAA)
		 *\return		The compiled colour
		 *\~french
		 *\brief		Compile les composantes de la couleur dans un uint32_t, au format BGRA (0xBBGGRRAA)
		 *\return		La couleur compilée
		 */
		uint32_t to_bgra()const;
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first ColourComponent
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le premier ColourComponent
		 *\return		L'itérateur
		 */
		inline ColourComponentArrayIt begin() { return m_arrayComponents.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the first ColourComponent
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur le premier ColourComponent
		 *\return		L'itérateur
		 */
		inline ColourComponentArrayConstIt begin()const { return m_arrayComponents.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to beyond the last ColourComponent
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur après le dernier ColourComponent
		 *\return		L'itérateur
		 */
		inline ColourComponentArrayIt end() { return m_arrayComponents.end(); }
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to beyond the last ColourComponent
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur après le dernier ColourComponent
		 *\return		L'itérateur
		 */
		inline ColourComponentArrayConstIt end()const { return m_arrayComponents.end(); }
		/**
		 *\~english
		 *\brief		Retrieves a constant pointer to the colour values
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur constant sur les valeurs de la couleur
		 *\return		Le pointeur
		 */
		inline float const * const_ptr()const { return &m_arrayValues[0]; }
		/**
		 *\~english
		 *\brief		Retrieves a pointer to the colour values
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur sur les valeurs de la couleur
		 *\return		Le pointeur
		 */
		inline float * ptr() { return &m_arrayValues[0]; }
		/**
		 *\~english
		 *\brief		Retrieves the asked component value
		 *\param[in]	p_eComponent	The asked component
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante demandée
		 *\param[in]	p_eComponent	La composante demandée
		 *\return		La valeur de la composante
		 */
		inline ColourComponent & operator []( eCOMPONENT p_eComponent ) { return m_arrayComponents[p_eComponent]; }
		/**
		 *\~english
		 *\brief		Retrieves the asked component value
		 *\param[in]	p_eComponent	The asked component
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante demandée
		 *\param[in]	p_eComponent	La composante demandée
		 *\return		La valeur de la composante
		 */
		inline ColourComponent const & operator []( eCOMPONENT p_eComponent )const { return m_arrayComponents[p_eComponent]; }
		/**
		 *\~english
		 *\brief		Retrieves the asked component value
		 *\param[in]	p_eComponent	The asked component
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante demandée
		 *\param[in]	p_eComponent	La composante demandée
		 *\return		La valeur de la composante
		 */
		inline ColourComponent & get( eCOMPONENT p_eComponent ) { return m_arrayComponents[p_eComponent]; }
		/**
		 *\~english
		 *\brief		Retrieves the asked component value
		 *\param[in]	p_eComponent	The asked component
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante demandée
		 *\param[in]	p_eComponent	La composante demandée
		 *\return		La valeur de la composante
		 */
		inline ColourComponent const & get( eCOMPONENT p_eComponent )const { return m_arrayComponents[p_eComponent]; }
		/**
		 *\~english
		 *\brief		Retrieves the red component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante rouge
		 *\return		La valeur de la composante
		 */
		inline ColourComponent & red() { return get( eCOMPONENT_RED ); }
		/**
		 *\~english
		 *\brief		Retrieves the red component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante rouge
		 *\return		La valeur de la composante
		 */
		inline ColourComponent const & red()const { return get( eCOMPONENT_RED ); }
		/**
		 *\~english
		 *\brief		Retrieves the green component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante verte
		 *\return		La valeur de la composante
		 */
		inline ColourComponent & green() { return get( eCOMPONENT_GREEN ); }
		/**
		 *\~english
		 *\brief		Retrieves the green component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante verte
		 *\return		La valeur de la composante
		 */
		inline ColourComponent const & green()const { return get( eCOMPONENT_GREEN ); }
		/**
		 *\~english
		 *\brief		Retrieves the blue component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante bleue
		 *\return		La valeur de la composante
		 */
		inline ColourComponent & blue() { return get( eCOMPONENT_BLUE );	}
		/**
		 *\~english
		 *\brief		Retrieves the blue component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante bleue
		 *\return		La valeur de la composante
		 */
		inline ColourComponent const & blue()const { return get( eCOMPONENT_BLUE );	}
		/**
		 *\~english
		 *\brief		Retrieves the alpha component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante alpha
		 *\return		La valeur de la composante
		 */
		inline ColourComponent & alpha() { return get( eCOMPONENT_ALPHA );	}
		/**
		 *\~english
		 *\brief		Retrieves the alpha component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante alpha
		 *\return		La valeur de la composante
		 */
		inline ColourComponent const & alpha()const { return get( eCOMPONENT_ALPHA );	}
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_clrB	The colours to add
		 *\return		Result of this + p_clrB
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_clrB	Les couleurs à ajouter
		 *\return		Resultat de this + p_clrB
		 */
		Colour & operator +=( Colour const & p_clrB );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_clrB	The colours to substract
		 *\return		Result of this - p_clrB
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_clrB	Les couleurs à soustraire
		 *\return		Resultat de this - p_clrB
		 */
		Colour & operator -=( Colour const & p_clrB );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_scalar	The value to add
		 *\return		Result of this + p_scalar
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_scalar	La valeur à ajouter
		 *\return		Resultat de this + p_scalar
		 */
		template< typename T >
		Colour & operator +=( T p_scalar )
		{
			for( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
			{
				m_arrayComponents[i] += p_scalar;
			}

			return *this;
		}
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_scalar	The value to substract
		 *\return		Result of this - p_scalar
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_scalar	La valeur à soustraire
		 *\return		Resultat de this - p_scalar
		 */
		template< typename T >
		Colour & operator -=( T p_scalar )
		{
			for( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
			{
				m_arrayComponents[i] -= p_scalar;
			}

			return *this;
		}
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_scalar	The value to multiply
		 *\return		Result of this * p_scalar
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_scalar	La valeur à multiplier
		 *\return		Resultat de this * p_scalar
		 */
		template< typename T >
		Colour & operator *=( T p_scalar )
		{
			for( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
			{
				m_arrayComponents[i] *= p_scalar;
			}

			return *this;
		}
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_scalar	The value to divide
		 *\return		Result of this / p_scalar
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	p_scalar	La valeur à diviser
		 *\return		Resultat de this / p_scalar
		 */
		template< typename T >
		Colour & operator /=( T p_scalar )
		{
			for( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
			{
				m_arrayComponents[i] /= p_scalar;
			}

			return *this;
		}
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_component	The value to add
		 *\return		Result of this + p_component
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_component	La valeur à ajouter
		 *\return		Resultat de this + p_component
		 */
		Colour & operator +=( ColourComponent const & p_component );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_component	The value to substract
		 *\return		Result of this - p_component
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_component	La valeur à soustraire
		 *\return		Resultat de this - p_component
		 */
		Colour & operator -=( ColourComponent const & p_component );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_component	The value to multiply
		 *\return		Result of this * p_component
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_component	La valeur à multiplier
		 *\return		Resultat de this * p_component
		 */
		Colour & operator *=( ColourComponent const & p_component );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_component	The value to divide
		 *\return		Result of this / p_component
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	p_component	La valeur à diviser
		 *\return		Resultat de this / p_component
		 */
		Colour & operator /=( ColourComponent const & p_component );
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_clrA,p_clrB	The colours to compare
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	p_clrA,p_clrB	Les couleurs à comparer
	 */
	bool operator ==( Colour const & p_clrA, Colour const & p_clrB );
	/**
	 *\~english
	 *\brief		Inequality operator
	 *\param[in]	p_clrA,p_clrB	The colours to compare
	 *\~french
	 *\brief		Opérateur de différence
	 *\param[in]	p_clrA,p_clrB	Les couleurs à comparer
	 */
	bool operator !=( Colour const & p_clrA, Colour const & p_clrB );
	/**
	 *\~english
	 *\brief		Addition assignment operator
	 *\param[in]	p_clrA,p_clrB	The colours to add
	 *\return		Result of p_clrA + p_clrB
	 *\~french
	 *\brief		Opérateur d'affectation par addition
	 *\param[in]	p_clrA,p_clrB	Les couleurs à ajouter
	 *\return		Resultat de p_clrA + p_clrB
	 */
	Colour operator +( Colour const & p_clrA, Colour const & p_clrB );
	/**
	 *\~english
	 *\brief		Substraction assignment operator
	 *\param[in]	p_clrA,p_clrB	The colours to substract
	 *\return		Result of p_clrA - p_clrB
	 *\~french
	 *\brief		Opérateur d'affectation par soustraction
	 *\param[in]	p_clrA,p_clrB	Les couleurs à soustraire
	 *\return		Resultat de p_clrA - p_clrB
	 */
	Colour operator -( Colour const & p_clrA, Colour const & p_clrB );
	/**
	 *\~english
	 *\brief		Addition assignment operator
	 *\param[in]	p_fScalar	The value to add
	 *\return		Result of p_clrA + p_fScalar
	 *\~french
	 *\brief		Opérateur d'affectation par addition
	 *\param[in]	p_fScalar	La valeur à ajouter
	 *\return		Resultat de p_clrA + p_fScalar
	 */
	template< typename T >
	Colour operator +( Colour const & p_clrA, T p_fScalar )
	{
		Colour l_clrResult( p_clrA );
		l_clrResult += p_fScalar;
		return l_clrResult;
	}

	/**
	 *\~english
	 *\brief		Substraction assignment operator
	 *\param[in]	p_fScalar	The value to substract
	 *\return		Result of p_clrA - p_fScalar
	 *\~french
	 *\brief		Opérateur d'affectation par soustraction
	 *\param[in]	p_fScalar	La valeur à soustraire
	 *\return		Resultat de p_clrA - p_fScalar
	 */
	template< typename T >
	Colour operator -( Colour const & p_clrA, T p_fScalar )
	{
		Colour l_clrResult( p_clrA );
		l_clrResult -= p_fScalar;
		return l_clrResult;
	}

	/**
	 *\~english
	 *\brief		Multiplication assignment operator
	 *\param[in]	p_fScalar	The value to multiply
	 *\return		Result of p_clrA * p_fScalar
	 *\~french
	 *\brief		Opérateur d'affectation par multiplication
	 *\param[in]	p_fScalar	La valeur à multiplier
	 *\return		Resultat de p_clrA * p_fScalar
	 */
	template< typename T >
	Colour operator *( Colour const & p_clrA, T p_fScalar )
	{
		Colour l_clrResult( p_clrA );
		l_clrResult *= p_fScalar;
		return l_clrResult;
	}

	/**
	 *\~english
	 *\brief		Division assignment operator
	 *\param[in]	p_fScalar	The value to divide
	 *\return		Result of p_clrA / p_fScalar
	 *\~french
	 *\brief		Opérateur d'affectation par division
	 *\param[in]	p_fScalar	La valeur à diviser
	 *\return		Resultat de p_clrA / p_fScalar
	 */
	template< typename T >
	Colour operator /( Colour const & p_clrA, T p_fScalar )
	{
		Colour l_clrResult( p_clrA );
		l_clrResult /= p_fScalar;
		return l_clrResult;
	}
	/**
	 *\~english
	 *\brief		Swapping function
	 *\param[in]	p_c1, p_c2	The two colours to swap
	 *\~french
	 *\brief		Fonction d'échange
	 *\param[in]	p_c1, p_c2	Les 2 couleurs à échanger
	 */
	void swap( Colour & p_c1, Colour & p_c2 );
}

#endif
