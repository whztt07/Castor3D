#include "OpaquePass.hpp"

#include "LightPass.hpp"

#include <Engine.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>
#include <GlslMaterial.hpp>
#include <GlslUtils.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Castor3D
{
	OpaquePass::OpaquePass( Scene & p_scene
		, Camera * p_camera
		, SsaoConfig const & p_config )
		: Castor3D::RenderTechniquePass{ cuT( "deferred_opaque" )
			, p_scene
			, p_camera
			, true
			, false
			, false
			, nullptr
			, p_config }
		, m_directionalShadowMap{ *p_scene.GetEngine() }
		, m_spotShadowMap{ *p_scene.GetEngine() }
		, m_pointShadowMap{ *p_scene.GetEngine() }
	{
	}

	OpaquePass::~OpaquePass()
	{
	}

	void OpaquePass::Render( RenderInfo & p_info, bool p_shadows )
	{
		DoRender( p_info, p_shadows );
	}

	bool OpaquePass::InitialiseShadowMaps()
	{
		m_scene.GetLightCache().ForEach( [this]( Light & p_light )
		{
			if ( p_light.IsShadowProducer() )
			{
				switch ( p_light.GetLightType() )
				{
				case LightType::eDirectional:
					m_directionalShadowMap.AddLight( p_light );
					break;

				case LightType::ePoint:
					m_pointShadowMap.AddLight( p_light );
					break;

				case LightType::eSpot:
					m_spotShadowMap.AddLight( p_light );
					break;
				}
			}
		} );

		bool l_result = m_directionalShadowMap.Initialise();

		if ( l_result )
		{
			l_result = m_spotShadowMap.Initialise();
		}

		if ( l_result )
		{
			l_result = m_pointShadowMap.Initialise();
		}

		ENSURE( l_result );
		return l_result;
	}

	void OpaquePass::CleanupShadowMaps()
	{
		m_pointShadowMap.Cleanup();
		m_spotShadowMap.Cleanup();
		m_directionalShadowMap.Cleanup();
	}

	void OpaquePass::UpdateShadowMaps( RenderQueueArray & p_queues )
	{
		m_pointShadowMap.Update( *m_camera, p_queues );
		m_spotShadowMap.Update( *m_camera, p_queues );
		m_directionalShadowMap.Update( *m_camera, p_queues );
	}

	void OpaquePass::RenderShadowMaps()
	{
	}

	void OpaquePass::DoGetDepthMaps( DepthMapArray & p_depthMaps )
	{
	}

	void OpaquePass::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags
		, SceneFlags & p_sceneFlags )const
	{
		RemFlag( p_programFlags, ProgramFlag::eLighting );
		RemFlag( p_sceneFlags, SceneFlag::eShadowFilterStratifiedPoisson );
	}

	String OpaquePass::DoGetVertexShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, bool p_invertNormals )const
	{
		using namespace GLSL;
		auto l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();
		// Vertex inputs
		auto position = l_writer.DeclAttribute< Vec4 >( ShaderProgram::Position );
		auto normal = l_writer.DeclAttribute< Vec3 >( ShaderProgram::Normal );
		auto tangent = l_writer.DeclAttribute< Vec3 >( ShaderProgram::Tangent );
		auto bitangent = l_writer.DeclAttribute< Vec3 >( ShaderProgram::Bitangent );
		auto texture = l_writer.DeclAttribute< Vec3 >( ShaderProgram::Texture );
		auto bone_ids0 = l_writer.DeclAttribute< IVec4 >( ShaderProgram::BoneIds0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = l_writer.DeclAttribute< IVec4 >( ShaderProgram::BoneIds1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = l_writer.DeclAttribute< Vec4 >( ShaderProgram::Weights0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = l_writer.DeclAttribute< Vec4 >( ShaderProgram::Weights1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto transform = l_writer.DeclAttribute< Mat4 >( ShaderProgram::Transform, CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) );
		auto material = l_writer.DeclAttribute< Int >( ShaderProgram::Material, CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = l_writer.DeclAttribute< Vec4 >( ShaderProgram::Position2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = l_writer.DeclAttribute< Vec3 >( ShaderProgram::Normal2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = l_writer.DeclAttribute< Vec3 >( ShaderProgram::Tangent2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto bitangent2 = l_writer.DeclAttribute< Vec3 >( ShaderProgram::Bitangent2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = l_writer.DeclAttribute< Vec3 >( ShaderProgram::Texture2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( l_writer.DeclBuiltin< Int >( cuT( "gl_InstanceID" ) ) );

		UBO_MATRIX( l_writer );
		UBO_MODEL_MATRIX( l_writer );
		UBO_SKINNING( l_writer, p_programFlags );
		UBO_MORPHING( l_writer, p_programFlags );
		UBO_SCENE( l_writer );
		UBO_MODEL( l_writer );

		// Outputs
		auto vtx_position = l_writer.DeclOutput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_tangentSpaceFragPosition = l_writer.DeclOutput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" ) );
		auto vtx_tangentSpaceViewPosition = l_writer.DeclOutput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" ) );
		auto vtx_normal = l_writer.DeclOutput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = l_writer.DeclOutput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = l_writer.DeclOutput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = l_writer.DeclOutput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = l_writer.DeclOutput< Int >( cuT( "vtx_instance" ) );
		auto vtx_material = l_writer.DeclOutput< Int >( cuT( "vtx_material" ) );
		auto gl_Position = l_writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > l_main = [&]()
		{
			auto l_v4Vertex = l_writer.DeclLocale( cuT( "l_v4Vertex" ), vec4( position.xyz(), 1.0 ) );
			auto l_v4Normal = l_writer.DeclLocale( cuT( "l_v4Normal" ), vec4( normal, 0.0 ) );
			auto l_v4Tangent = l_writer.DeclLocale( cuT( "l_v4Tangent" ), vec4( tangent, 0.0 ) );
			auto l_v3Texture = l_writer.DeclLocale( cuT( "l_v3Texture" ), texture );
			auto l_mtxModel = l_writer.DeclLocale< Mat4 >( cuT( "l_mtxModel" ) );

			if ( CheckFlag( p_programFlags, ProgramFlag::eSkinning ) )
			{
				auto l_mtxBoneTransform = l_writer.DeclLocale< Mat4 >( cuT( "l_mtxBoneTransform" ) );
				l_mtxBoneTransform = c3d_mtxBones[bone_ids0[0_i]] * weights0[0_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[1_i]] * weights0[1_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[2_i]] * weights0[2_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[3_i]] * weights0[3_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[0_i]] * weights1[0_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[1_i]] * weights1[1_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[2_i]] * weights1[2_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[3_i]] * weights1[3_i];
				l_mtxModel = c3d_mtxModel * l_mtxBoneTransform;
				vtx_material = c3d_materialIndex;
			}
			else if ( CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) )
			{
				l_mtxModel = transform;
				vtx_material = material;
			}
			else
			{
				l_mtxModel = c3d_mtxModel;
				vtx_material = c3d_materialIndex;
			}

			if ( CheckFlag( p_programFlags, ProgramFlag::eMorphing ) )
			{
				auto l_time = l_writer.DeclLocale( cuT( "l_time" ), 1.0_f - c3d_fTime );
				l_v4Vertex = vec4( l_v4Vertex.xyz() * l_time + position2.xyz() * c3d_fTime, 1.0 );
				l_v4Normal = vec4( l_v4Normal.xyz() * l_time + normal2.xyz() * c3d_fTime, 1.0 );
				l_v4Tangent = vec4( l_v4Tangent.xyz() * l_time + tangent2.xyz() * c3d_fTime, 1.0 );
				l_v3Texture = l_v3Texture * l_writer.Paren( 1.0_f - c3d_fTime ) + texture2 * c3d_fTime;
			}

			vtx_texture = l_v3Texture;
			l_v4Vertex = l_mtxModel * l_v4Vertex;
			vtx_position = l_v4Vertex.xyz();
			l_v4Vertex = c3d_mtxView * l_v4Vertex;
			auto l_mtxNormal = l_writer.DeclLocale( cuT( "l_mtxNormal" )
				, transpose( inverse( mat3( l_mtxModel ) ) ) );

			if ( p_invertNormals )
			{
				vtx_normal = normalize( l_mtxNormal * -l_v4Normal.xyz() );
			}
			else
			{
				vtx_normal = normalize( l_mtxNormal * l_v4Normal.xyz() );
			}

			vtx_tangent = normalize( l_mtxNormal * l_v4Tangent.xyz() );
			vtx_tangent = normalize( vtx_tangent - vtx_normal * dot( vtx_tangent, vtx_normal ) );
			vtx_bitangent = cross( vtx_normal, vtx_tangent );
			vtx_instance = gl_InstanceID;
			gl_Position = c3d_mtxProjection * l_v4Vertex;

			auto l_tbn = l_writer.DeclLocale( cuT( "l_tbn" ), transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
			vtx_tangentSpaceFragPosition = l_tbn * vtx_position;
			vtx_tangentSpaceViewPosition = l_tbn * c3d_v3CameraPosition;
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}

	String OpaquePass::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// UBOs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_MODEL( l_writer );

		// Fragment Inputs
		auto vtx_position = l_writer.DeclInput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_tangentSpaceFragPosition = l_writer.DeclInput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" ) );
		auto vtx_tangentSpaceViewPosition = l_writer.DeclInput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" ) );
		auto vtx_normal = l_writer.DeclInput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = l_writer.DeclInput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = l_writer.DeclInput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = l_writer.DeclInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = l_writer.DeclInput< Int >( cuT( "vtx_instance" ) );
		auto vtx_material = l_writer.DeclInput< Int >( cuT( "vtx_material" ) );

		LegacyMaterials l_materials{ l_writer };
		l_materials.Declare();

		auto c3d_mapDiffuse( l_writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse, CheckFlag( p_textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapNormal( l_writer.DeclUniform< Sampler2D >( ShaderProgram::MapNormal, CheckFlag( p_textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapSpecular( l_writer.DeclUniform< Sampler2D >( ShaderProgram::MapSpecular, CheckFlag( p_textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapEmissive( l_writer.DeclUniform< Sampler2D >( ShaderProgram::MapEmissive, CheckFlag( p_textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapHeight( l_writer.DeclUniform< Sampler2D >( ShaderProgram::MapHeight, CheckFlag( p_textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapGloss( l_writer.DeclUniform< Sampler2D >( ShaderProgram::MapGloss, CheckFlag( p_textureFlags, TextureChannel::eGloss ) ) );
		auto c3d_mapEnvironment( l_writer.DeclUniform< SamplerCube >( ShaderProgram::MapEnvironment, CheckFlag( p_textureFlags, TextureChannel::eReflection ) ) );

		auto c3d_fheightScale( l_writer.DeclUniform< Float >( cuT( "c3d_fheightScale" ), CheckFlag( p_textureFlags, TextureChannel::eHeight ), 0.1_f ) );

		auto gl_FragCoord( l_writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		uint32_t l_index = 0;
		auto out_c3dNormal = l_writer.DeclFragData< Vec4 >( cuT( "out_c3dNormal" ), l_index++ );
		auto out_c3dDiffuse = l_writer.DeclFragData< Vec4 >( cuT( "out_c3dDiffuse" ), l_index++ );
		auto out_c3dSpecular = l_writer.DeclFragData< Vec4 >( cuT( "out_c3dSpecular" ), l_index++ );
		auto out_c3dEmissive = l_writer.DeclFragData< Vec4 >( cuT( "out_c3dEmissive" ), l_index++ );

		auto l_parallaxMapping = DeclareParallaxMappingFunc( l_writer, p_textureFlags, p_programFlags );
		Declare_EncodeMaterial( l_writer );
		GLSL::Utils l_utils{ l_writer };
		l_utils.DeclareRemoveGamma();

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto l_v3Normal = l_writer.DeclLocale( cuT( "l_v3Normal" ), normalize( vtx_normal ) );
			auto l_v3Diffuse = l_writer.DeclLocale( cuT( "l_v3Diffuse" ), l_materials.GetDiffuse( vtx_material ) );
			auto l_v3Specular = l_writer.DeclLocale( cuT( "l_v3Specular" ), l_materials.GetSpecular( vtx_material ) );
			auto l_fMatShininess = l_writer.DeclLocale( cuT( "l_fMatShininess" ), l_materials.GetShininess( vtx_material ) );
			auto l_v3Emissive = l_writer.DeclLocale( cuT( "l_v3Emissive" ), l_v3Diffuse * l_materials.GetEmissive( vtx_material ) );
			auto l_gamma = l_writer.DeclLocale( cuT( "l_gamma" ), l_materials.GetGamma( vtx_material ) );
			auto l_texCoord = l_writer.DeclLocale( cuT( "l_texCoord" ), vtx_texture );

			if ( CheckFlag( p_textureFlags, TextureChannel::eHeight )
				&& CheckFlag( p_textureFlags, TextureChannel::eNormal ) )
			{
				auto l_viewDir = -l_writer.DeclLocale( cuT( "l_viewDir" ), normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				l_texCoord.xy() = l_parallaxMapping( l_texCoord.xy(), l_viewDir );
			}

			ComputePreLightingMapContributions( l_writer
				, l_v3Normal
				, l_fMatShininess
				, p_textureFlags
				, p_programFlags
				, p_sceneFlags );
			ComputePostLightingMapContributions( l_writer
				, l_v3Diffuse
				, l_v3Specular
				, l_v3Emissive
				, l_gamma
				, p_textureFlags
				, p_programFlags
				, p_sceneFlags );
			auto l_flags = l_writer.DeclLocale( cuT( "l_flags" ), 0.0_f );
			EncodeMaterial( l_writer
				, c3d_shadowReceiver
				, CheckFlag( p_textureFlags, TextureChannel::eReflection ) ? 1_i : 0_i
				, CheckFlag( p_textureFlags, TextureChannel::eRefraction ) ? 1_i : 0_i
				, c3d_envMapIndex
				, l_flags );

			out_c3dNormal = vec4( l_v3Normal, l_flags );
			out_c3dDiffuse = vec4( l_v3Diffuse, 0.0_f );
			out_c3dSpecular = vec4( l_v3Specular, l_fMatShininess );
			out_c3dEmissive = vec4( l_v3Emissive, l_materials.GetRefractionRatio( vtx_material ) );
		} );

		return l_writer.Finalise();
	}

	void OpaquePass::DoUpdatePipeline( RenderPipeline & p_pipeline )const
	{
		auto & l_scene = *m_camera->GetScene();
		m_sceneUbo.Update( l_scene, *m_camera, false );
	}
}