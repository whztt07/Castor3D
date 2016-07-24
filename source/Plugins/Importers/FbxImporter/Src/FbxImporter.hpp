/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_FBX_IMPORTER_H___
#define ___C3D_FBX_IMPORTER_H___

#include <Castor3DPrerequisites.hpp>

#include <FileParser/FileParser.hpp>

#include <Engine.hpp>

#include <Animation/Animation.hpp>
#include <Animation/KeyFrame.hpp>
#include <Material/Material.hpp>
#include <Material/Pass.hpp>
#include <Mesh/Face.hpp>
#include <Mesh/Importer.hpp>
#include <Mesh/Mesh.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Vertex.hpp>
#include <Mesh/Buffer/Buffer.hpp>
#include <Mesh/Skeleton/Skeleton.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Miscellaneous/Version.hpp>
#include <Plugin/Plugin.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/Viewport.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/SceneNode.hpp>
#include <Scene/Scene.hpp>
#include <Texture/TextureUnit.hpp>
#include <Texture/TextureLayout.hpp>

#include <fbxsdk.h>

namespace C3dFbx
{
	/*!
	\author		Sylvain DOREMUS
	\date		02/02/2016
	\~english
	\brief		FBX file importer.
	\~french
	\brief		Importeur de fichiers FBX.
	*/
	class FbxSdkImporter
		: public Castor3D::Importer
	{
	public:
		FbxSdkImporter( Castor3D::Engine & p_engine );
		~FbxSdkImporter();

		static Castor3D::ImporterUPtr Create( Castor3D::Engine & p_engine );

	private:
		/**
		 *\copydoc		Castor3D::Importer::DoImportScene
		 */
		bool DoImportScene( Castor3D::Scene & p_scene )override;
		/**
		 *\copydoc		Castor3D::Importer::DoImportMesh
		 */
		bool DoImportMesh( Castor3D::Mesh & p_mesh )override;

		void DoLoadMeshes( Castor3D::Mesh & p_mesh, Castor3D::Scene & p_scene, FbxNode * p_fbxNode );
		void DoLoadSkeleton( Castor3D::Mesh & p_mesh, FbxNode * p_fbxScene );
		void DoLoadMaterials( Castor3D::Scene & p_scene, FbxScene * p_fbxScene );
		void DoLoadAnimations( Castor3D::Mesh & p_mesh, FbxScene * p_fbxScene );
		void DoProcessBones( FbxNode * p_fbxNode, Castor3D::Skeleton & p_skeleton, Castor3D::BoneSPtr p_parent );
		void DoProcessBonesWeights( FbxNode * p_fbxNode, Castor3D::Skeleton const & p_skeleton, std::vector< Castor3D::VertexBoneData > & p_boneData );
		void DoProcessSkeletonAnimations( FbxScene * p_fbxScene, FbxNode * p_fbxNode, FbxSkin * p_fbxSkin, Castor3D::Skeleton & p_skeleton );
		Castor3D::SubmeshSPtr DoProcessMesh( Castor3D::Mesh & p_mesh, FbxMesh * p_fbxMesh );
		Castor3D::TextureUnitSPtr DoLoadTexture( FbxFileTexture * p_fbxTexture, Castor3D::Pass & p_pass, Castor3D::TextureChannel p_channel );
		Castor3D::TextureUnitSPtr DoLoadTexture( FbxLayeredTexture * p_fbxTexture, Castor3D::Pass & p_pass, Castor3D::TextureChannel p_channel );
		Castor3D::TextureUnitSPtr DoLoadTexture( FbxProceduralTexture * p_fbxTexture, Castor3D::Pass & p_pass, Castor3D::TextureChannel p_channel );
		Castor3D::TextureUnitSPtr DoLoadTexture( Castor3D::Scene & p_scene, FbxPropertyT< FbxDouble3 > const & p_property, Castor3D::Pass & p_pass, Castor3D::TextureChannel p_channel );

	private:
		int m_anonymous;
		std::map< Castor::String, uint32_t > m_mapBoneByID;
		std::vector< Castor3D::BoneSPtr > m_arrayBones;
	};
}

#endif
