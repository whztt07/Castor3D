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
#ifndef ___GLSL_CookTorranceLighting_H___
#define ___GLSL_CookTorranceLighting_H___

#include "GlslLighting.hpp"

namespace GLSL
{
	class CookTorranceLightingModel
		: public LightingModel
	{
	public:
		GlslWriter_API CookTorranceLightingModel( ShadowType p_shadows, GlslWriter & p_writer );
		GlslWriter_API static std::shared_ptr< LightingModel > Create( ShadowType p_shadows, GlslWriter & p_writer );
		GlslWriter_API void ComputeCombinedLighting( Vec3 const & p_worldEye
			, Vec3 const & p_albedo
			, Float const & p_metallic
			, Float const & p_roughness
			, Int const & p_receivesShadows
			, FragmentInput const & p_fragmentIn
			, OutputComponents & p_output );
		GlslWriter_API void ComputeDirectionalLight( DirectionalLight const & p_light
			, Vec3 const & p_worldEye
			, Vec3 const & p_albedo
			, Float const & p_metallic
			, Float const & p_roughness
			, Int const & p_receivesShadows
			, FragmentInput const & p_fragmentIn
			, OutputComponents & p_output );
		GlslWriter_API void ComputePointLight( PointLight const & p_light
			, Vec3 const & p_worldEye
			, Vec3 const & p_albedo
			, Float const & p_metallic
			, Float const & p_roughness
			, Int const & p_receivesShadows
			, FragmentInput const & p_fragmentIn
			, OutputComponents & p_output );
		GlslWriter_API void ComputeSpotLight( SpotLight const & p_light
			, Vec3 const & p_worldEye
			, Vec3 const & p_albedo
			, Float const & p_metallic
			, Float const & p_roughness
			, Int const & p_receivesShadows
			, FragmentInput const & p_fragmentIn
			, OutputComponents & p_output );
		GlslWriter_API void ComputeOneDirectionalLight( DirectionalLight const & p_light
			, Vec3 const & p_worldEye
			, Vec3 const & p_albedo
			, Float const & p_metallic
			, Float const & p_roughness
			, Int const & p_receivesShadows
			, FragmentInput const & p_fragmentIn
			, OutputComponents & p_output );
		GlslWriter_API void ComputeOnePointLight( PointLight const & p_light
			, Vec3 const & p_worldEye
			, Vec3 const & p_albedo
			, Float const & p_metallic
			, Float const & p_roughness
			, Int const & p_receivesShadows
			, FragmentInput const & p_fragmentIn
			, OutputComponents & p_output );
		GlslWriter_API void ComputeOneSpotLight( SpotLight const & p_light
			, Vec3 const & p_worldEye
			, Vec3 const & p_albedo
			, Float const & p_metallic
			, Float const & p_roughness
			, Int const & p_receivesShadows
			, FragmentInput const & p_fragmentIn
			, OutputComponents & p_output );

	protected:
		void DoDeclareModel();
		void Declare_ComputeDirectionalLight()override;
		void Declare_ComputePointLight()override;
		void Declare_ComputeSpotLight()override;
		void Declare_ComputeOneDirectionalLight()override;
		void Declare_ComputeOnePointLight()override;
		void Declare_ComputeOneSpotLight()override;

		void DoComputeLight( Light const & p_light
			, Vec3 const & p_worldEye
			, Vec3 const & p_direction
			, Vec3 const & p_albedo
			, Float const & p_metalness
			, Float const & p_roughness
			, Float const & p_shadowFactor
			, FragmentInput const & p_fragmentIn
			, OutputComponents & p_output );

		void DoDeclare_Distribution();
		void DoDeclare_Geometry();
		void DoDeclare_FresnelShlick();
		void DoDeclare_ComputeLight();

	public:
		GlslWriter_API static const Castor::String Name;
		Function< Float, InFloat, InFloat > m_distributionGGX;
		Function< Float, InFloat, InFloat > m_geometrySchlickGGX;
		Function< Float, InFloat, InFloat, InFloat > m_geometrySmith;
		Function< Float, InVec3, InVec3, InVec3, InFloat > m_smith;
		Function< Vec3, InFloat, InVec3 > m_schlickFresnel;
		Function< Vec3, InFloat, InFloat, InFloat, InFloat, InVec3 > m_cookTorrance;
		Function< Void, DirectionalLight, InVec3, InVec3, InFloat, InFloat, InInt, FragmentInput, OutputComponents & > m_computeDirectional;
		Function< Void, PointLight, InVec3, InVec3, InFloat, InFloat, InInt, FragmentInput, OutputComponents & > m_computePoint;
		Function< Void, SpotLight, InVec3, InVec3, InFloat, InFloat, InInt, FragmentInput, OutputComponents & > m_computeSpot;
		Function< Void, DirectionalLight, InVec3, InVec3, InFloat, InFloat, InInt, FragmentInput, OutputComponents & > m_computeOneDirectional;
		Function< Void, PointLight, InVec3, InVec3, InFloat, InFloat, InInt, FragmentInput, OutputComponents & > m_computeOnePoint;
		Function< Void, SpotLight, InVec3, InVec3, InFloat, InFloat, InInt, FragmentInput, OutputComponents & > m_computeOneSpot;
	};
}

#endif