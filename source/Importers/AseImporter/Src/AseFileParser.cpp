#include "AseFileParser.hpp"
#include "AseFileContext.hpp"

#include <RenderSystem.hpp>
#include <Buffer.hpp>
#include <SceneNode.hpp>
#include <Scene.hpp>
#include <Camera.hpp>
#include <Viewport.hpp>
#include <Material.hpp>
#include <MaterialManager.hpp>
#include <Pass.hpp>
#include <TextureUnit.hpp>
#include <Version.hpp>
#include <Geometry.hpp>
#include <Mesh.hpp>
#include <Engine.hpp>
#include <Submesh.hpp>
#include <StaticTexture.hpp>
#include <Plugin.hpp>
#include <Face.hpp>
#include <Vertex.hpp>

using namespace Castor3D;
using namespace Castor;

#if defined( _MSC_VER )
#	pragma warning( push )
#	pragma warning( disable:4100 )
#endif

namespace Ase
{
	AseFileParser::AseFileParser( Engine * p_pEngine )
		:	FileParser( eASE_SECTION_ROOT, eASE_SECTION_COUNT )
		,	m_pScene( NULL )
		,	m_pEngine( p_pEngine )
	{
	}

	AseFileParser::~AseFileParser()
	{
	}

	bool AseFileParser::ParseFile( TextFile & p_file, SceneSPtr p_pScene )
	{
		m_pScene = p_pScene;
		return FileParser::ParseFile( p_file );
	}

	bool AseFileParser::ParseFile( Path const & p_pathFile, SceneSPtr p_pScene )
	{
		m_pScene = p_pScene;
		return FileParser::ParseFile( p_pathFile );
	}

