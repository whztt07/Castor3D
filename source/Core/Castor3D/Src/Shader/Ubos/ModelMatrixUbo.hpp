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
#ifndef ___C3D_ModelMatrixUbo_H___
#define ___C3D_ModelMatrixUbo_H___

#include "Shader/UniformBuffer.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		12/04/2017
	\~english
	\brief		Matrices Uniform buffer management.
	\~french
	\brief		Gestion du tampon de variables uniformes pour les matrices.
	*/
	class ModelMatrixUbo
	{
	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API ModelMatrixUbo( ModelMatrixUbo const & ) = delete;
		C3D_API ModelMatrixUbo & operator=( ModelMatrixUbo const & ) = delete;
		C3D_API ModelMatrixUbo( ModelMatrixUbo && ) = default;
		C3D_API ModelMatrixUbo & operator=( ModelMatrixUbo && ) = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API ModelMatrixUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~ModelMatrixUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	p_model			The new model matrix.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	p_model			La nouvelle matrice modèle.
		 */
		C3D_API void update( castor::Matrix4x4r const & p_model )const;
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	p_model			The new model matrix.
		 *\param[in]	p_projection	The new normal matrix.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	p_model			La nouvelle matrice modèle.
		 *\param[in]	p_projection	La nouvelle matrice normale.
		 */
		C3D_API void update( castor::Matrix4x4r const & p_model
			, castor::Matrix4x4r const & p_normal )const;
		/**
		 *\~english
		 *\name			getters.
		 *\~french
		 *\name			getters.
		 */
		inline UniformBuffer & getUbo()
		{
			return m_ubo;
		}

		inline UniformBuffer const & getUbo()const
		{
			return m_ubo;
		}
		/**@}*/

	public:
		static constexpr uint32_t BindingPoint = 2u;
		C3D_API static castor::String const BufferModelMatrix;
		C3D_API static castor::String const MtxModel;
		C3D_API static castor::String const MtxNormal;

	private:
		//!\~english	The UBO.
		//!\~french		L'UBO.
		UniformBuffer m_ubo;
		//!\~english	The view matrix variable.
		//!\~french		La variable de la matrice vue.
		Uniform4x4f & m_model;
		//!\~english	The projection matrix variable.
		//!\~french		La variable de la matrice projection.
		Uniform4x4f & m_normal;
	};
}

#define UBO_MODEL_MATRIX( Writer )\
	glsl::Ubo modelMatrices{ writer, castor3d::ModelMatrixUbo::BufferModelMatrix, castor3d::ModelMatrixUbo::BindingPoint };\
	auto c3d_mtxModel = modelMatrices.declMember< glsl::Mat4 >( castor3d::ModelMatrixUbo::MtxModel );\
	auto c3d_mtxNormal = modelMatrices.declMember< glsl::Mat4 >( castor3d::ModelMatrixUbo::MtxNormal );\
	modelMatrices.end()

#endif