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
#ifndef ___C3D_SHADOW_MAP_PASS_H___
#define ___C3D_SHADOW_MAP_PASS_H___

#include "Mesh/Buffer/BufferDeclaration.hpp"
#include "Render/RenderPass.hpp"
#include "Render/Viewport.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Texture/TextureUnit.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		30/08/2016
	\~english
	\brief		Shadow mapping implementation.
	\~french
	\brief		Implémentation du mappage d'ombres.
	*/
	class ShadowMapPass
		: public RenderPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\param[in]	p_scene		The scene.
		 *\param[in]	p_light		The light.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 *\param[in]	p_scene		La scène.
		 *\param[in]	p_light		La source lumineuse.
		 */
		C3D_API ShadowMapPass( Engine & p_engine, Scene & p_scene, Light & p_light, TextureUnit & p_shadowMap, uint32_t p_index );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMapPass();
		/**
		 *\~english
		 *\brief		Initialises the pipeline, FBO and program.
		 *\param		p_size	The FBO dimensions.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Initialise le pipeline, le FBO et le programme.
		 *\param		p_size	Les dimensions du FBO.
		 *\return		\p true if ok.
		 */
		C3D_API bool Initialise( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Update function.
		 *\remarks		Updates the scenes render nodes, if needed.
		 *\~french
		 *\brief		Fonction de mise à jour.
		 *\remarks		Met les noeuds de scènes à jour, si nécessaire.
		 */
		C3D_API void Update();
		/**
		 *\~english
		 *\brief		Render function
		 *\param[in]	p_scene		The scene to render.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[in]	p_scene		La scène à dessiner.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 */
		C3D_API void Render();
		/**
		 *\copydoc		Castor3D::RenderPass::CreateAnimatedNode
		 */
		C3D_API AnimatedGeometryRenderNode CreateAnimatedNode(
			Pass & p_pass,
			RenderPipeline & p_pipeline,
			Submesh & p_submesh,
			Geometry & p_primitive,
			AnimatedSkeletonSPtr p_skeleton,
			AnimatedMeshSPtr p_mesh )override;
		/**
		 *\copydoc		Castor3D::RenderPass::CreateStaticNode
		 */
		C3D_API StaticGeometryRenderNode CreateStaticNode(
			Pass & p_pass,
			RenderPipeline & p_pipeline,
			Submesh & p_submesh,
			Geometry & p_primitive )override;
		/**
		 *\copydoc		Castor3D::RenderPass::CreateBillboardNode
		 */
		C3D_API BillboardRenderNode CreateBillboardNode(
			Pass & p_pass,
			RenderPipeline & p_pipeline,
			BillboardBase & p_billboard )override;
		/**
		 *\~english
		 *\return		The shadow map.
		 *\~english
		 *\return		La map d'ombres.
		 */
		inline TextureUnit & GetShadowMap()
		{
			return m_shadowMap;
		}

	protected:
		void DoRenderOpaqueNodes( SceneRenderNodes & p_nodes, Camera const & p_camera );
		void DoRenderTransparentNodes( SceneRenderNodes & p_nodes, Camera const & p_camera );

	private:
		/**
		 *\~english
		 *\brief		Initialises the pipeline, FBO and program.
		 *\param		p_size	The FBO dimensions.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Initialise le pipeline, le FBO et le programme.
		 *\param		p_size	Les dimensions du FBO.
		 *\return		\p true if ok.
		 */
		C3D_API virtual bool DoInitialise( Castor::Size const & p_size ) = 0;
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API virtual void DoCleanup() = 0;
		/**
		 *\~english
		 *\brief		Update function.
		 *\remarks		Updates the scenes render nodes, if needed.
		 *\~french
		 *\brief		Fonction de mise à jour.
		 *\remarks		Met les noeuds de scènes à jour, si nécessaire.
		 */
		C3D_API virtual void DoUpdate() = 0;
		/**
		 *\~english
		 *\brief		Render function
		 *\param[in]	p_scene		The scene to render.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[in]	p_scene		La scène à dessiner.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 */
		C3D_API virtual void DoRender() = 0;
		/**
		 *\~english
		 *\brief		Updates the program, to add variables to it.
		 *\param[in]	p_program	The program to update.
		 *\~french
		 *\brief		Met à jour le programme, en lui ajoutant des variables.
		 *\param[in]	p_program	Le programme à mettre à jour.
		 */
		C3D_API virtual void DoUpdateProgram( ShaderProgram & p_program ) = 0;
		/**
		 *\copydoc		Castor3D::RenderPass::DoRenderStaticSubmeshesNonInstanced
		 */
		void DoRenderInstancedSubmeshesInstanced( Scene & p_scene, Camera const & p_camera, SubmeshStaticRenderNodesByPipelineMap & p_nodes );
		/**
		 *\copydoc		Castor3D::RenderPass::DoRenderStaticSubmeshesNonInstanced
		 */
		void DoRenderStaticSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, StaticGeometryRenderNodesByPipelineMap & p_nodes );
		/**
		 *\copydoc		Castor3D::RenderPass::DoRenderAnimatedSubmeshesNonInstanced
		 */
		void DoRenderAnimatedSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, AnimatedGeometryRenderNodesByPipelineMap & p_nodes );
		/**
		 *\copydoc		Castor3D::RenderPass::DoRenderBillboards
		 */
		void DoRenderBillboards( Scene & p_scene, Camera const & p_camera, BillboardRenderNodesByPipelineMap & p_nodes );
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetTransparentPixelShaderSource
		 */
		Castor::String DoGetTransparentPixelShaderSource(
			Castor::FlagCombination< TextureChannel > const & p_textureFlags,
			Castor::FlagCombination< ProgramFlag > const & p_programFlags,
			uint8_t p_sceneFlags )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoUpdateOpaquePipeline
		 */
		void DoUpdateOpaquePipeline( RenderPipeline & p_pipeline, DepthMapArray & p_depthMaps )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoUpdateTransparentPipeline
		 */
		void DoUpdateTransparentPipeline( RenderPipeline & p_pipeline, DepthMapArray & p_depthMaps )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoPrepareOpaqueFrontPipeline
		 */
		void DoPrepareOpaqueFrontPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoPrepareOpaqueBackPipeline
		 */
		void DoPrepareOpaqueBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoPrepareTransparentFrontPipeline
		 */
		void DoPrepareTransparentFrontPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoPrepareTransparentBackPipeline
		 */
		void DoPrepareTransparentBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoCompleteProgramFlags
		 */
		void DoCompleteProgramFlags( Castor::FlagCombination< ProgramFlag > & p_programFlags )const override;

	protected:
		//!\~english	The scene.
		//!\~french		La scène.
		Scene & m_scene;
		//!\~english	The light used to generate the shadows.
		//!\~french		La lumière utilisée pour générer les ombres.
		Light & m_light;
		//!\~english	The shadow map texture.
		//!\~french		La texture de mappage d'ombres.
		TextureUnit & m_shadowMap;
		//!\~english	The shadow map layer index.
		//!\~french		L'index dans la texture mappage d'ombres.
		uint32_t m_index;
		//!\~english	The frame buffer.
		//!\~french		Le tampon d'image.
		FrameBufferSPtr m_frameBuffer;
		//!\~english	The geometry buffer.
		//!\~french		Les tampons de géométrie.
		std::set< GeometryBuffersSPtr > m_geometryBuffers;
	};
}

#endif