	void AseFileParser::DoInitialiseParser( TextFile & p_file )
	{
		AddParser( eASE_SECTION_ROOT,			cuT( "*3DSMAX_ASCIIEXPORT"		),	AseParser_RootFormat				);
		AddParser( eASE_SECTION_ROOT,			cuT( "*COMMENT"					),	AseParser_RootComment				);
		AddParser( eASE_SECTION_ROOT,			cuT( "*SCENE"					),	AseParser_RootScene					);
		AddParser( eASE_SECTION_ROOT,			cuT( "*MATERIAL_LIST"			),	AseParser_RootMaterials				);
		AddParser( eASE_SECTION_ROOT,			cuT( "*GEOMOBJECT"				),	AseParser_RootGeometry				);
		AddParser( eASE_SECTION_SCENE,			cuT( "*SCENE_FILENAME"			),	AseParser_SceneFileName				);
		AddParser( eASE_SECTION_SCENE,			cuT( "*SCENE_FIRSTFRAME"		),	AseParser_SceneFirstFrame			);
		AddParser( eASE_SECTION_SCENE,			cuT( "*SCENE_LASTFRAME"			),	AseParser_SceneLastFrame			);
		AddParser( eASE_SECTION_SCENE,			cuT( "*SCENE_FRAMESPEED"		),	AseParser_SceneFrameSpeed			);
		AddParser( eASE_SECTION_SCENE,			cuT( "*SCENE_TICKSPERFRAME"		),	AseParser_SceneTicksPerFrame		);
		AddParser( eASE_SECTION_SCENE,			cuT( "*SCENE_BACKGROUND_STATIC"	),	AseParser_SceneBgColour				);
		AddParser( eASE_SECTION_SCENE,			cuT( "*SCENE_AMBIENT_STATIC"	),	AseParser_SceneAmbientLight			);
		AddParser( eASE_SECTION_SCENE,			cuT( "}"						),	AseParser_SceneEnd					);
		AddParser( eASE_SECTION_MATERIALS,		cuT( "*MATERIAL_COUNT"			),	AseParser_MaterialsCount			);
		AddParser( eASE_SECTION_MATERIALS,		cuT( "*MATERIAL"				),	AseParser_MaterialsMat				);
		AddParser( eASE_SECTION_MATERIALS,		cuT( "}"						),	AseParser_MaterialsEnd				);
		AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_NAME"			),	AseParser_MaterialName				);
		AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_CLASS"			),	AseParser_MaterialClass				);
		AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_AMBIENT"		),	AseParser_MaterialAmbient			);
		AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_DIFFUSE"		),	AseParser_MaterialDiffuse			);
		AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_SPECULAR"		),	AseParser_MaterialSpecular			);
		AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_SHINE"			),	AseParser_MaterialShine				);
		AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_SHINESTRENGTH"	),	AseParser_MaterialShineStrength		);
		AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_TRANSPARENCY"	),	AseParser_MaterialTransparency		);
		AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_WIRESIZE"		),	AseParser_MaterialWiresize			);
		AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_SHADING"		),	AseParser_MaterialShading			);
		AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_XP_FALLOFF"		),	AseParser_MaterialXPFalloff			);
		AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_SELFILLUM"		),	AseParser_MaterialSelfillum			);
		AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_FALLOFF"		),	AseParser_MaterialFalloff			);
		AddParser( eASE_SECTION_MATERIAL,		cuT( "*MATERIAL_XP_TYPE"		),	AseParser_MaterialXPType			);
		AddParser( eASE_SECTION_MATERIAL,		cuT( "*SUBMATERIAL"				),	AseParser_MaterialSubmat			);
		AddParser( eASE_SECTION_MATERIAL,		cuT( "*MAP_DIFFUSE"				),	AseParser_MaterialMapDiffuse		);
		AddParser( eASE_SECTION_MATERIAL,		cuT( "}"						),	AseParser_MaterialEnd				);
		AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_NAME"			),	AseParser_SubMaterialName			);
		AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_CLASS"			),	AseParser_MaterialClass				);
		AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_AMBIENT"		),	AseParser_MaterialAmbient			);
		AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_DIFFUSE"		),	AseParser_MaterialDiffuse			);
		AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_SPECULAR"		),	AseParser_MaterialSpecular			);
		AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_SHINE"			),	AseParser_MaterialShine				);
		AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_SHINESTRENGTH"	),	AseParser_MaterialShineStrength		);
		AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_TRANSPARENCY"	),	AseParser_MaterialTransparency		);
		AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_WIRESIZE"		),	AseParser_MaterialWiresize			);
		AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_SHADING"		),	AseParser_MaterialShading			);
		AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_XP_FALLOFF"		),	AseParser_MaterialXPFalloff			);
		AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_SELFILLUM"		),	AseParser_MaterialSelfillum			);
		AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_FALLOFF"		),	AseParser_MaterialFalloff			);
		AddParser( eASE_SECTION_SUBMAT,			cuT( "*MATERIAL_XP_TYPE"		),	AseParser_MaterialXPType			);
		AddParser( eASE_SECTION_SUBMAT,			cuT( "*MAP_DIFFUSE"				),	AseParser_MaterialMapDiffuse		);
		AddParser( eASE_SECTION_SUBMAT,			cuT( "}"						),	AseParser_SubMaterialEnd			);
		AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*MAP_NAME"				),	AseParser_MapDiffuseName			);
		AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*MAP_CLASS"				),	AseParser_MapDiffuseClass			);
		AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*MAP_SUBNO"				),	AseParser_MapDiffuseSubno			);
		AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*MAP_AMOUNT"				),	AseParser_MapDiffuseAmount			);
		AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*BITMAP"					),	AseParser_MapDiffuseBitmap			);
		AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*MAP_TYPE"				),	AseParser_MapDiffuseType			);
		AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_U_OFFSET"			),	AseParser_MapDiffuseUOffset			);
		AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_V_OFFSET"			),	AseParser_MapDiffuseVOffset			);
		AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_U_TILING"			),	AseParser_MapDiffuseUTiling			);
		AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_V_TILING"			),	AseParser_MapDiffuseVTiling			);
		AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_ANGLE"				),	AseParser_MapDiffuseAngle			);
		AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_BLUR"				),	AseParser_MapDiffuseBlur			);
		AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_BLUR_OFFSET"			),	AseParser_MapDiffuseBlurOffset		);
		AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_NOUSE_AMT"			),	AseParser_MapDiffuseNouseAMT		);
		AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_NOISE_SIZE"			),	AseParser_MapDiffuseNoiseSize		);
		AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_NOISE_LEVEL"			),	AseParser_MapDiffuseNoiseLevel		);
		AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*UVW_NOISE_PHASE"			),	AseParser_MapDiffuseNoisePhase		);
		AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "*BITMAP_FILTER"			),	AseParser_MapDiffuseFilter			);
		AddParser( eASE_SECTION_MAPDIFFUSE,		cuT( "}"						),	AseParser_MapDiffuseEnd				);
		AddParser( eASE_SECTION_GEOMETRY,		cuT( "*NODE_NAME"				),	AseParser_GeometryNodeName			);
		AddParser( eASE_SECTION_GEOMETRY,		cuT( "*NODE_TM"					),	AseParser_GeometryNodeTM			);
		AddParser( eASE_SECTION_GEOMETRY,		cuT( "*MESH"					),	AseParser_GeometryMesh				);
		AddParser( eASE_SECTION_GEOMETRY,		cuT( "*PROP_MOTIONBLUR"			),	AseParser_GeometryMotionBlur		);
		AddParser( eASE_SECTION_GEOMETRY,		cuT( "*PROP_CASTSHADOW"			),	AseParser_GeometryCastShadow		);
		AddParser( eASE_SECTION_GEOMETRY,		cuT( "*PROP_RECVSHADOW"			),	AseParser_GeometryRecvShadow		);
		AddParser( eASE_SECTION_GEOMETRY,		cuT( "*MATERIAL_REF"			),	AseParser_GeometryMaterialRef		);
		AddParser( eASE_SECTION_GEOMETRY,		cuT( "}"						),	AseParser_GeometryEnd				);
		AddParser( eASE_SECTION_GEONODE,		cuT( "*NODE_NAME"				),	AseParser_GeoNodeName				);
		AddParser( eASE_SECTION_GEONODE,		cuT( "*INHERIT_POS"				),	AseParser_GeoNodeInheritPos			);
		AddParser( eASE_SECTION_GEONODE,		cuT( "*INHERIT_ROT"				),	AseParser_GeoNodeInheritRot			);
		AddParser( eASE_SECTION_GEONODE,		cuT( "*INHERIT_SCL"				),	AseParser_GeoNodeInheritScl			);
		AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_ROW0"					),	AseParser_GeoNodeRow0				);
		AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_ROW1"					),	AseParser_GeoNodeRow1				);
		AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_ROW2"					),	AseParser_GeoNodeRow2				);
		AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_ROW3"					),	AseParser_GeoNodeRow3				);
		AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_POS"					),	AseParser_GeoNodePos				);
		AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_ROTAXIS"				),	AseParser_GeoNodeRotAxis			);
		AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_ROTANGLE"				),	AseParser_GeoNodeRotAngle			);
		AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_SCALE"				),	AseParser_GeoNodeScale				);
		AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_SCALEAXIS"			),	AseParser_GeoNodeScaleAxis			);
		AddParser( eASE_SECTION_GEONODE,		cuT( "*TM_SCALEAXISANG"			),	AseParser_GeoNodeScaleAngle			);
		AddParser( eASE_SECTION_GEONODE,		cuT( "}"						),	AseParser_GeoNodeEnd				);
		AddParser( eASE_SECTION_GEOMESH,		cuT( "*TIMEVALUE"				),	AseParser_GeoMeshTimeValue			);
		AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_NUMVERTEX"			),	AseParser_GeoMeshNumVertex			);
		AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_VERTEX_LIST"		),	AseParser_GeoMeshVertexList			);
		AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_NUMFACES"			),	AseParser_GeoMeshNumFaces			);
		AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_FACE_LIST"			),	AseParser_GeoMeshFaceList			);
		AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_NUMTVERTEX"			),	AseParser_GeoMeshNumTVertex			);
		AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_TVERTLIST"			),	AseParser_GeoMeshTVertexList		);
		AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_NUMTVFACES"			),	AseParser_GeoMeshNumTFaces			);
		AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_TFACELIST"			),	AseParser_GeoMeshTFaceList			);
		AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_NUMCVERTEX"			),	AseParser_GeoMeshNumCVertex			);
		AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_CVERTLIST"			),	AseParser_GeoMeshCVertexList		);
		AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_NUMCVFACES"			),	AseParser_GeoMeshNumCFaces			);
		AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_CFACELIST"			),	AseParser_GeoMeshCFaceList			);
		AddParser( eASE_SECTION_GEOMESH,		cuT( "*MESH_NORMALS"			),	AseParser_GeoMeshNormals			);
		AddParser( eASE_SECTION_GEOMESH,		cuT( "}"						),	AseParser_GeoMeshEnd				);
		AddParser( eASE_SECTION_VERTEXLIST,		cuT( "*MESH_VERTEX"				),	AseParser_VertexListVertex			);
		AddParser( eASE_SECTION_VERTEXLIST,		cuT( "}"						),	AseParser_VertexListEnd				);
		AddParser( eASE_SECTION_FACELIST,		cuT( "*MESH_FACE"				),	AseParser_FaceListFace				);
		AddParser( eASE_SECTION_FACELIST,		cuT( "}"						),	AseParser_FaceListEnd				);
		AddParser( eASE_SECTION_TVERTEXLIST,	cuT( "*MESH_TVERT"				),	AseParser_TVertexListVertex			);
		AddParser( eASE_SECTION_TVERTEXLIST,	cuT( "}"						),	AseParser_TVertexListEnd			);
		AddParser( eASE_SECTION_TFACELIST,		cuT( "*MESH_TFACE"				),	AseParser_TFaceListFace				);
		AddParser( eASE_SECTION_TFACELIST,		cuT( "}"						),	AseParser_TFaceListEnd				);
		AddParser( eASE_SECTION_CVERTEXLIST,	cuT( "*MESH_CVERT"				),	AseParser_CVertexListVertex			);
		AddParser( eASE_SECTION_CVERTEXLIST,	cuT( "}"						),	AseParser_CVertexListEnd			);
		AddParser( eASE_SECTION_CFACELIST,		cuT( "*MESH_CFACE"				),	AseParser_CFaceListFace				);
		AddParser( eASE_SECTION_CFACELIST,		cuT( "}"						),	AseParser_CFaceListEnd				);
		AddParser( eASE_SECTION_NORMALSLIST,	cuT( "*MESH_FACENORMAL"			),	AseParser_NormalsListFaceNormal		);
		AddParser( eASE_SECTION_NORMALSLIST,	cuT( "*MESH_VERTEXNORMAL"		),	AseParser_NormalsListVertexNormal	);
		AddParser( eASE_SECTION_NORMALSLIST,	cuT( "}"						),	AseParser_NormalsListEnd			);
		std::shared_ptr< AseFileContext > l_pContext = std::make_shared< AseFileContext >( this, &p_file );
		m_pParsingContext = std::static_pointer_cast< FileParserContext >( l_pContext );
		l_pContext->Initialise();
		l_pContext->strName.clear();
		l_pContext->pScene = m_pScene;
	}

	void AseFileParser::DoCleanupParser()
	{
	}

	void AseFileParser::DoDiscardParser( String const & p_strLine )
	{
		String strToLog( cuT( "Parser not found @ line #" ) );
		Logger::LogWarning( strToLog << m_pParsingContext->ui64Line << cuT( " : " ) << p_strLine );
	}

	void AseFileParser::DoValidate()
	{
		std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( m_pParsingContext );

		if ( !m_pScene )
		{
			m_pMesh = l_pContext->pMesh;
		}
	}
}

