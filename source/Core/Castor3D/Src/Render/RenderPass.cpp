#include "RenderPass.hpp"

#include "Engine.hpp"

#include "Material/Pass.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/Pipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneNode.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	RenderPass::RenderPass( String const & p_name, Engine & p_engine, bool p_multisampling )
		: OwnedBy< Engine >{ p_engine }
		, Named{ p_name }
		, m_renderSystem{ *p_engine.GetRenderSystem() }
		, m_multisampling{ p_multisampling }
		, m_renderQueue{ *this }
	{
	}

	RenderPass::~RenderPass()
	{
	}

	String RenderPass::GetVertexShaderSource( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags, bool p_invertNormals )const
	{
		return DoGetVertexShaderSource( p_textureFlags, p_programFlags, p_sceneFlags, p_invertNormals );
	}

	String RenderPass::GetPixelShaderSource( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )const
	{
		String l_return;

		if ( CheckFlag( p_programFlags, ProgramFlag::eAlphaBlending ) )
		{
			l_return = DoGetTransparentPixelShaderSource( p_textureFlags, p_programFlags, p_sceneFlags );
		}
		else
		{
			l_return = DoGetOpaquePixelShaderSource( p_textureFlags, p_programFlags, p_sceneFlags );
		}

		return l_return;
	}

	String RenderPass::GetGeometryShaderSource( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )const
	{
		return DoGetGeometryShaderSource( p_textureFlags, p_programFlags, p_sceneFlags );
	}

	void RenderPass::PreparePipeline( BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode, uint16_t p_textureFlags, uint16_t & p_programFlags, uint8_t p_sceneFlags, bool p_twoSided )
	{
		DoCompleteProgramFlags( p_programFlags );
		auto l_backProgram = DoGetProgram( p_textureFlags, p_programFlags, p_sceneFlags, false );

		if ( CheckFlag( p_programFlags, ProgramFlag::eAlphaBlending ) )
		{
			auto l_frontProgram = DoGetProgram( p_textureFlags, p_programFlags, p_sceneFlags, true );
			auto l_flags = PipelineFlags{ p_colourBlendMode, p_alphaBlendMode, p_textureFlags, p_programFlags, p_sceneFlags };
			DoPrepareTransparentFrontPipeline( *l_frontProgram, l_flags );
			DoPrepareTransparentBackPipeline( *l_backProgram, l_flags );
		}
		else
		{
			auto l_flags = PipelineFlags{ p_colourBlendMode, BlendMode::eNoBlend, p_textureFlags, p_programFlags, p_sceneFlags };

			if ( p_twoSided )
			{
				auto l_frontProgram = DoGetProgram( p_textureFlags, p_programFlags, p_sceneFlags, true );
				DoPrepareOpaqueFrontPipeline( *l_frontProgram, l_flags );
			}

			DoPrepareOpaqueBackPipeline( *l_backProgram, l_flags );
		}
	}

	Pipeline * RenderPass::GetOpaquePipelineFront( BlendMode p_colourBlendMode, uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )
	{
		auto l_it = m_frontOpaquePipelines.find( { p_colourBlendMode, BlendMode::eNoBlend, p_textureFlags, p_programFlags, p_sceneFlags } );
		Pipeline * l_return{ nullptr };

		if ( l_it != m_frontOpaquePipelines.end() )
		{
			l_return = l_it->second.get();
		}

		return l_return;
	}

	Pipeline * RenderPass::GetOpaquePipelineBack( BlendMode p_colourBlendMode, uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )
	{
		auto l_it = m_backOpaquePipelines.find( { p_colourBlendMode, BlendMode::eNoBlend, p_textureFlags, p_programFlags, p_sceneFlags } );
		Pipeline * l_return{ nullptr };

		if ( l_it != m_backOpaquePipelines.end() )
		{
			l_return = l_it->second.get();
		}

		return l_return;
	}

	Pipeline * RenderPass::GetTransparentPipelineFront( BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode, uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )
	{
		auto l_it = m_frontTransparentPipelines.find( { p_colourBlendMode, p_alphaBlendMode, p_textureFlags, p_programFlags, p_sceneFlags } );
		Pipeline * l_return{ nullptr };

		if ( l_it != m_frontTransparentPipelines.end() )
		{
			l_return = l_it->second.get();
		}

		return l_return;
	}

	Pipeline * RenderPass::GetTransparentPipelineBack( BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode, uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )
	{
		auto l_it = m_backTransparentPipelines.find( { p_colourBlendMode, p_alphaBlendMode, p_textureFlags, p_programFlags, p_sceneFlags } );
		Pipeline * l_return{ nullptr };

		if ( l_it != m_backTransparentPipelines.end() )
		{
			l_return = l_it->second.get();
		}

		return l_return;
	}

	AnimatedGeometryRenderNode RenderPass::CreateAnimatedNode( Pass & p_pass
															   , Pipeline & p_pipeline
															   , Submesh & p_submesh
															   , Geometry & p_primitive
															   , AnimatedSkeletonSPtr p_skeleton
															   , AnimatedMeshSPtr p_mesh )
	{
		auto l_animationBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferAnimation );

		return AnimatedGeometryRenderNode
		{
			DoCreateSceneRenderNode( *p_primitive.GetScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*p_submesh.GetGeometryBuffers( p_pipeline.GetProgram() ),
			*p_primitive.GetParent(),
			p_submesh,
			p_primitive,
			p_skeleton.get(),
			p_mesh.get(),
			*l_animationBuffer
		};
	}

	StaticGeometryRenderNode RenderPass::CreateStaticNode( Pass & p_pass
														   , Pipeline & p_pipeline
														   , Submesh & p_submesh
														   , Geometry & p_primitive )
	{
		return StaticGeometryRenderNode
		{
			DoCreateSceneRenderNode( *p_primitive.GetScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*p_submesh.GetGeometryBuffers( p_pipeline.GetProgram() ),
			*p_primitive.GetParent(),
			p_submesh,
			p_primitive,
		};
	}

	BillboardRenderNode RenderPass::CreateBillboardNode( Pass & p_pass
														 , Pipeline & p_pipeline
														 , BillboardBase & p_billboard )
	{
		auto l_billboardBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferBillboards );
		Point2iFrameVariableSPtr l_pt2i;

		return BillboardRenderNode
		{
			DoCreateSceneRenderNode( p_billboard.GetParentScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*p_billboard.GetGeometryBuffers( p_pipeline.GetProgram() ),
			*p_billboard.GetNode(),
			p_billboard,
			*l_billboardBuffer,
			*l_billboardBuffer->GetVariable( ShaderProgram::Dimensions, l_pt2i ),
			*l_billboardBuffer->GetVariable( ShaderProgram::WindowSize, l_pt2i )
		};
	}

	void RenderPass::UpdateOpaquePipeline( Camera const & p_camera, Pipeline & p_pipeline, DepthMapArray & p_depthMaps )const
	{
		p_pipeline.SetProjectionMatrix( p_camera.GetViewport().GetProjection() );
		p_pipeline.SetViewMatrix( p_camera.GetView() );
		DoUpdateOpaquePipeline( p_pipeline, p_depthMaps );
		auto & l_sceneUbo = p_pipeline.GetSceneUbo();
		p_camera.FillShader( l_sceneUbo );
	}

	void RenderPass::UpdateTransparentPipeline( Camera const & p_camera, Pipeline & p_pipeline, DepthMapArray & p_depthMaps )const
	{
		p_pipeline.SetProjectionMatrix( p_camera.GetViewport().GetProjection() );
		p_pipeline.SetViewMatrix( p_camera.GetView() );
		DoUpdateTransparentPipeline( p_pipeline, p_depthMaps );
		auto & l_sceneUbo = p_pipeline.GetSceneUbo();
		p_camera.FillShader( l_sceneUbo );
	}

	PassRenderNode RenderPass::DoCreatePassRenderNode( Pass & p_pass, Pipeline & p_pipeline )
	{
		FrameVariableBufferSPtr l_passBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferPass );
		FrameVariableBufferSPtr l_matrixBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferMatrix );
		FrameVariableBufferSPtr l_animationBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferAnimation );
		Point4rFrameVariableSPtr l_pt4r;
		OneFloatFrameVariableSPtr l_1f;

		auto l_node = PassRenderNode
		{
			p_pass,
			p_pipeline,
			*l_matrixBuffer,
			*l_passBuffer,
			*l_passBuffer->GetVariable( ShaderProgram::MatAmbient, l_pt4r ),
			*l_passBuffer->GetVariable( ShaderProgram::MatDiffuse, l_pt4r ),
			*l_passBuffer->GetVariable( ShaderProgram::MatSpecular, l_pt4r ),
			*l_passBuffer->GetVariable( ShaderProgram::MatEmissive, l_pt4r ),
			*l_passBuffer->GetVariable( ShaderProgram::MatShininess, l_1f ),
			*l_passBuffer->GetVariable( ShaderProgram::MatOpacity, l_1f ),
		};
		p_pass.FillRenderNode( l_node );
		return l_node;
	}

	SceneRenderNode RenderPass::DoCreateSceneRenderNode( Scene & p_scene, Pipeline & p_pipeline )
	{
		FrameVariableBufferSPtr l_sceneBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferScene );
		Point3rFrameVariableSPtr l_pt3r;

		return SceneRenderNode
		{
			*l_sceneBuffer,
			*l_sceneBuffer->GetVariable( ShaderProgram::CameraPos, l_pt3r )
		};
	}

	ShaderProgramSPtr RenderPass::DoGetProgram( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags, bool p_invertNormals )const
	{
		return GetEngine()->GetShaderProgramCache().GetAutomaticProgram( *this, p_textureFlags, p_programFlags, p_sceneFlags, p_invertNormals );
	}

	String RenderPass::DoGetVertexShaderSource( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags, bool p_invertNormals )const
	{
		using namespace GLSL;
		auto l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();
		// Vertex inputs
		auto position = l_writer.GetAttribute< Vec4 >( ShaderProgram::Position );
		auto normal = l_writer.GetAttribute< Vec3 >( ShaderProgram::Normal );
		auto tangent = l_writer.GetAttribute< Vec3 >( ShaderProgram::Tangent );
		auto bitangent = l_writer.GetAttribute< Vec3 >( ShaderProgram::Bitangent );
		auto texture = l_writer.GetAttribute< Vec3 >( ShaderProgram::Texture );
		auto bone_ids0 = l_writer.GetAttribute< IVec4 >( ShaderProgram::BoneIds0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = l_writer.GetAttribute< IVec4 >( ShaderProgram::BoneIds1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Weights0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Weights1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto transform = l_writer.GetAttribute< Mat4 >( ShaderProgram::Transform, CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Position2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = l_writer.GetAttribute< Vec3 >( ShaderProgram::Normal2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = l_writer.GetAttribute< Vec3 >( ShaderProgram::Tangent2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto bitangent2 = l_writer.GetAttribute< Vec3 >( ShaderProgram::Bitangent2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = l_writer.GetAttribute< Vec3 >( ShaderProgram::Texture2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( l_writer.GetBuiltin< Int >( cuT( "gl_InstanceID" ) ) );

		UBO_MATRIX( l_writer );
		UBO_ANIMATION( l_writer, p_programFlags );

		// Outputs
		auto vtx_worldSpacePosition = l_writer.GetOutput< Vec3 >( cuT( "vtx_worldSpacePosition" ) );
		auto vtx_normal = l_writer.GetOutput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = l_writer.GetOutput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = l_writer.GetOutput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = l_writer.GetOutput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = l_writer.GetOutput< Int >( cuT( "vtx_instance" ) );
		auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > l_main = [&]()
		{
			auto l_v4Vertex = l_writer.GetLocale( cuT( "l_v4Vertex" ), vec4( position.xyz(), 1.0 ) );
			auto l_v4Normal = l_writer.GetLocale( cuT( "l_v4Normal" ), vec4( normal, 0.0 ) );
			auto l_v4Tangent = l_writer.GetLocale( cuT( "l_v4Tangent" ), vec4( tangent, 0.0 ) );
			auto l_v4Bitangent = l_writer.GetLocale( cuT( "l_v4Bitangent" ), vec4( bitangent, 0.0 ) );
			auto l_v3Texture = l_writer.GetLocale( cuT( "l_v3Texture" ), texture );
			auto l_mtxModel = l_writer.GetLocale< Mat4 >( cuT( "l_mtxModel" ) );

			if ( CheckFlag( p_programFlags, ProgramFlag::eSkinning ) )
			{
				auto l_mtxBoneTransform = l_writer.GetLocale< Mat4 >( cuT( "l_mtxBoneTransform" ) );
				l_mtxBoneTransform = c3d_mtxBones[bone_ids0[0_i]] * weights0[0_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[1_i]] * weights0[1_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[2_i]] * weights0[2_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[3_i]] * weights0[3_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[0_i]] * weights1[0_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[1_i]] * weights1[1_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[2_i]] * weights1[2_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[3_i]] * weights1[3_i];
				l_mtxModel = c3d_mtxModel * l_mtxBoneTransform;
			}
			else if ( CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) )
			{
				l_mtxModel = transform;
			}
			else
			{
				l_mtxModel = c3d_mtxModel;
			}

			if ( CheckFlag( p_programFlags, ProgramFlag::eMorphing ) )
			{
				auto l_time = l_writer.GetLocale( cuT( "l_time" ), 1.0_f - c3d_fTime );
				l_v4Vertex = vec4( l_v4Vertex.xyz() * l_time + position2.xyz() * c3d_fTime, 1.0 );
				l_v4Normal = vec4( l_v4Normal.xyz() * l_time + normal2.xyz() * c3d_fTime, 1.0 );
				l_v4Tangent = vec4( l_v4Tangent.xyz() * l_time + tangent2.xyz() * c3d_fTime, 1.0 );
				l_v4Bitangent = vec4( l_v4Bitangent.xyz() * l_time + bitangent2.xyz() * c3d_fTime, 1.0 );
				l_v3Texture = l_v3Texture * l_writer.Paren( 1.0_f - c3d_fTime ) + texture2 * c3d_fTime;
			}

			vtx_texture = l_v3Texture;
			l_v4Vertex = l_mtxModel * l_v4Vertex;
			vtx_worldSpacePosition = l_v4Vertex.xyz();
			l_v4Vertex = c3d_mtxView * l_v4Vertex;

			if ( p_invertNormals )
			{
				vtx_normal = normalize( l_writer.Paren( l_mtxModel * -l_v4Normal ).xyz() );
			}
			else
			{
				vtx_normal = normalize( l_writer.Paren( l_mtxModel * l_v4Normal ).xyz() );
			}

			vtx_tangent = normalize( l_writer.Paren( l_mtxModel * l_v4Tangent ).xyz() );
			vtx_bitangent = normalize( l_writer.Paren( l_mtxModel * l_v4Bitangent ).xyz() );
			vtx_instance = gl_InstanceID;
			gl_Position = c3d_mtxProjection * l_v4Vertex;
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}
}
