#include "ObjImporter.hpp"

#include "ObjGroup.hpp"

#include <Graphics/Colour.hpp>
#include <Graphics/Image.hpp>

#include <Engine.hpp>
#include <Cache/GeometryCache.hpp>
#include <Cache/MaterialCache.hpp>
#include <Cache/MeshCache.hpp>
#include <Cache/SceneCache.hpp>
#include <Cache/SceneNodeCache.hpp>

#include <Event/Frame/InitialiseEvent.hpp>
#include <Cache/CacheView.hpp>
#include <Material/Material.hpp>
#include <Material/LegacyPass.hpp>
#include <Mesh/Face.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Vertex.hpp>
#include <Mesh/Buffer/Buffer.hpp>
#include <Miscellaneous/Version.hpp>
#include <Plugin/Plugin.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Obj
{
	ObjImporter::ObjImporter( Engine & p_engine )
		: Importer( p_engine )
		, m_collImages( p_engine.GetImageCache() )
		, m_pFile( nullptr )
	{
	}

	ImporterUPtr ObjImporter::Create( Engine & p_engine )
	{
		return std::make_unique< ObjImporter >( p_engine );
	}

	bool ObjImporter::DoImportScene( Scene & p_scene )
	{
		auto l_mesh = p_scene.GetMeshCache().Add( cuT( "Mesh_OBJ" ) );
		bool l_return = DoImportMesh( *l_mesh );

		if ( l_return )
		{
			SceneNodeSPtr l_node = p_scene.GetSceneNodeCache().Add( l_mesh->GetName(), p_scene.GetObjectRootNode() );
			GeometrySPtr l_geometry = p_scene.GetGeometryCache().Add( l_mesh->GetName(), l_node, nullptr );
			l_geometry->SetMesh( l_mesh );
			m_geometries.insert( { l_geometry->GetName(), l_geometry } );
		}

		m_arrayLoadedMaterials.clear();
		m_arrayTextures.clear();
		return l_return;
	}

	bool ObjImporter::DoImportMesh( Mesh & p_mesh )
	{
		bool l_return{ false };

		try
		{
			TextFile l_file( m_fileName, File::OpenMode::eRead );

			if ( l_file.IsOk() )
			{
				m_pFile = &l_file;
				DoReadObjFile( p_mesh );
				m_arrayLoadedMaterials.clear();
				m_arrayTextures.clear();
			}

			if ( m_pThread )
			{
				m_pThread->join();
				m_pThread.reset();
			}

			l_return = true;
		}
		catch ( std::exception & exc )
		{
			Logger::LogWarning( std::stringstream() << "Encountered exception while importing mesh: " << exc.what() );
		}

		return l_return;
	}

	void ObjImporter::DoAddTexture( String const & p_strValue, Pass & p_pass, TextureChannel p_channel )
	{
		Point3f l_offset( 0, 0, 0 );
		Point3f l_scale( 1, 1, 1 );
		Point3f l_turbulence( 0, 0, 0 );
		ImageSPtr l_pImage;
		String l_value( p_strValue );
		DoParseTexParams( l_value, l_offset.ptr(), l_scale.ptr(), l_turbulence.ptr() );
		m_mapOffsets[&p_pass] = l_offset;
		m_mapScales[&p_pass] = l_scale;
		m_mapTurbulences[&p_pass] = l_turbulence;
		Logger::LogDebug( StringStream() << cuT( "-	Texture :    " ) + l_value );
		Logger::LogDebug( StringStream() << cuT( "-	Offset :     " ) << l_offset );
		Logger::LogDebug( StringStream() << cuT( "-	Scale :      " ) << l_scale );
		Logger::LogDebug( StringStream() << cuT( "-	Turbulence : " ) << l_turbulence );

		if ( !l_value.empty() )
		{
			auto l_texture = LoadTexture( Path{ l_value }, p_pass, p_channel );

			if ( l_texture )
			{
				m_arrayTextures.push_back( l_texture->GetTexture() );
			}
		}
	}

	void ObjImporter::DoReadObjFile( Mesh & p_mesh )
	{
		std::ifstream l_file( m_fileName.c_str() );
		std::string l_line;
		std::string l_mtlfile;
		uint32_t l_nv = 0u;
		uint32_t l_nvt = 0u;
		uint32_t l_nvn = 0u;
		uint32_t l_nf = 0u;
		uint32_t l_ntf = 0u;
		uint32_t l_ng = 0u;
		std::vector< uint32_t > l_faces;

		while ( std::getline( l_file, l_line ) )
		{
			string::trim( l_line );
			std::stringstream l_stream{ l_line };
			std::string l_ident;
			l_stream >> l_ident;

			if ( l_ident == "v" )
			{
				if ( l_ntf )
				{
					l_faces.push_back( l_ntf );
					l_ntf = 0u;
				}

				++l_nv;
			}
			else if ( l_ident == "vt" )
			{
				++l_nvt;
			}
			else if ( l_ident == "vn" )
			{
				++l_nvn;
			}
			else if ( l_ident == "f" )
			{
				++l_nf;
				++l_ntf;
			}
			else if ( l_ident == "g" || l_ident == "usemtl" )
			{
				if ( l_ntf )
				{
					l_faces.push_back( l_ntf );
					l_ntf = 0u;
				}
			}
			else if ( l_ident == "mtllib" )
			{
				l_mtlfile = l_line.substr( l_line.find_first_of( " " ) + 1 );
				string::trim( l_mtlfile );
			}
		}

		if ( l_ntf )
		{
			l_faces.push_back( l_ntf );
		}

		// Material description file
		if ( File::FileExists( m_filePath / l_mtlfile ) )
		{
			DoReadMaterials( p_mesh, m_filePath / l_mtlfile );
		}
		else
		{
			Logger::LogWarning( cuT( "Mtl file " ) + m_filePath / l_mtlfile + cuT( " doesn't exist" ) );
		}

		l_file.clear();
		l_file.seekg( 0, std::ios::beg );

		Point3rArray l_allvtx( l_nv );
		Point2rArray l_alltex( l_nvt );
		Point3rArray l_allnml( l_nvn );
		Point3rArray::iterator l_vtxit = l_allvtx.begin();
		Point2rArray::iterator l_texit = l_alltex.begin();
		Point3rArray::iterator l_nmlit = l_allnml.begin();

		std::cout << "    Vertex count: " << l_nv << std::endl;
		std::cout << "    TexCoord count: " << l_nvt << std::endl;
		std::cout << "    Normal count: " << l_nvn << std::endl;
		std::cout << "    Group count: " << l_faces.size() << std::endl;

		while ( std::getline( l_file, l_line ) )
		{
			string::trim( l_line );
			std::stringstream l_stream( l_line );
			std::string l_ident;
			l_stream >> l_ident;

			if ( l_ident == "v" )
			{
				l_stream >> ( *l_vtxit )[0] >> ( *l_vtxit )[1] >> ( *l_vtxit )[2];
				++l_vtxit;
			}
			else if ( l_ident == "vt" )
			{
				l_stream >> ( *l_texit )[0] >> ( *l_texit )[1];
				++l_texit;
			}
			else if ( l_ident == "vn" )
			{
				l_stream >> ( *l_nmlit )[0] >> ( *l_nmlit )[1] >> ( *l_nmlit )[2];
				++l_nmlit;
			}
		}

		l_file.clear();
		l_file.seekg( 0, std::ios::beg );

		InterleavedVertexArray l_vertex{ l_nf * 3 };
		std::vector< FaceIndices > l_index{ l_nf };
		auto l_vit = l_vertex.begin();
		auto l_fit = l_index.begin();
		auto l_facesit = l_faces.end();
		uint32_t i{ 0u };
		std::string l_mtlname;

		while ( std::getline( l_file, l_line ) )
		{
			string::trim( l_line );
			std::stringstream l_stream( l_line );
			std::string l_ident;
			l_stream >> l_ident;

			if ( l_ident == "g" )
			{
				if ( l_facesit == l_faces.end() )
				{
					l_facesit = l_faces.begin();
				}
				else
				{
					DoCreateSubmesh( p_mesh, l_mtlname, std::vector< FaceIndices >{ l_index.begin(), l_fit }, InterleavedVertexArray{ l_vertex.begin(), l_vit } );
					l_vit = l_vertex.begin();
					l_fit = l_index.begin();
					i = 0u;
					++l_facesit;
				}

				std::cout << "    Group faces count: " << *l_facesit << std::endl;
			}
			else if ( l_ident == "usemtl" )
			{
				if ( l_vertex.begin() != l_vit )
				{
					DoCreateSubmesh( p_mesh, l_mtlname, std::vector< FaceIndices >{ l_index.begin(), l_fit }, InterleavedVertexArray{ l_vertex.begin(), l_vit } );
					l_vit = l_vertex.begin();
					l_fit = l_index.begin();
					i = 0u;
					++l_facesit;
				}

				l_stream >> l_mtlname;
			}
			else if ( l_ident == "f" )
			{
				for ( uint32_t i = 0u; i < 3u; i++ )
				{
					std::string l_face;
					l_stream >> l_face;

					size_t l_index1 = l_face.find( '/' );
					std::string l_component = l_face.substr( 0, l_index1 );
					uint32_t l_iv = string::to_int( l_component ) - 1;
					l_vit->m_pos[0] = l_allvtx[l_iv][0];
					l_vit->m_pos[1] = l_allvtx[l_iv][1];
					l_vit->m_pos[2] = l_allvtx[l_iv][2];

					++l_index1;
					size_t l_index2 = l_face.find( '/', l_index1 );
					l_component = l_face.substr( l_index1, l_index2 - l_index1 );

					if ( !l_component.empty() )
					{
						uint32_t l_ivt = string::to_int( l_component ) - 1;
						l_vit->m_tex[0] = l_alltex[l_ivt][0];
						l_vit->m_tex[1] = l_alltex[l_ivt][1];
					}

					++l_index2;
					l_component = l_face.substr( l_index2 );

					if ( !l_component.empty() )
					{
						uint32_t l_ivn = string::to_int( l_component ) - 1;
						l_vit->m_nml[0] = l_allnml[l_ivn][0];
						l_vit->m_nml[1] = l_allnml[l_ivn][1];
						l_vit->m_nml[2] = l_allnml[l_ivn][2];
					}

					++l_vit;
				}

				l_fit->m_index[0] = i++;
				l_fit->m_index[1] = i++;
				l_fit->m_index[2] = i++;
				++l_fit;
			}
		}

		if ( l_vit != l_vertex.begin()
				&& l_fit != l_index.begin() )
		{
			DoCreateSubmesh( p_mesh, l_mtlname, std::vector< FaceIndices > { l_index.begin(), l_fit }, InterleavedVertexArray{ l_vertex.begin(), l_vit } );
		}
	}

	void ObjImporter::DoCreateSubmesh( Castor3D::Mesh & p_mesh, Castor::String const & p_mtlName, std::vector< FaceIndices > && p_faces, Castor3D::InterleavedVertexArray && p_vertex )
	{
		auto l_submesh = p_mesh.CreateSubmesh();
		l_submesh->SetDefaultMaterial( p_mesh.GetScene()->GetMaterialView().Find( p_mtlName ) );
		l_submesh->AddPoints( p_vertex );
		l_submesh->AddFaceGroup( p_faces );
		l_submesh->ComputeTangentsFromNormals();
	}

	void ObjImporter::DoParseTexParams( String & p_strValue, float * p_offset, float * p_scale, float * p_turb )
	{
		String l_strSrc( p_strValue );
		String l_strParam;
		String l_value;
		bool l_bParam = false;
		bool l_bValue = false;
		xchar l_char;
		p_strValue.clear();

		for ( std::size_t i = 0 ; i < l_strSrc.size() ; i++ )
		{
			l_char = l_strSrc[i];

			if ( l_bParam )
			{
				// On a rencontré un tiret, on est en train de récupérer le param
				if ( l_char == cuT( ' ' ) )
				{
					// on est sur un espace, on a fini de récupérer le param
					l_bValue = true;
					l_bParam = false;
				}
				else
				{
					l_strParam += l_char;
				}
			}
			else if ( l_bValue )
			{
				// On a fini de récupérer le param, on est en train de récupérer les valeurs
				if ( l_char == cuT( ' ' ) )
				{
					// On est sur un espace, on vérifie le caractère suivant pour savoir, en fonction du param si c'est une valeur ou pas
					if ( DoIsValidValue( l_strParam, l_strSrc, uint32_t( i + 1 ) ) )
					{
						l_value += l_char;
					}
					else
					{
						// Les caractères suivants ne forment pas une valeur valide pour le param
						l_bValue = false;

						if ( l_strParam == cuT( "s" ) )
						{
							DoParseTexParam( l_strParam + cuT( " " ) + l_value, p_scale );
						}
						else if ( l_strParam == cuT( "o" ) )
						{
							DoParseTexParam( l_strParam + cuT( " " ) + l_value, p_offset );
						}
						else if ( l_strParam == cuT( "t" ) )
						{
							DoParseTexParam( l_strParam + cuT( " " ) + l_value, p_turb );
						}
					}
				}
				else
				{
					l_value += l_char;
				}
			}
			else if ( l_char == cuT( '-' ) )
			{
				// on est sur un tiret, on commence à récupérer le param
				l_strParam.clear();
				l_bParam = true;
			}
			else
			{
				// On n'est nulle part, on ajoute le caractère au résultat
				p_strValue += l_strSrc.substr( i );
				i = l_strSrc.size();
			}
		}
	}

	void ObjImporter::DoParseTexParam( String const & p_strParam, float * p_values )
	{
		StringArray l_arraySplitted = string::split( p_strParam, cuT( " " ) );
		std::size_t l_uiMax = std::min< std::size_t >( 4, l_arraySplitted.size() );

		for ( std::size_t i = 1 ; i < l_uiMax ; i++ )
		{
			p_values[i - 1] = string::to_float( l_arraySplitted[i] );
		}
	}

	bool ObjImporter::DoIsValidValue( String const & p_strParam, String const & p_strSrc, uint32_t p_index )
	{
		bool l_bReturn = false;
		StringArray l_arraySplitted;

		if ( p_index < p_strSrc.size() )
		{
			l_arraySplitted = string::split( p_strSrc.substr( p_index ), cuT( " " ), 2 );

			if ( l_arraySplitted.size() > 1 )
			{
				if ( p_strParam == cuT( "s" ) || p_strParam == cuT( "o" ) || p_strParam == cuT( "t" ) )
				{
					l_bReturn = string::is_floating( l_arraySplitted[0] );
				}
				else if ( p_strParam == cuT( "blendu" ) || p_strParam == cuT( "blendv" ) || p_strParam == cuT( "cc" ) || p_strParam == cuT( "clamp" ) )
				{
					l_bReturn = ( l_arraySplitted[0] == cuT( "on" ) || l_arraySplitted[0] == cuT( "off" ) );
				}
				else if ( p_strParam == cuT( "texres" ) )
				{
					l_bReturn = string::is_integer( l_arraySplitted[0] );
				}
				else if ( p_strParam == cuT( "bm" ) )
				{
					l_bReturn = string::is_floating( l_arraySplitted[0] );
				}
				else if ( p_strParam == cuT( "mm" ) )
				{
					l_bReturn = string::is_floating( l_arraySplitted[0] );
				}
				else if ( p_strParam == cuT( "imfchan" ) )
				{
					l_bReturn = ( l_arraySplitted[0] == cuT( "r" ) || l_arraySplitted[0] == cuT( "g" ) || l_arraySplitted[0] == cuT( "b" ) || l_arraySplitted[0] == cuT( "m" ) || l_arraySplitted[0] == cuT( "l" ) || l_arraySplitted[0] == cuT( "z" ) );
				}
			}
		}

		return l_bReturn;
	}

	void ObjImporter::DoReadMaterials( Mesh & p_mesh, Path const & p_pathMatFile )
	{
		String l_strLine;
		String l_section;
		String l_value;
		StringArray l_arraySplitted;
		LegacyPassSPtr l_pass;
		MaterialSPtr l_material;
		float l_components[3];
		float l_fAlpha = 1.0f;
		bool l_bOpaFound = false;
		TextFile l_matFile( p_pathMatFile, File::OpenMode::eRead );

		while ( l_matFile.IsOk() )
		{
			l_matFile.ReadLine( l_strLine, 1000 );

			if ( !l_strLine.empty() )
			{
				l_arraySplitted = string::split( l_strLine, cuT( " " ), 1 );

				if ( l_arraySplitted.size() >= 1 )
				{
					l_section = l_arraySplitted[0];

					if ( l_arraySplitted.size() > 1 )
					{
						l_value = l_arraySplitted[1];
					}
					else
					{
						l_value.clear();
					}

					string::trim( l_value );
					string::trim( string::to_lower_case( l_section ) );

					if ( l_section == cuT( "newmtl" ) )
					{
						// New material description
						if ( l_pass )
						{
							if ( l_fAlpha < 1.0 )
							{
								l_pass->SetOpacity( l_fAlpha );
							}

							l_fAlpha = 1.0f;
						}

						if ( p_mesh.GetScene()->GetMaterialView().Has( l_value ) )
						{
							l_material = p_mesh.GetScene()->GetMaterialView().Find( l_value );
							REQUIRE( l_material->GetType() == MaterialType::eLegacy );
							l_pass = l_material->GetTypedPass< MaterialType::eLegacy >( 0u );
						}
						else
						{
							l_material.reset();
						}

						if ( !l_material )
						{
							l_material = p_mesh.GetScene()->GetMaterialView().Add( l_value, MaterialType::eLegacy );
							l_material->CreatePass();
							l_pass = l_material->GetTypedPass< MaterialType::eLegacy >( 0u );
							m_arrayLoadedMaterials.push_back( l_material );
						}

						l_pass->SetTwoSided( true );
						l_bOpaFound = false;
						l_fAlpha = 1.0f;
						Logger::LogDebug( cuT( "Material : " ) + l_value );
					}
					else if ( l_section == cuT( "illum" ) )
					{
						// Illumination description
					}
					else if ( l_section == cuT( "kd" ) )
					{
						// Diffuse colour
						StringStream l_stream( l_value );
						l_stream >> l_components[0] >> l_components[1] >> l_components[2];
						l_pass->SetDiffuse( Castor::Colour::from_components( l_components[0], l_components[1], l_components[2], real( 1.0 ) ) );
					}
					else if ( l_section == cuT( "ks" ) )
					{
						// Specular colour
						StringStream l_stream( l_value );
						l_stream >> l_components[0] >> l_components[1] >> l_components[2];
						l_pass->SetSpecular( Castor::Colour::from_components( l_components[0], l_components[1], l_components[2], real( 1.0 ) ) );
					}
					else if ( l_section == cuT( "tr" ) || l_section == cuT( "d" ) )
					{
						// Opacity
						if ( !l_bOpaFound )
						{
							l_bOpaFound = true;
							l_fAlpha = string::to_float( l_value );
						}
					}
					else if ( l_section == cuT( "ns" ) )
					{
						// Shininess
						l_pass->SetShininess( string::to_float( l_value ) );
					}
					else if ( l_section == cuT( "map_kd" ) )
					{
						// Diffuse map
						DoAddTexture( l_value, *l_pass, TextureChannel::eDiffuse );
					}
					else if ( l_section == cuT( "bump" ) || l_section == cuT( "map_bump" ) )
					{
						// Normal map
						DoAddTexture( l_value, *l_pass, TextureChannel::eNormal );
					}
					else if ( l_section == cuT( "map_d" ) || l_section == cuT( "map_opacity" ) )
					{
						// Opacity map
						DoAddTexture( l_value, *l_pass, TextureChannel::eOpacity );
					}
					else if ( l_section == cuT( "refl" ) )
					{
						// Reflection map
					}
					else if ( l_section == cuT( "map_ks" ) )
					{
						// Specular map
						DoAddTexture( l_value, *l_pass, TextureChannel::eSpecular );
					}
					else if ( l_section == cuT( "map_ns" ) )
					{
						// Gloss/Shininess map
						DoAddTexture( l_value, *l_pass, TextureChannel::eGloss );
					}
				}
			}
		}

		if ( l_pass && l_bOpaFound )
		{
			l_pass->SetOpacity( l_fAlpha );
		}
	}
}