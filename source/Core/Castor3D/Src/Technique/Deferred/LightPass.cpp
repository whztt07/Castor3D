﻿#include "LightPass.hpp"

#include <Engine.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/Viewport.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Scene.hpp>
#include <Shader/ModelMatrixUbo.hpp>
#include <Shader/SceneUbo.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/DepthStencilState.hpp>
#include <State/MultisampleState.hpp>
#include <State/RasteriserState.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>
#include <GlslUtils.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Castor3D
{
	//************************************************************************************************

	namespace
	{
		std::unique_ptr< GLSL::LightingModel > CreateLightingModel( GLSL::GlslWriter & p_writer
			, LightType p_light
			, GLSL::ShadowType p_shadows )
		{
			std::unique_ptr< GLSL::LightingModel > l_result;

			switch ( p_light )
			{
			case LightType::eDirectional:
				{
					l_result = p_writer.CreateDirectionalLightingModel( GLSL::PhongLightingModel::Name
						, p_shadows );
					auto light = p_writer.DeclUniform< GLSL::DirectionalLight >( cuT( "light" ) );
				}
				break;

			case LightType::ePoint:
				{
					l_result = p_writer.CreatePointLightingModel( GLSL::PhongLightingModel::Name
						, p_shadows );
					auto light = p_writer.DeclUniform< GLSL::PointLight >( cuT( "light" ) );
				}
				break;

			case LightType::eSpot:
				{
					l_result = p_writer.CreateSpotLightingModel( GLSL::PhongLightingModel::Name
						, p_shadows );
					auto light = p_writer.DeclUniform< GLSL::SpotLight >( cuT( "light" ) );
				}
				break;

			default:
				FAILURE( "Invalid light type" );
				break;
			}

			return l_result;
		}
	}

	//************************************************************************************************

	String GetTextureName( DsTexture p_texture )
	{
		static std::array< String, size_t( DsTexture::eCount ) > Values
		{
			{
				cuT( "c3d_mapDepth" ),
				cuT( "c3d_mapNormal" ),
				cuT( "c3d_mapDiffuse" ),
				cuT( "c3d_mapSpecular" ),
				cuT( "c3d_mapEmissive" ),
			}
		};

		return Values[size_t( p_texture )];
	}

	PixelFormat GetTextureFormat( DsTexture p_texture )
	{
		static std::array< PixelFormat, size_t( DsTexture::eCount ) > Values
		{
			{
				PixelFormat::eD24S8,
				PixelFormat::eRGBA16F32F,
				PixelFormat::eRGBA16F32F,
				PixelFormat::eRGBA16F32F,
				PixelFormat::eRGBA16F32F,
			}
		};

		return Values[size_t( p_texture )];
	}

	AttachmentPoint GetTextureAttachmentPoint( DsTexture p_texture )
	{
		static std::array< AttachmentPoint, size_t( DsTexture::eCount ) > Values
		{
			{
				AttachmentPoint::eDepthStencil,
				AttachmentPoint::eColour,
				AttachmentPoint::eColour,
				AttachmentPoint::eColour,
				AttachmentPoint::eColour,
			}
		};

		return Values[size_t( p_texture )];
	}

	uint32_t GetTextureAttachmentIndex( DsTexture p_texture )
	{
		static std::array< uint32_t, size_t( DsTexture::eCount ) > Values
		{
			{
				0,
				0,
				1,
				2,
				3,
			}
		};

		return Values[size_t( p_texture )];
	}

	float GetMaxDistance( LightCategory const & p_light
		, Point3f const & p_attenuation
		, float p_max )
	{
		constexpr float l_threshold = 0.000001f;
		auto l_const = std::abs( p_attenuation[0] );
		auto l_linear = std::abs( p_attenuation[1] );
		auto l_quadr = std::abs( p_attenuation[2] );
		float l_result = p_max;

		if ( l_const >= l_threshold
			|| l_linear >= l_threshold
			|| l_quadr >= l_threshold )
		{
			float l_maxChannel = std::max( std::max( p_light.GetColour()[0]
				, p_light.GetColour()[1] )
				, p_light.GetColour()[2] );
			auto l_c = 256.0f * l_maxChannel * p_light.GetDiffuseIntensity();

			if ( l_quadr >= l_threshold )
			{
				if ( l_linear < l_threshold )
				{
					REQUIRE( l_c >= l_const );
					l_result = sqrtf( ( l_c - l_const ) / l_quadr );
				}
				else
				{
					auto l_delta = l_linear * l_linear - 4 * l_quadr * ( l_const - l_c );
					REQUIRE( l_delta >= 0 );
					l_result = ( -l_linear + sqrtf( l_delta ) ) / ( 2 * l_quadr );
				}
			}
			else if ( l_linear >= l_threshold )
			{
				l_result = ( l_c - l_const ) / l_linear;
			}
		}

		return std::min( p_max, l_result );
	}

	void Declare_EncodeMaterial( GLSL::GlslWriter & p_writer )
	{
		using namespace GLSL;
		using GLSL::operator<<;
		auto l_encodeMaterial = p_writer.ImplementFunction< Void >( cuT( "EncodeMaterial" )
			, [&]( Int const & p_receiver
				, Int const & p_reflection
				, Int const & p_refraction
				, Int const & p_envMapIndex
				, Float p_encoded )
			{
				auto l_flags = p_writer.DeclLocale( cuT( "l_flags" )
					, p_writer.Paren( p_receiver << 7 )
						+ p_writer.Paren( p_refraction << 6 )
						+ p_writer.Paren( p_reflection << 5 )
						+ p_writer.Paren( p_envMapIndex ) );
				p_encoded = p_writer.Cast< Float >( l_flags );
			}, InInt{ &p_writer, cuT( "p_receiver" ) }
			, InInt{ &p_writer, cuT( "p_reflection" ) }
			, InInt{ &p_writer, cuT( "p_refraction" ) }
			, InInt{ &p_writer, cuT( "p_envMapIndex" ) }
			, OutFloat{ &p_writer, cuT( "p_encoded" ) } );
	}
	
	void Declare_DecodeMaterial( GLSL::GlslWriter & p_writer )
	{
		using namespace GLSL;
		auto l_decodeMaterial = p_writer.ImplementFunction< Void >( cuT( "DecodeMaterial" )
			, [&]( Float const & p_encoded
				, Int p_receiver
				, Int p_reflection
				, Int p_refraction
				, Int p_envMapIndex )
			{
				auto l_flags = p_writer.DeclLocale( cuT( "l_flags" ), p_writer.Cast< Int >( p_encoded ) );
				p_receiver = l_flags >> 7;
				l_flags -= p_writer.Paren( p_receiver << 7 );
				p_refraction = l_flags >> 6;
				l_flags -= p_writer.Paren( p_refraction << 6 );
				p_reflection = l_flags >> 5;
				l_flags -= p_writer.Paren( p_reflection << 5 );
				p_envMapIndex = l_flags;
			}, InFloat{ &p_writer, cuT( "p_encoded" ) }
			, OutInt{ &p_writer, cuT( "p_receiver" ) }
			, OutInt{ &p_writer, cuT( "p_reflection" ) }
			, OutInt{ &p_writer, cuT( "p_refraction" ) }
			, OutInt{ &p_writer, cuT( "p_envMapIndex" ) } );
	}

	void Declare_DecodeReceiver( GLSL::GlslWriter & p_writer )
	{
		using namespace GLSL;
		auto l_decodeReceiver = p_writer.ImplementFunction< Void >( cuT( "DecodeReceiver" )
			, [&]( Float const & p_encoded
				, Int p_receiver )
			{
				auto l_flags = p_writer.DeclLocale( cuT( "l_flags" ), p_writer.Cast< Int >( p_encoded ) );
				p_receiver = l_flags >> 7;
			}, InFloat{ &p_writer, cuT( "p_encoded" ) }
			, OutInt{ &p_writer, cuT( "p_receiver" ) } );
	}

	void EncodeMaterial( GLSL::GlslWriter & p_writer
		, GLSL::Int const & p_receiver
		, GLSL::Int const & p_reflection
		, GLSL::Int const & p_refraction
		, GLSL::Int const & p_envMapIndex
		, GLSL::Float const & p_encoded )
	{
		using namespace GLSL;
		p_writer << WriteFunctionCall< Void >( &p_writer
			, cuT( "EncodeMaterial" )
			, InInt{ p_receiver }
		, InInt{ p_reflection }
		, InInt{ p_refraction }
		, InInt{ p_envMapIndex }
		, OutFloat{ p_encoded } );
		p_writer << Endi{};
	}

	void DecodeMaterial( GLSL::GlslWriter & p_writer
		, GLSL::Float const & p_encoded
		, GLSL::Int const & p_receiver
		, GLSL::Int const & p_reflection
		, GLSL::Int const & p_refraction
		, GLSL::Int const & p_envMapIndex )
	{
		using namespace GLSL;
		p_writer << WriteFunctionCall< Void >( &p_writer
			, cuT( "DecodeMaterial" )
			, InFloat{ p_encoded }
			, OutInt{ p_receiver }
			, OutInt{ p_reflection }
			, OutInt{ p_refraction }
			, OutInt{ p_envMapIndex } );
		p_writer << Endi{};
	}

	void DecodeReceiver( GLSL::GlslWriter & p_writer
		, GLSL::Float const & p_encoded
		, GLSL::Int const & p_receiver )
	{
		using namespace GLSL;
		p_writer << WriteFunctionCall< Void >( &p_writer
			, cuT( "DecodeReceiver" )
			, InFloat{ p_encoded }
			, OutInt{ p_receiver } );
		p_writer << Endi{};
	}

	//************************************************************************************************

	const String GpInfo::GPInfo = cuT( "GPInfo" );
	const String GpInfo::InvViewProj = cuT( "c3d_mtxInvViewProj" );
	const String GpInfo::InvView = cuT( "c3d_mtxInvView" );
	const String GpInfo::InvProj = cuT( "c3d_mtxInvProj" );
	const String GpInfo::View = cuT( "c3d_mtxGView" );
	const String GpInfo::Proj = cuT( "c3d_mtxGProj" );
	const String GpInfo::RenderSize = cuT( "c3d_renderSize" );

	GpInfo::GpInfo( Engine & p_engine )
		: m_gpInfoUbo{ GpInfo::GPInfo, *p_engine.GetRenderSystem() }
		, m_invViewProjUniform{ m_gpInfoUbo.CreateUniform< UniformType::eMat4x4f >( GpInfo::InvViewProj ) }
		, m_invViewUniform{ m_gpInfoUbo.CreateUniform< UniformType::eMat4x4f >( GpInfo::InvView ) }
		, m_invProjUniform{ m_gpInfoUbo.CreateUniform< UniformType::eMat4x4f >( GpInfo::InvProj ) }
		, m_gViewUniform{ m_gpInfoUbo.CreateUniform< UniformType::eMat4x4f >( GpInfo::View ) }
		, m_gProjUniform{ m_gpInfoUbo.CreateUniform< UniformType::eMat4x4f >( GpInfo::Proj ) }
		, m_renderSize{ m_gpInfoUbo.CreateUniform< UniformType::eVec2f >( GpInfo::RenderSize ) }
	{
	}

	GpInfo::~GpInfo()
	{
		m_gpInfoUbo.Cleanup();
	}

	void GpInfo::Update( Size const & p_size
		, Camera const & p_camera
		, Matrix4x4r const & p_invViewProj
		, Matrix4x4r const & p_invView
		, Matrix4x4r const & p_invProj )
	{
		m_invViewProjUniform->SetValue( p_invViewProj );
		m_invViewUniform->SetValue( p_invView );
		m_invProjUniform->SetValue( p_invProj );
		m_gViewUniform->SetValue( p_camera.GetView() );
		m_gProjUniform->SetValue( p_camera.GetViewport().GetProjection() );
		m_renderSize->SetValue( Point2f( p_size.width(), p_size.height() ) );
		m_gpInfoUbo.Update();
	}

	//************************************************************************************************

	LightPass::Program::Program( Engine & p_engine
		, String const & p_vtx
		, String const & p_pxl )
	{
		auto & l_renderSystem = *p_engine.GetRenderSystem();
		ShaderModel l_model = l_renderSystem.GetGpuInformations().GetMaxShaderModel();

		m_program = p_engine.GetShaderProgramCache().GetNewProgram( false );
		m_program->CreateObject( ShaderType::eVertex );
		m_program->CreateObject( ShaderType::ePixel );
		m_program->SetSource( ShaderType::eVertex, l_model, p_vtx );
		m_program->SetSource( ShaderType::ePixel, l_model, p_pxl );

		m_lightColour = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_lightBase.m_colour" ), ShaderType::ePixel );

		for ( int i = 0; i < int( DsTexture::eCount ); i++ )
		{
			m_program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture( i ) ), ShaderType::ePixel )->SetValue( i );
		}
	}

	LightPass::Program::~Program()
	{
		m_firstPipeline->Cleanup();
		m_blendPipeline->Cleanup();
		m_firstPipeline.reset();
		m_blendPipeline.reset();
		m_geometryBuffers.reset();
		m_lightColour = nullptr;
		m_program.reset();
	}

	void LightPass::Program::Initialise( VertexBuffer & p_vbo
		, IndexBufferSPtr p_ibo
		, MatrixUbo & p_matrixUbo
		, SceneUbo & p_sceneUbo
		, UniformBuffer & p_gpInfoUbo
		, ModelMatrixUbo * p_modelMatrixUbo )
	{
		m_program->Initialise();

		m_firstPipeline = DoCreatePipeline( false );
		m_firstPipeline->AddUniformBuffer( p_matrixUbo.GetUbo() );
		m_firstPipeline->AddUniformBuffer( p_sceneUbo.GetUbo() );
		m_firstPipeline->AddUniformBuffer( p_gpInfoUbo );

		m_blendPipeline = DoCreatePipeline( true );
		m_blendPipeline->AddUniformBuffer( p_matrixUbo.GetUbo() );
		m_blendPipeline->AddUniformBuffer( p_sceneUbo.GetUbo() );
		m_blendPipeline->AddUniformBuffer( p_gpInfoUbo );

		if ( p_modelMatrixUbo )
		{
			m_firstPipeline->AddUniformBuffer( p_modelMatrixUbo->GetUbo() );
			m_blendPipeline->AddUniformBuffer( p_modelMatrixUbo->GetUbo() );
		}

		m_geometryBuffers = m_program->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, *m_program );
		m_geometryBuffers->Initialise( { p_vbo }, p_ibo );
	}

	void LightPass::Program::Cleanup()
	{
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		m_program->Cleanup();
	}

	void LightPass::Program::Bind( Light const & p_light )
	{
		DoBind( p_light );
	}

	void LightPass::Program::Render( Size const & p_size
		, Point3f const & p_colour
		, uint32_t p_count
		, bool p_first )const
	{
		m_lightColour->SetValue( p_colour );

		if ( p_first )
		{
			m_firstPipeline->Apply();
		}
		else
		{
			m_blendPipeline->Apply();
		}

		m_geometryBuffers->Draw( p_count, 0 );
	}

	//************************************************************************************************

	LightPass::LightPass( Engine & p_engine
		, FrameBuffer & p_frameBuffer
		, FrameBufferAttachment & p_depthAttach
		, bool p_shadows )
		: m_engine{ p_engine }
		, m_shadows{ p_shadows }
		, m_matrixUbo{ p_engine }
		, m_frameBuffer{ p_frameBuffer }
		, m_depthAttach{ p_depthAttach }
	{
	}

	void LightPass::Render( Size const & p_size
		, GeometryPassResult const & p_gp
		, Light const & p_light
		, Camera const & p_camera
		, Matrix4x4r const & p_invViewProj
		, Castor::Matrix4x4r const & p_invView
		, Castor::Matrix4x4r const & p_invProj
		, bool p_first )
	{
		m_gpInfo->Update( p_size
			, p_camera
			, p_invViewProj
			, p_invView
			, p_invProj );

		DoUpdate( p_size
			, p_light
			, p_camera );

		m_program->Bind( p_light );

		DoRender( p_size
			, p_gp
			, p_light.GetColour()
			, p_first );
	}

	void LightPass::DoInitialise( Scene const & p_scene
		, LightType p_type
		, VertexBuffer & p_vbo
		, IndexBufferSPtr p_ibo
		, SceneUbo & p_sceneUbo
		, ModelMatrixUbo * p_modelMatrixUbo )
	{
		m_gpInfo = std::make_unique< GpInfo >( m_engine );
		SceneFlags l_sceneFlags{ p_scene.GetFlags() };
		m_program = DoCreateProgram( DoGetVertexShaderSource( l_sceneFlags )
			, DoGetPixelShaderSource( l_sceneFlags, p_type ) );
		m_program->Initialise( p_vbo
			, p_ibo
			, m_matrixUbo
			, p_sceneUbo
			, m_gpInfo->GetUbo()
			, p_modelMatrixUbo );
	}

	void LightPass::DoCleanup()
	{
		m_program->Cleanup();
		m_program.reset();
		m_gpInfo.reset();
		m_matrixUbo.GetUbo().Cleanup();
	}

	void LightPass::DoRender( Castor::Size const & p_size
		, GeometryPassResult const & p_gp
		, Point3f const & p_colour
		, bool p_first )
	{
		m_frameBuffer.Bind( FrameBufferTarget::eDraw );
		m_depthAttach.Attach( AttachmentPoint::eDepthStencil );
		m_frameBuffer.SetDrawBuffers();
		p_gp[size_t( DsTexture::eDepth )]->Bind();
		p_gp[size_t( DsTexture::eNormal )]->Bind();
		p_gp[size_t( DsTexture::eDiffuse )]->Bind();
		p_gp[size_t( DsTexture::eSpecular )]->Bind();
		p_gp[size_t( DsTexture::eEmissive )]->Bind();

		m_program->Render( p_size
			, p_colour
			, GetCount()
			, p_first );

		p_gp[size_t( DsTexture::eEmissive )]->Unbind();
		p_gp[size_t( DsTexture::eSpecular )]->Unbind();
		p_gp[size_t( DsTexture::eDiffuse )]->Unbind();
		p_gp[size_t( DsTexture::eNormal )]->Unbind();
		p_gp[size_t( DsTexture::eDepth )]->Unbind();
		m_frameBuffer.Unbind();
	}
	
	String LightPass::DoGetPixelShaderSource( SceneFlags const & p_sceneFlags
		, LightType p_type )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_engine.GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_GPINFO( l_writer );
		auto c3d_mapDepth = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eDepth ) );
		auto c3d_mapNormals = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eNormal ) );
		auto c3d_mapDiffuse = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eDiffuse ) );
		auto c3d_mapSpecular = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eSpecular ) );
		auto c3d_mapEmissive = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eEmissive ) );
		auto gl_FragCoord = l_writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

		// Shader outputs
		auto pxl_v4FragColor = l_writer.DeclFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

		// Utility functions
		auto l_lighting = CreateLightingModel( l_writer
			, p_type
			, m_shadows ? GetShadowType( p_sceneFlags ) : ShadowType::eNone );
		GLSL::Fog l_fog{ GetFogType( p_sceneFlags ), l_writer };
		GLSL::Utils l_utils{ l_writer };
		l_utils.DeclareCalcTexCoord();
		l_utils.DeclareCalcWSPosition();
		Declare_DecodeReceiver( l_writer );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto l_texCoord = l_writer.DeclLocale( cuT( "l_texCoord" ), l_utils.CalcTexCoord() );
			auto l_v4Diffuse = l_writer.DeclLocale( cuT( "l_v4Diffuse" ), texture( c3d_mapDiffuse, l_texCoord ) );
			auto l_v4Normal = l_writer.DeclLocale( cuT( "l_v4Normal" ), texture( c3d_mapNormals, l_texCoord ) );
			auto l_v4Specular = l_writer.DeclLocale( cuT( "l_v4Specular" ), texture( c3d_mapSpecular, l_texCoord ) );
			auto l_v4Emissive = l_writer.DeclLocale( cuT( "l_v4Emissive" ), texture( c3d_mapEmissive, l_texCoord ) );
			auto l_flags = l_writer.DeclLocale( cuT( "l_flags" ), l_writer.Cast< Int >( l_v4Normal.w() ) );
			auto l_iShadowReceiver = l_writer.DeclLocale( cuT( "l_iShadowReceiver" ), 0_i );
			DecodeReceiver( l_writer, l_flags, l_iShadowReceiver );
			auto l_v3MapDiffuse = l_writer.DeclLocale( cuT( "l_v3MapDiffuse" ), l_v4Diffuse.xyz() );
			auto l_v3MapSpecular = l_writer.DeclLocale( cuT( "l_v3MapSpecular" ), l_v4Specular.xyz() );
			auto l_fMatShininess = l_writer.DeclLocale( cuT( "l_fMatShininess" ), l_v4Specular.w() );
			auto l_v3Specular = l_writer.DeclLocale( cuT( "l_v3Specular" ), vec3( 0.0_f, 0, 0 ) );
			auto l_v3Diffuse = l_writer.DeclLocale( cuT( "l_v3Diffuse" ), vec3( 0.0_f, 0, 0 ) );
			auto l_eye = l_writer.DeclLocale( cuT( "l_eye" ), c3d_v3CameraPosition );

			auto l_wsPosition = l_writer.DeclLocale( cuT( "l_wsPosition" ), l_utils.CalcWSPosition( l_texCoord, c3d_mtxInvViewProj ) );
			auto l_wsNormal = l_writer.DeclLocale( cuT( "l_wsNormal" ), l_v4Normal.xyz() );

			OutputComponents l_output{ l_v3Diffuse, l_v3Specular };

			switch ( p_type )
			{
			case LightType::eDirectional:
				{
					auto light = l_writer.GetBuiltin< GLSL::DirectionalLight >( cuT( "light" ) );
					l_lighting->ComputeOneDirectionalLight( light
						, l_eye
						, l_fMatShininess
						, l_iShadowReceiver
						, FragmentInput( l_wsPosition, l_wsNormal )
						, l_output );
				}
				break;

			case LightType::ePoint:
				{
				auto light = l_writer.GetBuiltin< GLSL::PointLight >( cuT( "light" ) );
					l_lighting->ComputeOnePointLight( light
						, l_eye
						, l_fMatShininess
						, l_iShadowReceiver
						, FragmentInput( l_wsPosition, l_wsNormal )
						, l_output );
				}
				break;

			case LightType::eSpot:
				{
				auto light = l_writer.GetBuiltin< GLSL::SpotLight >( cuT( "light" ) );
					l_lighting->ComputeOneSpotLight( light
						, l_eye
						, l_fMatShininess
						, l_iShadowReceiver
						, FragmentInput( l_wsPosition, l_wsNormal )
						, l_output );
				}
				break;
			}

			pxl_v4FragColor = vec4( l_v3Diffuse * l_v3MapDiffuse.xyz()
				+ l_v3Specular * l_v3MapSpecular.xyz(), 1.0 );
		} );

		return l_writer.Finalise();
	}

	//************************************************************************************************
}