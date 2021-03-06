/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MatrixUbo_H___
#define ___C3D_MatrixUbo_H___

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
	class MatrixUbo
	{
	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API MatrixUbo( MatrixUbo const & ) = delete;
		C3D_API MatrixUbo & operator=( MatrixUbo const & ) = delete;
		C3D_API MatrixUbo( MatrixUbo && ) = default;
		C3D_API MatrixUbo & operator=( MatrixUbo && ) = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit MatrixUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~MatrixUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	view		The new view matrix.
		 *\param[in]	projection	The new projection matrix.
		 *\param[in]	jitter		The jittering value.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	view		La nouvelle matrice de vue.
		 *\param[in]	projection	La nouvelle matrice de projection.
		 *\param[in]	jitter		La valeur de jittering.
		 */
		C3D_API void update( castor::Matrix4x4r const & view
			, castor::Matrix4x4r const & projection
			, castor::Point2r const & jitter = castor::Point2r{} )const;
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\remarks		View matrix won't be updated.
		 *\param[in]	projection	The new projection matrix.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\remarks		La matrice de vue ne sera pas mise à jour.
		 *\param[in]	projection	La nouvelle matrice de projection.
		 */
		C3D_API void update( castor::Matrix4x4r const & projection )const;
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
		static uint32_t constexpr BindingPoint = 1u;
		C3D_API static castor::String const BufferMatrix;
		C3D_API static castor::String const Projection;
		C3D_API static castor::String const CurView;
		C3D_API static castor::String const PrvView;
		C3D_API static castor::String const CurViewProj;
		C3D_API static castor::String const PrvViewProj;
		C3D_API static castor::String const InvProjection;
		C3D_API static castor::String const CurJitter;
		C3D_API static castor::String const PrvJitter;

	private:
		//!\~english	The UBO.
		//!\~french		L'UBO.
		UniformBuffer m_ubo;
		//!\~english	The projection matrix variable.
		//!\~french		La variable de la matrice projection.
		Uniform4x4f & m_projection;
		//!\~english	The inverse projection matrix variable.
		//!\~french		La variable de la matrice projection inverse.
		Uniform4x4f & m_invProjection;
		//!\~english	The current view matrix variable.
		//!\~french		La variable de la matrice vue courante.
		Uniform4x4f & m_curView;
		//!\~english	The previous view matrix variable.
		//!\~french		La variable de la matrice vue précédente.
		Uniform4x4f & m_prvView;
		//!\~english	The current view projection matrix variable.
		//!\~french		La variable de la matrice vue projection courante.
		Uniform4x4f & m_curViewProj;
		//!\~english	The previous view projection matrix variable.
		//!\~french		La variable de la matrice vue projection précédente.
		Uniform4x4f & m_prvViewProj;
		//!\~english	The current jittering value variable.
		//!\~french		La variable de la valeur de jittering courante.
		Uniform2f & m_curJitter;
		//!\~english	The previous jittering value variable.
		//!\~french		La variable de la valeur de jittering précédente.
		Uniform2f & m_prvJitter;
	};
}

#define UBO_MATRIX( Writer )\
	glsl::Ubo matrices{ writer, castor3d::MatrixUbo::BufferMatrix, castor3d::MatrixUbo::BindingPoint };\
	auto c3d_projection = matrices.declMember< glsl::Mat4 >( castor3d::MatrixUbo::Projection );\
	auto c3d_invProjection = matrices.declMember< glsl::Mat4 >( castor3d::MatrixUbo::InvProjection );\
	auto c3d_curView = matrices.declMember< glsl::Mat4 >( castor3d::MatrixUbo::CurView );\
	auto c3d_prvView = matrices.declMember< glsl::Mat4 >( castor3d::MatrixUbo::PrvView );\
	auto c3d_curViewProj = matrices.declMember< glsl::Mat4 >( castor3d::MatrixUbo::CurViewProj );\
	auto c3d_prvViewProj = matrices.declMember< glsl::Mat4 >( castor3d::MatrixUbo::PrvViewProj );\
	auto c3d_curJitter = matrices.declMember< glsl::Vec2 >( castor3d::MatrixUbo::CurJitter );\
	auto c3d_prvJitter = matrices.declMember< glsl::Vec2 >( castor3d::MatrixUbo::PrvJitter );\
	matrices.end()

#endif
