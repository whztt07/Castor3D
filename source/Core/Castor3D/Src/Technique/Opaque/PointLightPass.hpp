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
#ifndef ___C3D_DeferredPointLightPass_H___
#define ___C3D_DeferredPointLightPass_H___

#include "MeshLightPass.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Point light pass.
	\~french
	\brief		Passe de lumière omnidirectionnelle.
	*/
	class PointLightPass
		: public MeshLightPass
	{
	protected:
		/*!
		\author		Sylvain DOREMUS
		\version	0.10.0
		\date		08/06/2017
		\~english
		\brief		Point light pass program.
		\~french
		\brief		Programme de passe de lumière omnidirectionnelle.
		*/
		struct Program
			: public MeshLightPass::Program
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\param[in]	p_engine	The engine.
			 *\param[in]	p_vtx		The vertex shader source.
			 *\param[in]	p_pxl		The fragment shader source.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	p_engine	Le moteur.
			 *\param[in]	p_vtx		Le source du vertex shader.
			 *\param[in]	p_pxl		Le source du fagment shader.
			 */
			Program( Engine & p_engine
				, GLSL::Shader const & p_vtx
				, GLSL::Shader const & p_pxl );
			/**
			 *\~english
			 *\brief		Destructor.
			 *\~french
			 *\brief		Destructeur.
			 */
			virtual ~Program();

		private:
			/**
			 *\copydoc		Castor3D::LightPass::Program::DoBind
			 */
			void DoBind( Light const & p_light )override;

		private:
			//!\~english	The variable containing the light position.
			//!\~french		La variable contenant la position de la lumière.
			PushUniform3fSPtr m_lightPosition;
			//!\~english	The variable containing the light attenuation.
			//!\~french		La variable contenant l'atténuation de la lumière.
			PushUniform3fSPtr m_lightAttenuation;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine		The engine.
		 *\param[in]	p_frameBuffer	The target framebuffer.
		 *\param[in]	p_depthAttach	The depth buffer attach.
		 *\param[in]	p_shadows		Tells if shadows are enabled for this light pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine		Le moteur.
		 *\param[in]	p_frameBuffer	Le tampon d'image cible.
		 *\param[in]	p_depthAttach	L'attache du tampon de profondeur.
		 *\param[in]	p_shadows		Dit si les ombres sont activées pour cette passe d'éclairage.
		 */
		PointLightPass( Engine & p_engine
			, FrameBuffer & p_frameBuffer
			, FrameBufferAttachment & p_depthAttach
			, bool p_shadows );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~PointLightPass();

	private:
		/**
		 *\copydoc		Castor3D::LightPass::DoCreateProgram
		 */
		LightPass::ProgramPtr DoCreateProgram( GLSL::Shader const & p_vtx
			, GLSL::Shader const & p_pxl )const override;
		/**
		 *\copydoc		Castor3D::MeshLightPass::DoGenerateVertices
		 */
		Castor::Point3fArray DoGenerateVertices()const override;
		/**
		 *\copydoc		Castor3D::MeshLightPass::DoGenerateFaces
		 */
		UIntArray DoGenerateFaces()const override;
		/**
		 *\copydoc		Castor3D::MeshLightPass::DoComputeModelMatrix
		 */
		Castor::Matrix4x4r DoComputeModelMatrix( Light const & p_light
			, Camera const & p_camera )const override;
	};
}

#endif
