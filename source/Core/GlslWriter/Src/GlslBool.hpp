﻿/*
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
#ifndef ___GLSL_BOOL_H___
#define ___GLSL_BOOL_H___

#include "GlslOptional.hpp"

namespace GLSL
{
	struct Boolean
		: public Type
	{
		inline Boolean();
		inline Boolean( bool p_value );
		inline Boolean( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
		template< typename T > inline Boolean & operator=( T const & p_rhs );
		template< typename T > inline Boolean & operator=( int p_rhs );
		inline operator uint32_t();
	};

	GlslWriter_API Boolean operator==( Type const & p_a, Type const & p_b );
	GlslWriter_API Boolean operator!=( Type const & p_a, Type const & p_b );
}

#include "GlslBool.inl"

#endif