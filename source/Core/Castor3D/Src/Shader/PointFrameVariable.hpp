/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_POINT_FRAME_VARIABLE_H___
#define ___C3D_POINT_FRAME_VARIABLE_H___

#include "FrameVariable.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		14/08/2010
	\~english
	\brief		Helper structure containing definitions for a point frame variable type.
	\~french
	\brief		Structure d'aide contenant des informations sur une variable de frame de type point.
	*/
	template< typename T, uint32_t Count > struct PntFrameVariableDefinitions;
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		14/08/2010
	\~english
	\brief		Point shader variable with variable type and dimensions
	\~french
	\brief		Variable point à type et dimensions variables
	*/
	template <typename T, uint32_t Count>
	class PointFrameVariable
		: public TFrameVariable<T>
	{
	protected:
		typedef Castor::Policy<T> policy;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_program	The program
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_program	Le programme
		 */
		explicit PointFrameVariable( ShaderProgram & p_program );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_program		The program
		 *\param[in]	p_occurences	The array dimension
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_program		Le programme
		 *\param[in]	p_occurences	Les dimensions du tableau
		 */
		PointFrameVariable( ShaderProgram & p_program, uint32_t p_occurences );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~PointFrameVariable();
		/**
		 *\~english
		 *\brief		Retrieves the value
		 *\return		A Coords containing the value
		 *\~french
		 *\brief		Récupère la valeur
		 *\return		Un Coords contenant la valeur
		 */
		inline Castor::Coords< T, Count > GetValue();
		/**
		 *\~english
		 *\brief		Retrieves the value
		 *\return		A Point containing the value
		 *\~french
		 *\brief		Récupère la valeur
		 *\return		Un Point contenant la valeur
		 */
		inline Castor::Point< T, Count > GetValue()const;
		/**
		 *\~english
		 *\brief		Retrieves the value at given index
		 *\remarks		Check the index bounds
		 *\param[in]	p_index	The index
		 *\return		A Coords containing the value at given index
		 *\~french
		 *\brief		Récupère la valeur à l'index donné
		 *\remarks		Vérifie que l'index est dans les bornes
		 *\param[in]	p_index	L'indice
		 *\return		Un Coords contenant la valeur à l'index donné
		 */
		inline Castor::Coords< T, Count > GetValue( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Retrieves the value at given index
		 *\remarks		Check the index bounds
		 *\param[in]	p_index	The index
		 *\return		A Point containing the value at given index
		 *\~french
		 *\brief		Récupère la valeur à l'index donné
		 *\remarks		Vérifie que l'index est dans les bornes
		 *\param[in]	p_index	L'indice
		 *\return		Un Point contenant la valeur à l'index donné
		 */
		inline Castor::Point< T, Count > GetValue( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Defines the value of the variable
		 *\param[in]	p_ptValue	The new value
		 *\~french
		 *\brief		Définit la valeur de la variable
		 *\param[in]	p_ptValue	La valeur
		 */
		inline void SetValue( Castor::Point< T, Count > const & p_ptValue );
		/**
		 *\~english
		 *\brief		Defines the value of the variable
		 *\param[in]	p_ptValue	The new value
		 *\param[in]	p_index	The index of the value
		 *\~french
		 *\brief		Définit la valeur de la variable
		 *\param[in]	p_ptValue	La valeur
		 *\param[in]	p_index	L'index de la valeur à modifier
		 */
		inline void SetValue( Castor::Point< T, Count > const & p_ptValue, uint32_t p_index );
		/**
		 *\copydoc		Castor3D::FrameVariable::size
		 */
		virtual uint32_t size()const override;
		/**
		 *\~english
		 *\return		The type of the variable.
		 *\~french
		 *\return		Le type de la variable.
		 */
		static inline VariableType GetVariableType();
		/**
		 *\~english
		 *\return		The variable's full type.
		 *\~english
		 *\return		Le type complet de la variable.
		 */
		static inline FrameVariableType GetFrameVariableType();
		/**
		 *\~english
		 *\return		The variable's full type name.
		 *\~english
		 *\return		Le nom du type complet de la variable.
		 */
		static inline Castor::String GetFrameVariableTypeName();
		/**
		 *\~english
		 *\brief		Array subscript operator
		 *\remarks		Doesn't check the index bounds
		 *\param[in]	p_index	The index
		 *\return		A reference to the value at given index
		 *\~french
		 *\brief		Opérateur d'accès de type tableau
		 *\remarks		Ne vérifie pas que l'index est dans les bornes
		 *\param[in]	p_index	L'indice
		 *\return		Une référence sur la valeur à l'index donné
		 */
		inline Castor::Coords< T, Count > operator[]( uint32_t p_index )
		{
			return Castor::Coords< T, Count >( &this->m_values[p_index * Count] );
		}
		/**
		 *\~english
		 *\brief		Array subscript operator
		 *\remarks		Doesn't check the index bounds
		 *\param[in]	p_index	The index
		 *\return		A constant reference to the value at given index
		 *\~french
		 *\brief		Opérateur d'accès de type tableau
		 *\remarks		Ne vérifie pas que l'index est dans les bornes
		 *\param[in]	p_index	L'indice
		 *\return		Une référence constante sur la valeur à l'index donné
		 */
		inline Castor::Point< T, Count > operator[]( uint32_t p_index )const
		{
			return Castor::Point< T, Count >( &this->m_values[p_index * Count] );
		}
		/**
		 *\copydoc		Castor3D::FrameVariable::GetType
		 */
		inline VariableType GetType()const override
		{
			return PointFrameVariable< T, Count >::GetVariableType();
		}
		/**
		 *\copydoc		Castor3D::FrameVariable::GetFullType
		 */
		inline FrameVariableType GetFullType()const override
		{
			return PointFrameVariable< T, Count >::GetFrameVariableType();
		}
		/**
		 *\copydoc		Castor3D::FrameVariable::GetFullTypeName
		 */
		inline Castor::String GetFullTypeName()const override
		{
			return PointFrameVariable< T, Count >::GetFrameVariableTypeName();
		}

	private:
		/**
		 *\copydoc		Castor3D::FrameVariable::DoSetStrValue
		 */
		inline void DoSetStrValue( Castor::String const & p_value, uint32_t p_index = 0 )override;
		/**
		 *\copydoc		Castor3D::FrameVariable::DoGetStrValue
		 */
		inline Castor::String DoGetStrValue( uint32_t p_index = 0 )const override;
	};
}

#include "PointFrameVariable.inl"

#endif