namespace
{
	void RetrieveVertex( uint32_t & p_uiIndex, float * p_position, String p_strLine )
	{
		StringStream l_stream( p_strLine );
		l_stream >> p_uiIndex >> p_position[0] >> p_position[1] >> p_position[2];
	}

	void RetrieveFace( int * p_indices, String p_strLine )
	{
		int l_index = 0;
		sscanf( str_utils::to_str( p_strLine ).c_str(), "%d:\tA:\t%d B:\t%d C:\t%d", & l_index, & p_indices[0], & p_indices[1], & p_indices[2] );
	}

	template< size_t Count, typename Type >
	void ReadNValues( Type * p_colour, String p_strLine )
	{
		StringStream l_stream( p_strLine );

		for ( size_t i = 0; i < Count; ++i )
		{
			l_stream >> p_colour[i];
		}
	}

	void Read3Floats( float * p_colour, String p_strLine )
	{
		ReadNValues< 3 >( p_colour, p_strLine );
	}

	void Read4Floats( float * p_colour, String p_strLine )
	{
		ReadNValues< 4 >( p_colour, p_strLine );
	}

	void Read3Ints( int * p_colour, String p_strLine )
	{
		ReadNValues< 3 >( p_colour, p_strLine );
	}

	void Read4Ints( int * p_colour, String p_strLine )
	{
		ReadNValues< 4 >( p_colour, p_strLine );
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_RootFormat )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_RootComment )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_RootScene )
{
	if ( !std::static_pointer_cast< AseFileContext >( p_pContext )->pScene )
	{
		p_pParser->Ignore();
	}
}
END_ATTRIBUTE_PUSH( eASE_SECTION_SCENE )

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_RootMaterials )
{
}
END_ATTRIBUTE_PUSH( eASE_SECTION_MATERIALS )

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_RootGeometry )
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );

	if ( ( !l_pContext->pScene && l_pContext->pMesh ) )
	{
		p_pParser->Ignore();
	}
}
END_ATTRIBUTE_PUSH( eASE_SECTION_GEOMETRY )

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SceneFileName )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SceneFirstFrame )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SceneLastFrame )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SceneFrameSpeed )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SceneTicksPerFrame )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SceneBgColour )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SceneAmbientLight )
{
	Colour l_colour;
	Point3f l_ptColour;
	p_arrayParams[0]->Get( l_ptColour );
	l_colour.from_rgb( l_ptColour );
	std::static_pointer_cast< AseFileContext >( p_pContext )->pScene->SetAmbientLight( l_colour );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SceneEnd )
{
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialsCount )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialsMat )
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	p_arrayParams[0]->Get( l_pContext->uiUInt32 );
}
END_ATTRIBUTE_PUSH( eASE_SECTION_MATERIAL )

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialsEnd )
{
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialSubmat )
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	l_pContext->pPass = l_pContext->pMaterial->CreatePass();
}
END_ATTRIBUTE_PUSH( eASE_SECTION_SUBMAT )

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialName )
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	Engine * l_pEngine = l_pContext->m_pParser->GetEngine();
	MaterialManager & l_manager = l_pEngine->GetMaterialManager();
	MaterialSPtr l_pMaterial;
	String l_strName;
	p_arrayParams[0]->Get( l_strName );
	str_utils::replace( l_strName, cuT( "\"" ), cuT( "" ) );
	l_pMaterial = l_manager.find( l_strName );

	if ( !l_pMaterial )
	{
		l_pMaterial = std::make_shared< Material >( l_pEngine, l_strName );
		l_pMaterial->CreatePass();
		l_manager.insert( l_strName, l_pMaterial );
	}

	l_pContext->pMaterial = l_pMaterial;
	l_pContext->pPass = l_pMaterial->GetPass( 0 );
	l_pContext->pPass->SetTwoSided( true );
	l_pContext->m_mapMaterials.insert( std::make_pair( l_pContext->uiUInt32, l_pMaterial ) );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialClass )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialAmbient )
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	Colour l_colour;
	Point3f l_ptColour;
	p_arrayParams[0]->Get( l_ptColour );
	l_colour.from_rgb( l_ptColour );
	l_pContext->pPass->SetAmbient( l_colour );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialDiffuse )
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	Colour l_colour;
	Point3f l_ptColour;
	p_arrayParams[0]->Get( l_ptColour );
	l_colour.from_rgb( l_ptColour );
	l_pContext->pPass->SetDiffuse( l_colour );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialSpecular )
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	Colour l_colour;
	Point3f l_ptColour;
	p_arrayParams[0]->Get( l_ptColour );
	l_colour.from_rgb( l_ptColour );
	l_pContext->pPass->SetSpecular( l_colour );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialShine )
{
	float l_fValue;
	p_arrayParams[0]->Get( l_fValue );
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	l_pContext->pPass->SetShininess( l_fValue * 128.0f );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialShineStrength )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialTransparency )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialWiresize )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialShading )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialXPFalloff )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialSelfillum )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialFalloff )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialXPType )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialMapDiffuse )
{
}
END_ATTRIBUTE_PUSH( eASE_SECTION_MAPDIFFUSE )

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MaterialEnd )
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	l_pContext->pMaterial.reset();
	l_pContext->pPass.reset();
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SubMaterialName )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_SubMaterialEnd )
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	l_pContext->pPass = l_pContext->pMaterial->GetPass( 0 );
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseName )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseClass )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseSubno )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseAmount )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseBitmap )
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	Engine * l_pEngine = l_pContext->m_pParser->GetEngine();
	PassSPtr l_pPass = l_pContext->pPass;
	Path l_filePath = p_pContext->pFile->GetFileFullPath().GetPath();
	String l_strName;
	p_arrayParams[0]->Get( l_strName );
	ImageSPtr l_pImage;
	ImageCollection & l_imgCollection = l_pEngine->GetImageManager();
	StaticTextureSPtr l_pTexture;
	str_utils::replace( l_strName, cuT( "\"" ), cuT( "" ) );

	if ( File::FileExists( l_strName ) )
	{
		l_pContext->pTextureUnit = l_pPass->AddTextureUnit();
		l_pImage = l_imgCollection.find( l_strName );

		if ( !l_pImage )
		{
			l_pImage = std::make_shared< Image >( l_strName, l_strName );
			l_imgCollection.insert( l_strName, l_pImage );
		}
	}
	else if ( File::FileExists( l_filePath / l_strName ) )
	{
		l_pContext->pTextureUnit = l_pPass->AddTextureUnit();
		l_strName = l_filePath / l_strName;
		l_pImage = l_imgCollection.find( l_strName );

		if ( !l_pImage )
		{
			l_pImage = std::make_shared< Image >( l_strName, l_strName );
			l_imgCollection.insert( l_strName, l_pImage );
		}
	}

	if ( l_pImage )
	{
		l_pTexture = l_pEngine->GetRenderSystem()->CreateStaticTexture();
		l_pTexture->SetDimension( eTEXTURE_DIMENSION_2D );
		l_pTexture->SetImage( l_pImage->GetPixels() );
		l_pContext->pTextureUnit->SetTexture( l_pTexture );
		l_pContext->pTextureUnit->SetChannel( eTEXTURE_CHANNEL_DIFFUSE );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseType )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseUOffset )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseVOffset )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseUTiling )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseVTiling )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseAngle )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseBlur )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseBlurOffset )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseNouseAMT )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseNoiseSize )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseNoiseLevel )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseNoisePhase )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseFilter )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_MapDiffuseEnd )
{
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeometryNodeName )
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	Engine * l_pEngine = l_pContext->m_pParser->GetEngine();
	p_arrayParams[0]->Get( l_pContext->strName );
	l_pContext->pMesh = l_pEngine->CreateMesh( eMESH_TYPE_CUSTOM, l_pContext->strName );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeometryNodeTM )
{
	if ( !std::static_pointer_cast< AseFileContext >( p_pContext )->pScene )
	{
		p_pParser->Ignore();
	}
}
END_ATTRIBUTE_PUSH( eASE_SECTION_GEONODE )

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeometryMesh )
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	l_pContext->pSubmesh = l_pContext->pMesh->CreateSubmesh();
}
END_ATTRIBUTE_PUSH( eASE_SECTION_GEOMESH )

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeometryMotionBlur )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeometryCastShadow )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeometryRecvShadow )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeometryMaterialRef )
{
	uint32_t l_uiMaterialId;
	p_arrayParams[0]->Get( l_uiMaterialId );
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	MaterialPtrUIntMap::const_iterator l_it = l_pContext->m_mapMaterials.find( l_uiMaterialId );

	if ( l_it != l_pContext->m_mapMaterials.end() )
	{
		std::for_each( l_pContext->pMesh->Begin(), l_pContext->pMesh->End(), [&]( SubmeshSPtr p_pSubmesh )
		{
			p_pSubmesh->SetDefaultMaterial( l_it->second );
		} );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeometryEnd )
{
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeName )
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	String l_strValue;
	p_arrayParams[0]->Get( l_strValue );
	l_pContext->pSceneNode = l_pContext->pScene->CreateSceneNode( l_strValue );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeInheritPos )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeInheritRot )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeInheritScl )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeRow0 )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeRow1 )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeRow2 )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeRow3 )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodePos )
{
	Point3f l_position;
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	p_arrayParams[0]->Get( l_position );
	l_pContext->pSceneNode->SetPosition( l_position );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeRotAxis )
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	Point3f l_value;
	p_arrayParams[0]->Get( l_value );
	std::memcpy( l_pContext->fAxis, l_value.const_ptr(), sizeof( float ) * 3 );
	l_pContext->bBool1 = true;

	if ( l_pContext->bBool2 )
	{
		Quaternion l_quat;
		l_quat.FromAxisAngle( Point3r( l_pContext->fAxis[0], l_pContext->fAxis[1], l_pContext->fAxis[2] ), Angle::FromRadians( l_pContext->fAngle ) );
		l_pContext->pSceneNode->SetOrientation( l_quat );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeRotAngle )
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	p_arrayParams[0]->Get( l_pContext->fAngle );
	l_pContext->bBool2 = true;

	if ( l_pContext->bBool1 )
	{
		Quaternion l_quat;
		l_quat.FromAxisAngle( Point3r( l_pContext->fAxis[0], l_pContext->fAxis[1], l_pContext->fAxis[2] ), Angle::FromRadians( l_pContext->fAngle ) );
		l_pContext->pSceneNode->SetOrientation( l_quat );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeScale )
{
	Point3f l_scale;
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	p_arrayParams[0]->Get( l_scale );
	l_pContext->pSceneNode->SetScale( l_scale );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeScaleAxis )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeScaleAngle )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoNodeEnd )
{
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshTimeValue )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshNumVertex )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshVertexList )
{
}
END_ATTRIBUTE_PUSH( eASE_SECTION_VERTEXLIST )

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshNumFaces )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshFaceList )
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
}
END_ATTRIBUTE_PUSH( eASE_SECTION_FACELIST )

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshNumTVertex )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshTVertexList )
{
}
END_ATTRIBUTE_PUSH( eASE_SECTION_TVERTEXLIST )

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshNumTFaces )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshTFaceList )
{
}
END_ATTRIBUTE_PUSH( eASE_SECTION_TFACELIST )

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshNumCVertex )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshCVertexList )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshNumCFaces )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshCFaceList )
{
}
END_ATTRIBUTE_PUSH( eASE_SECTION_CFACELIST )

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshNormals )
{
}
END_ATTRIBUTE_PUSH( eASE_SECTION_NORMALSLIST )

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_GeoMeshEnd )
{
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_VertexListVertex )
{
	uint32_t l_uiIndex;
	float l_vertex[3];
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );
	RetrieveVertex( l_uiIndex, l_vertex, l_strParams );
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	l_pContext->pSubmesh->AddPoint( l_vertex );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_VertexListEnd )
{
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_FaceListFace )
{
	int l_index[3];
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );
	RetrieveFace( l_index, l_strParams );
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	l_pContext->pSubmesh->AddFace( l_index[0], l_index[1], l_index[2] );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_FaceListEnd )
{
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_TVertexListVertex )
{
	uint32_t l_uiIndex;
	float l_vertex[3];
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );
	RetrieveVertex( l_uiIndex, l_vertex, l_strParams );
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	l_pContext->m_arrayTexCoords.push_back( std::make_shared< Point3r >( l_vertex[0], l_vertex[1], l_vertex[2] ) );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_TVertexListEnd )
{
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_TFaceListFace )
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	int l_index[4];
	FaceSPtr l_pFace;
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );
	Read4Ints( l_index, l_strParams );
	l_pFace = l_pContext->pSubmesh->GetFace( l_index[0] );
	Vertex::SetTexCoord( l_pContext->pSubmesh->GetPoint( l_pFace->GetVertexIndex( 0 ) ), l_pContext->m_arrayTexCoords[ l_index[1] ]->at( 0 ), l_pContext->m_arrayTexCoords[ l_index[1] ]->at( 1 ) );
	Vertex::SetTexCoord( l_pContext->pSubmesh->GetPoint( l_pFace->GetVertexIndex( 1 ) ), l_pContext->m_arrayTexCoords[ l_index[2] ]->at( 0 ), l_pContext->m_arrayTexCoords[ l_index[2] ]->at( 1 ) );
	Vertex::SetTexCoord( l_pContext->pSubmesh->GetPoint( l_pFace->GetVertexIndex( 2 ) ), l_pContext->m_arrayTexCoords[ l_index[3] ]->at( 0 ), l_pContext->m_arrayTexCoords[ l_index[3] ]->at( 1 ) );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_TFaceListEnd )
{
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_CVertexListVertex )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_CVertexListEnd )
{
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_CFaceListFace )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_CFaceListEnd )
{
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_NormalsListFaceNormal )
{
	std::shared_ptr< AseFileContext > l_pContext = std::static_pointer_cast< AseFileContext >( p_pContext );
	uint32_t l_uiIndex;
	float l_vertex[3];
	FaceSPtr l_pFace;
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );
	RetrieveVertex( l_uiIndex, l_vertex, l_strParams );
	l_pFace = l_pContext->pSubmesh->GetFace( l_uiIndex );
	Vertex::SetNormal( l_pContext->pSubmesh->GetPoint( l_pFace->GetVertexIndex( 0 ) ), l_vertex[0], l_vertex[1], l_vertex[2] );
	Vertex::SetNormal( l_pContext->pSubmesh->GetPoint( l_pFace->GetVertexIndex( 1 ) ), l_vertex[0], l_vertex[1], l_vertex[2] );
	Vertex::SetNormal( l_pContext->pSubmesh->GetPoint( l_pFace->GetVertexIndex( 2 ) ), l_vertex[0], l_vertex[1], l_vertex[2] );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_NormalsListVertexNormal )
{
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Ase, AseParser_NormalsListEnd )
{
}
END_ATTRIBUTE_POP()

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif