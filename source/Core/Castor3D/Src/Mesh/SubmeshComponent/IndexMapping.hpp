/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FaceMapping_H___
#define ___C3D_FaceMapping_H___

#include "SubmeshComponent.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\date		11/11/2017
	\~english
	\brief		A submesh component for faces.
	\~french
	\brief		Un composant de sous-maillage pour les faces.
	*/
	class IndexMapping
		: public SubmeshComponent
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	submesh	The parent submesh.
		 *\param[in]	type	The component type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh	Le sous-maillage parent.
		 *\param[in]	type	Le type de composant.
		 */
		C3D_API IndexMapping( Submesh & submesh
			, castor::String const & type );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~IndexMapping();
		/**
		 *\~english
		 *\brief		Sorts the face from farthest to nearest from the camera
		 *\param[in]	p_cameraPosition	The camera position, relative to submesh
		 *\~french
		 *\brief		Trie les faces des plus éloignées aux plus proches de la caméra
		 *\param[in]	p_cameraPosition	La position de la caméra, relative au sous-maillage
		 */
		C3D_API virtual void sortByDistance( castor::Point3r const & p_cameraPosition ) = 0;
		/**
		 *\~english
		 *\brief		Generates normals and tangents
		 *\~french
		 *\brief		Génère les normales et les tangentes
		 */
		C3D_API virtual void computeNormals( bool p_reverted = false ) = 0;
		/**
		 *\copydoc		castor3d::SubmeshComponent::gather
		 */
		inline void gather( VertexBufferArray & buffers )override
		{
		}
		/**
		 *\copydoc		castor3d::SubmeshComponent::getProgramFlags
		 */
		inline ProgramFlags getProgramFlags()const override
		{
			return ProgramFlags{};
		}

	private:
		inline bool doInitialise()override
		{
			return true;
		}

		inline void doCleanup()override
		{
		}

		inline void doUpload()override
		{
		}
	};
}

#endif