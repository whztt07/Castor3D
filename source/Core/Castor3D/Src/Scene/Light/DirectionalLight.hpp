﻿/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DIRECTIONAL_LIGHT_H___
#define ___C3D_DIRECTIONAL_LIGHT_H___

#include "Light.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version 	0.1
	\date 		09/02/2010
	\~english
	\brief		Class which represents a Directional Light
	\remark		A directional light is a light which enlights from an infinite point in a given direction
	\~french
	\brief		Classe representant une lumière directionnelle
	\remark		Une lumière directionnelle illumine à partir d'un point infini dans une direction donnée
	*/
	class DirectionalLight
		: public LightCategory
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		DirectionalLight loader
		\~french
		\brief		Loader de DirectionalLight
		*/
		class TextWriter
			: public LightCategory::TextWriter
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API TextWriter( castor::String const & p_tabs, DirectionalLight const * p_category = nullptr );
			/**
			 *\~english
			 *\brief		Writes a light into a text file
			 *\param[in]	p_file	The file to save the cameras in
			 *\param[in]	p_light	The light to save
			 *\~french
			 *\brief		Ecrit une lumière dans un fichier texte
			 *\param[in]	p_file	Le fichier
			 *\param[in]	p_light	La lumière
			 */
			C3D_API bool operator()( DirectionalLight const & p_light, castor::TextFile & p_file );
			/**
			 *\copydoc		castor3d::LightCategory::TextWriter::writeInto
			 */
			C3D_API bool writeInto( castor::TextFile & p_file )override;

		private:
			DirectionalLight const * m_category;
		};

	private:
		friend class Scene;

	private:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_light	The parent Light.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_light	La Light parente.
		 */
		C3D_API explicit DirectionalLight( Light & p_light );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~DirectionalLight();
		/**
		 *\~english
		 *\brief		Creation function, used by Factory.
		 *\return		A light source.
		 *\~french
		 *\brief		Fonction de création utilisée par Factory.
		 *\return		Une source lumineuse.
		 */
		C3D_API static LightCategoryUPtr create( Light & p_light );
		/**
		 *\copydoc		castor3d::LightCategory::Update
		 */
		C3D_API void update( castor::Point3r const & p_target
			, Viewport & p_viewport
			, int32_t p_index = -1 )override;
		/**
		 *\copydoc		castor3d::LightCategory::createTextWriter
		 */
		C3D_API std::unique_ptr < LightCategory::TextWriter > createTextWriter( castor::String const & p_tabs )override
		{
			return std::make_unique< TextWriter >( p_tabs, this );
		}
		/**
		 *\~english
		 *\return		The light source direction.
		 *\~french
		 *\return		La direction de la source lumineuse.
		 */
		inline castor::Point3f const & getDirection()const
		{
			return m_direction;
		}
		/**
		 *\~english
		 *\return		The light space transformation matrix.
		 *\~french
		 *\return		La matrice de transformation de la lumière.
		 */
		inline castor::Matrix4x4f const & getLightSpaceTransform()const
		{
			return m_lightSpace;
		}

	private:
		/**
		 *\copydoc		castor3d::LightCategory::updateNode
		 */
		C3D_API void updateNode( SceneNode const & p_node )override;
		/**
		 *\copydoc		castor::LightCategory::doBind
		 */
		C3D_API void doBind( castor::PxBufferBase & p_texture, uint32_t p_index, uint32_t & p_offset )const override;

	private:
		//!\~english	The light source direction.
		//!\~french		La direction de la source lumineuse.
		castor::Point3f m_direction;
		//!\~english	The light source space transformation matrix.
		//!\~french		La matrice de transformation vers l'espace de la source lumineuse.
		mutable castor::Matrix4x4f m_lightSpace;
	};
}

#endif
