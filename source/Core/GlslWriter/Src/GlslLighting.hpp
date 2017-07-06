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
#ifndef ___GLSL_LIGHTING_H___
#define ___GLSL_LIGHTING_H___

#include "GlslShadow.hpp"

namespace GLSL
{
	struct FragmentInput
	{
		GlslWriter_API FragmentInput( GlslWriter & p_writer );
		GlslWriter_API FragmentInput( InVec3 const & p_v3Vertex, InVec3 const & p_v3Normal );
		InVec3 m_v3Vertex;
		InVec3 m_v3Normal;
	};

	struct OutputComponents
	{
		GlslWriter_API OutputComponents( GlslWriter & p_writer );
		GlslWriter_API OutputComponents( InOutVec3 const & p_v3Diffuse, InOutVec3 const & p_v3Specular );
		InOutVec3 m_v3Diffuse;
		InOutVec3 m_v3Specular;
	};

	GlslWriter_API Castor::String ParamToString( Castor::String & p_sep, FragmentInput const & p_value );
	GlslWriter_API Castor::String ParamToString( Castor::String & p_sep, OutputComponents const & p_value );

	GlslWriter_API Castor::String ToString( FragmentInput const & p_value );
	GlslWriter_API Castor::String ToString( OutputComponents const & p_value );

	class LightingModel
	{
	public:
		GlslWriter_API LightingModel( ShadowType p_shadows, GlslWriter & p_writer );
		GlslWriter_API void DeclareModel();
		GlslWriter_API void DeclareDirectionalModel();
		GlslWriter_API void DeclarePointModel();
		GlslWriter_API void DeclareSpotModel();
		// Calls
		GlslWriter_API DirectionalLight GetDirectionalLight( Type const & p_value );
		GlslWriter_API PointLight GetPointLight( Type const & p_value );
		GlslWriter_API SpotLight GetSpotLight( Type const & p_value );

	protected:
		GlslWriter_API Light GetBaseLight( Type const & p_value );
		GlslWriter_API void Declare_Light();
		GlslWriter_API void Declare_DirectionalLight();
		GlslWriter_API void Declare_PointLight();
		GlslWriter_API void Declare_SpotLight();
		GlslWriter_API void Declare_GetBaseLight();
		GlslWriter_API void Declare_GetDirectionalLight();
		GlslWriter_API void Declare_GetPointLight();
		GlslWriter_API void Declare_GetSpotLight();

		virtual void DoDeclareModel() = 0;
		virtual void Declare_ComputeDirectionalLight() = 0;
		virtual void Declare_ComputePointLight() = 0;
		virtual void Declare_ComputeSpotLight() = 0;
		virtual void Declare_ComputeOneDirectionalLight() = 0;
		virtual void Declare_ComputeOnePointLight() = 0;
		virtual void Declare_ComputeOneSpotLight() = 0;

	protected:
		ShadowType m_shadows;
		GlslWriter & m_writer;
		Shadow m_shadowModel;
	};
}

#endif
