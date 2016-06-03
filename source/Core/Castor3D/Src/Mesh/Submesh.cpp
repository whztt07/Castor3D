#include "Submesh.hpp"

#include "Engine.hpp"
#include "MaterialManager.hpp"
#include "ShaderManager.hpp"

#include "Face.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Material/Pass.hpp"
#include "Mesh/Skeleton/BonedVertex.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Vertex.hpp"

#include <BlockTracker.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		template< typename T, typename U >
		void DoCopySubmesh( uint32_t p_count, stINTERLEAVED_VERTEXT< T > const * p_src, stINTERLEAVED_VERTEXT< U > * p_dst )
		{
			for ( uint32_t i{ 0u }; i < p_count; ++i )
			{
				p_dst->m_pos[0] = U( p_src->m_pos[0] );
				p_dst->m_pos[1] = U( p_src->m_pos[1] );
				p_dst->m_pos[2] = U( p_src->m_pos[2] );
				p_dst->m_nml[0] = U( p_src->m_nml[0] );
				p_dst->m_nml[1] = U( p_src->m_nml[1] );
				p_dst->m_nml[2] = U( p_src->m_nml[2] );
				p_dst->m_tan[0] = U( p_src->m_tan[0] );
				p_dst->m_tan[1] = U( p_src->m_tan[1] );
				p_dst->m_tan[2] = U( p_src->m_tan[2] );
				p_dst->m_bin[0] = U( p_src->m_bin[0] );
				p_dst->m_bin[1] = U( p_src->m_bin[1] );
				p_dst->m_bin[2] = U( p_src->m_bin[2] );
				p_dst->m_tex[0] = U( p_src->m_tex[0] );
				p_dst->m_tex[1] = U( p_src->m_tex[1] );
				p_dst->m_tex[2] = U( p_src->m_tex[2] );
				p_dst++;
				p_src++;
			}
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< Submesh >::DoWrite( Submesh const & p_obj )
	{
		bool l_return = true;

		{
			VertexBuffer const & l_buffer = p_obj.GetVertexBuffer();
			size_t l_size = l_buffer.GetSize();
			uint32_t l_stride = l_buffer.GetDeclaration().GetStride();
			uint32_t l_count = uint32_t( l_size / l_stride );
			l_return = DoWriteChunk( l_count, eCHUNK_TYPE_SUBMESH_VERTEX_COUNT, m_chunk );

			if ( l_return )
			{
				stINTERLEAVED_VERTEX const * l_srcbuf = reinterpret_cast< stINTERLEAVED_VERTEX const * >( l_buffer.data() );
				std::vector< stINTERLEAVED_VERTEXT< double > > l_dstbuf( l_count );
				DoCopySubmesh( l_count, l_srcbuf, l_dstbuf.data() );
				l_return = DoWriteChunk( l_dstbuf, eCHUNK_TYPE_SUBMESH_VERTEX, m_chunk );
			}
		}

		if ( l_return && p_obj.HasIndexBuffer() )
		{
			IndexBuffer const & l_buffer = p_obj.GetIndexBuffer();
			uint32_t l_count = l_buffer.GetSize() / 3;
			l_return = DoWriteChunk( l_count, eCHUNK_TYPE_SUBMESH_FACE_COUNT, m_chunk );

			if ( l_return )
			{
				l_return = DoWriteChunk( l_buffer.data(), l_buffer.GetSize(), eCHUNK_TYPE_SUBMESH_FACES, m_chunk );
			}
		}

		if ( l_return && p_obj.HasBonesBuffer() )
		{
			VertexBuffer const & l_buffer = p_obj.GetBonesBuffer();
			uint32_t l_stride = l_buffer.GetDeclaration().GetStride();
			uint32_t l_count = l_buffer.GetSize() / l_stride;
			l_return = DoWriteChunk( l_count, eCHUNK_TYPE_SUBMESH_BONE_COUNT, m_chunk );

			if ( l_return )
			{
				stVERTEX_BONE_DATA const * l_bne = reinterpret_cast< stVERTEX_BONE_DATA const * >( l_buffer.data() );
				l_return = DoWriteChunk( l_bne, l_count, eCHUNK_TYPE_SUBMESH_BONES, m_chunk );
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	bool BinaryParser< Submesh >::DoParse( Submesh & p_obj )
	{
		bool l_return = true;
		String l_name;
		std::vector< stFACE_INDICES > l_faces;
		std::vector< stVERTEX_BONE_DATA > l_bones;
		std::vector< stINTERLEAVED_VERTEXT< double > > l_srcbuf;
		std::vector< uint8_t > l_matrices;
		uint32_t l_count{ 0u };
		uint32_t l_faceCount{ 0u };
		uint32_t l_boneCount{ 0u };
		uint32_t l_mtxCount{ 0u };
		BinaryChunk l_chunk;

		while ( l_return && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case eCHUNK_TYPE_SUBMESH_VERTEX_COUNT:
				l_return = DoParseChunk( l_count, l_chunk );

				if ( l_return )
				{
					l_srcbuf.resize( l_count );
				}

				break;

			case eCHUNK_TYPE_SUBMESH_VERTEX:
				l_return = DoParseChunk( l_srcbuf, l_chunk );

				if ( l_return && !l_srcbuf.empty() )
				{
					std::vector< stINTERLEAVED_VERTEX > l_dstbuf( l_srcbuf.size() );
					DoCopySubmesh( uint32_t( l_srcbuf.size() ), l_srcbuf.data(), l_dstbuf.data() );
					p_obj.SetVerticesData( l_dstbuf );
				}

				break;

			case eCHUNK_TYPE_SUBMESH_BONE_COUNT:
				l_return = DoParseChunk( l_count, l_chunk );

				if ( l_return )
				{
					l_boneCount = l_count;
					l_bones.resize( l_count );
				}

				break;

			case eCHUNK_TYPE_SUBMESH_BONES:
				l_return = DoParseChunk( l_bones, l_chunk );

				if ( l_return && l_boneCount > 0 )
				{
					p_obj.SetBonesData( l_bones );
				}

				l_boneCount = 0u;
				break;

			case eCHUNK_TYPE_SUBMESH_FACE_COUNT:
				l_return = DoParseChunk( l_count, l_chunk );

				if ( l_return )
				{
					l_faceCount = l_count;
					l_faces.resize( l_count );
				}

				break;

			case eCHUNK_TYPE_SUBMESH_FACES:
				l_return = DoParseChunk( l_faces, l_chunk );

				if ( l_return && l_faceCount > 0 )
				{
					p_obj.SetFacesData( l_faces );
				}

				l_faceCount = 0u;
				break;

			default:
				l_return = false;
				break;
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	Submesh::Submesh( Engine & p_engine, MeshRPtr p_mesh, uint32_t p_uiId )
		: OwnedBy< Engine >( p_engine )
		, m_defaultMaterial( p_engine.GetMaterialManager().GetDefaultMaterial() )
		, m_id( p_uiId )
		, m_parentMesh( p_mesh )
		, m_programFlags( 0 )
		, m_initialised( false )
		, m_dirty( true )
		, m_layout( {
						BufferElementDeclaration( ShaderProgram::Position, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_3FLOATS, Vertex::GetOffsetPos() ),
						BufferElementDeclaration( ShaderProgram::Normal, eELEMENT_USAGE_NORMAL, eELEMENT_TYPE_3FLOATS, Vertex::GetOffsetNml() ),
						BufferElementDeclaration( ShaderProgram::Tangent, eELEMENT_USAGE_TANGENT, eELEMENT_TYPE_3FLOATS, Vertex::GetOffsetTan() ),
						BufferElementDeclaration( ShaderProgram::Bitangent, eELEMENT_USAGE_BITANGENT, eELEMENT_TYPE_3FLOATS, Vertex::GetOffsetBin() ),
						BufferElementDeclaration( ShaderProgram::Texture, eELEMENT_USAGE_TEXCOORDS, eELEMENT_TYPE_3FLOATS, Vertex::GetOffsetTex() ),
					} )
	{
	}

	Submesh::~Submesh()
	{
		Cleanup();
	}

	void Submesh::Initialise()
	{
		if ( !m_vertexBuffer )
		{
			DoGenerateBuffers();
		}

		if ( !m_initialised )
		{
			if ( m_vertexBuffer )
			{
				m_initialised = m_vertexBuffer->Create();
				m_initialised &= m_vertexBuffer->Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
			}

			if ( m_initialised && m_indexBuffer )
			{
				m_initialised = m_indexBuffer->Create();
				m_initialised &= m_indexBuffer->Initialise( eBUFFER_ACCESS_TYPE_STREAM, eBUFFER_ACCESS_NATURE_DRAW );
			}

			if ( m_initialised && m_bonesBuffer )
			{
				m_initialised = m_bonesBuffer->Create();
				m_initialised &= m_bonesBuffer->Initialise( eBUFFER_ACCESS_TYPE_STREAM, eBUFFER_ACCESS_NATURE_DRAW );
			}

			if ( m_initialised && m_matrixBuffer )
			{
				m_initialised = m_matrixBuffer->Create();
				m_initialised &= m_matrixBuffer->Initialise( eBUFFER_ACCESS_TYPE_STREAM, eBUFFER_ACCESS_NATURE_DRAW );
			}
		}
	}

	void Submesh::Cleanup()
	{
		m_initialised = false;
		DoDestroyBuffers();
	}

	void Submesh::ComputeContainers()
	{
		auto l_nbVertex{ GetVertexCount() };

		if ( l_nbVertex > 0 )
		{
			Point3r l_min{ std::numeric_limits< real >::max() };
			Point3r l_max{ std::numeric_limits< real >::lowest() };
			Coords3r l_cur;
			auto l_data{ m_vertexBuffer->data() };
			auto l_stride{ m_vertexBuffer->GetDeclaration().GetStride() };

			for ( uint32_t i = 1; i < l_nbVertex; i++ )
			{
				Vertex::GetPosition( l_data, l_cur );

				if ( l_cur[0] > l_max[0] )
				{
					l_max[0] = l_cur[0];
				}

				if ( l_cur[0] < l_min[0] )
				{
					l_min[0] = l_cur[0];
				}

				if ( l_cur[1] > l_max[1] )
				{
					l_max[1] = l_cur[1];
				}

				if ( l_cur[1] < l_min[1] )
				{
					l_min[1] = l_cur[1];
				}

				if ( l_cur[2] > l_max[2] )
				{
					l_max[2] = l_cur[2];
				}

				if ( l_cur[2] < l_min[2] )
				{
					l_min[2] = l_cur[2];
				}
			}

			m_box.Load( l_min, l_max );
			m_sphere.Load( m_box );
			l_data += l_stride;
		}
	}

	uint32_t Submesh::GetFaceCount()const
	{
		return m_indexBuffer ? m_indexBuffer->GetSize() / 3 : 0u;
	}

	uint32_t Submesh::GetVertexCount()const
	{
		return m_vertexBuffer ? m_vertexBuffer->GetSize() / m_layout.GetStride() : 0u;
	}

	void Submesh::SetVerticesData( stINTERLEAVED_VERTEX const * const p_begin, stINTERLEAVED_VERTEX const * const p_end )
	{
		m_vertices = std::vector< stINTERLEAVED_VERTEX >{ p_begin, p_end };
	}

	void Submesh::SetBonesData( stVERTEX_BONE_DATA const * const p_begin, stVERTEX_BONE_DATA const * const p_end )
	{
		m_bones = std::vector< stVERTEX_BONE_DATA >{ p_begin, p_end };
		m_programFlags |= ePROGRAM_FLAG_SKINNING;
	}

	void Submesh::SetFacesData( stFACE_INDICES const * const p_begin, stFACE_INDICES const * const p_end )
	{
		m_faces = std::vector< stFACE_INDICES >{ p_begin, p_end };
	}

	void Submesh::ClearFaces()
	{
		m_faces.clear();

		if ( m_indexBuffer )
		{
			m_indexBuffer->Clear();
		}
	}

	SubmeshSPtr Submesh::Clone()
	{
		SubmeshSPtr l_clone = std::make_shared< Submesh >( *GetEngine(), m_parentMesh, m_id );
		uint32_t l_stride = m_layout.GetStride();

		//On effectue une copie des vertex
		for ( VertexPtrArrayConstIt l_it = m_points.begin(); l_it != m_points.end(); ++l_it )
		{
			BufferElementGroupSPtr l_pVertexSrc = ( *l_it );
			BufferElementGroupSPtr l_pVertexDst = l_clone->AddPoint( 0, 0, 0 );
			memcpy( l_pVertexDst->ptr(), l_pVertexSrc->const_ptr(), l_stride );
		}

		// TODO copier les bones

		for ( uint32_t j = 0; j < GetFaceCount(); j++ )
		{
			auto const & l_faceSrc = GetFace( j );
			l_clone->AddFace( l_faceSrc[0], l_faceSrc[1], l_faceSrc[2] );
		}

		return l_clone;
	}

	void Submesh::ResetGpuBuffers()
	{
		DoDestroyBuffers();
		DoCreateBuffers();
	}

	void Submesh::Draw( GeometryBuffers const & p_geometryBuffers )
	{
		ENSURE( m_initialised );
		uint32_t l_size = m_vertexBuffer->GetSize() / m_layout.GetStride();

		if ( m_indexBuffer )
		{
			l_size = m_indexBuffer->GetSize();
		}

		p_geometryBuffers.Draw( l_size, 0 );
	}

	void Submesh::DrawInstanced( GeometryBuffers const & p_geometryBuffers, uint32_t p_count )
	{
		ENSURE( m_initialised );
		uint32_t l_size = m_vertexBuffer->GetSize() / m_layout.GetStride();

		if ( m_indexBuffer )
		{
			l_size = m_indexBuffer->GetSize();
		}

		p_geometryBuffers.DrawInstanced( l_size, 0, p_count );
	}

	void Submesh::ComputeFacesFromPolygonVertex()
	{
		if ( m_points.size() )
		{
			BufferElementGroupSPtr l_v1 = m_points[0];
			BufferElementGroupSPtr l_v2 = m_points[1];
			BufferElementGroupSPtr l_v3 = m_points[2];
			AddFace( 0, 1, 2 );
			Vertex::SetTexCoord( l_v1, 0.0, 0.0 );
			Vertex::SetTexCoord( l_v2, 0.0, 0.0 );
			Vertex::SetTexCoord( l_v3, 0.0, 0.0 );

			for ( uint32_t i = 2; i < uint32_t( m_points.size() - 1 ); i++ )
			{
				l_v2 = m_points[i];
				l_v3 = m_points[i + 1];
				AddFace( 0, i, i + 1 );
				Vertex::SetTexCoord( l_v2, 0.0, 0.0 );
				Vertex::SetTexCoord( l_v3, 0.0, 0.0 );
			}
		}
	}

	void Submesh::ComputeNormals( bool p_reverted )
	{
		if ( !m_hasNormals )
		{
			Point3r l_vec2m1;
			Point3r l_vec3m1;
			Point3r l_tex2m1;
			Point3r l_tex3m1;
			Point3r l_pt1;
			Point3r l_pt2;
			Point3r l_pt3;
			Point3r l_uv1;
			Point3r l_uv2;
			Point3r l_uv3;
			BufferElementGroupSPtr l_pVtx1;
			BufferElementGroupSPtr l_pVtx2;
			BufferElementGroupSPtr l_pVtx3;
			Point3r l_vFaceNormal;
			Point3r l_vFaceTangent;

			// First we flush normals and tangents
			for ( VertexPtrArray::iterator l_it = m_points.begin(); l_it != m_points.end(); ++l_it )
			{
				l_pVtx1 = *l_it;
				Vertex::SetNormal( l_pVtx1, l_pt1 );
				Vertex::SetTangent( l_pVtx1, l_pt1 );
			}

			Coords3r l_coord;

			// Then we compute normals and tangents
			if ( p_reverted )
			{
				for ( auto const & l_face : m_faces )
				{
					l_pVtx1 = m_points[l_face[0]];
					l_pVtx2 = m_points[l_face[1]];
					l_pVtx3 = m_points[l_face[2]];
					Vertex::GetPosition( l_pVtx1, l_pt1 );
					Vertex::GetPosition( l_pVtx2, l_pt2 );
					Vertex::GetPosition( l_pVtx3, l_pt3 );
					Vertex::GetTexCoord( l_pVtx1, l_uv1 );
					Vertex::GetTexCoord( l_pVtx2, l_uv2 );
					Vertex::GetTexCoord( l_pVtx3, l_uv3 );
					l_vec2m1 = l_pt2 - l_pt1;
					l_vec3m1 = l_pt3 - l_pt1;
					l_tex2m1 = l_uv2 - l_uv1;
					l_tex3m1 = l_uv3 - l_uv1;
					l_vFaceNormal	= -( l_vec3m1 ^ l_vec2m1 );
					l_vFaceTangent	= ( l_vec2m1 * l_tex3m1[1] ) - ( l_vec3m1 * l_tex2m1[1] );
					Vertex::GetNormal( l_pVtx1, l_coord ) += l_vFaceNormal;
					Vertex::GetNormal( l_pVtx2, l_coord ) += l_vFaceNormal;
					Vertex::GetNormal( l_pVtx3, l_coord ) += l_vFaceNormal;
					Vertex::GetTangent( l_pVtx1, l_coord ) += l_vFaceTangent;
					Vertex::GetTangent( l_pVtx2, l_coord ) += l_vFaceTangent;
					Vertex::GetTangent( l_pVtx3, l_coord ) += l_vFaceTangent;
				}
			}
			else
			{
				for ( auto const & l_face : m_faces )
				{
					l_pVtx1 = m_points[l_face[0]];
					l_pVtx2 = m_points[l_face[1]];
					l_pVtx3 = m_points[l_face[2]];
					Vertex::GetPosition( l_pVtx1, l_pt1 );
					Vertex::GetPosition( l_pVtx2, l_pt2 );
					Vertex::GetPosition( l_pVtx3, l_pt3 );
					Vertex::GetTexCoord( l_pVtx1, l_uv1 );
					Vertex::GetTexCoord( l_pVtx2, l_uv2 );
					Vertex::GetTexCoord( l_pVtx3, l_uv3 );
					l_vec2m1 = l_pt2 - l_pt1;
					l_vec3m1 = l_pt3 - l_pt1;
					l_tex2m1 = l_uv2 - l_uv1;
					l_tex3m1 = l_uv3 - l_uv1;
					l_vFaceNormal	= l_vec3m1 ^ l_vec2m1;
					l_vFaceTangent	= ( l_vec3m1 * l_tex2m1[1] ) - ( l_vec2m1 * l_tex3m1[1] );
					Vertex::GetNormal( l_pVtx1, l_coord ) += l_vFaceNormal;
					Vertex::GetNormal( l_pVtx2, l_coord ) += l_vFaceNormal;
					Vertex::GetNormal( l_pVtx3, l_coord ) += l_vFaceNormal;
					Vertex::GetTangent( l_pVtx1, l_coord ) += l_vFaceTangent;
					Vertex::GetTangent( l_pVtx2, l_coord ) += l_vFaceTangent;
					Vertex::GetTangent( l_pVtx3, l_coord ) += l_vFaceTangent;
				}
			}

			// Eventually we normalize the normals and tangents
			for ( auto l_vtx : m_points )
			{
				Coords3r l_value;
				Vertex::GetNormal( l_vtx, l_value );
				point::normalise( l_value );
				Vertex::GetTangent( l_vtx, l_value );
				point::normalise( l_value );
			}

			m_hasNormals = true;
		}
	}

	void Submesh::ComputeNormals( Face const & p_face )
	{
		BufferElementGroupSPtr l_pVtx1, l_pVtx2, l_pVtx3;
		Point3r l_vec2m1;
		Point3r l_vec3m1;
		Point3r l_vFaceNormal;
		Point3r l_pt1;
		Point3r l_pt2;
		Point3r l_pt3;
		l_pVtx1 = m_points[p_face[0]];
		l_pVtx2 = m_points[p_face[1]];
		l_pVtx3 = m_points[p_face[2]];
		Vertex::GetPosition( l_pVtx1, l_pt1 );
		Vertex::GetPosition( l_pVtx2, l_pt2 );
		Vertex::GetPosition( l_pVtx3, l_pt3 );
		l_vec2m1 = l_pt2 - l_pt1;
		l_vec3m1 = l_pt3 - l_pt1;
		l_vFaceNormal = point::get_normalised( l_vec2m1 ^ l_vec3m1 );
		Vertex::SetNormal( l_pVtx1, l_vFaceNormal );
		Vertex::SetNormal( l_pVtx2, l_vFaceNormal );
		Vertex::SetNormal( l_pVtx3, l_vFaceNormal );
		ComputeTangents( p_face );
	}

	void Submesh::ComputeTangents( Face const & p_face )
	{
		BufferElementGroupSPtr l_pVtx1, l_pVtx2, l_pVtx3;
		Point3r l_vec2m1;
		Point3r l_vec3m1;
		Point3r l_vFaceTangent;
		Point3r l_tex2m1;
		Point3r l_tex3m1;
		Point3r l_uv1;
		Point3r l_uv2;
		Point3r l_uv3;
		Point3r l_pt1;
		Point3r l_pt2;
		Point3r l_pt3;
		l_pVtx1 = m_points[p_face[0]];
		l_pVtx2 = m_points[p_face[1]];
		l_pVtx3 = m_points[p_face[2]];
		Vertex::GetPosition( l_pVtx1, l_pt1 );
		Vertex::GetPosition( l_pVtx2, l_pt2 );
		Vertex::GetPosition( l_pVtx3, l_pt3 );
		Vertex::GetTexCoord( l_pVtx1, l_uv1 );
		Vertex::GetTexCoord( l_pVtx2, l_uv2 );
		Vertex::GetTexCoord( l_pVtx3, l_uv3 );
		l_vec2m1 = l_pt2 - l_pt1;
		l_vec3m1 = l_pt3 - l_pt1;
		l_tex2m1 = l_uv2 - l_uv1;
		l_tex3m1 = l_uv3 - l_uv1;
		l_vFaceTangent = point::get_normalised( ( l_vec2m1 * l_tex3m1[1] ) - ( l_vec3m1 * l_tex2m1[1] ) );
		Vertex::SetTangent( l_pVtx1, l_vFaceTangent );
		Vertex::SetTangent( l_pVtx2, l_vFaceTangent );
		Vertex::SetTangent( l_pVtx3, l_vFaceTangent );
	}

	void Submesh::ComputeTangentsFromNormals()
	{
		Point3rArray l_arrayTangents( m_points.size() );

		// Pour chaque vertex, on stocke la somme des tangentes qui peuvent lui être affectées
		for ( auto const & l_face : m_faces )
		{
			BufferElementGroupSPtr	l_pVtx1 = m_points[l_face[0]];
			BufferElementGroupSPtr	l_pVtx2 = m_points[l_face[1]];
			BufferElementGroupSPtr	l_pVtx3 = m_points[l_face[2]];
			Point3r l_pt1;
			Point3r l_pt2;
			Point3r l_pt3;
			Point3r l_uv1;
			Point3r l_uv2;
			Point3r l_uv3;
			Vertex::GetPosition( l_pVtx1, l_pt1 );
			Vertex::GetPosition( l_pVtx2, l_pt2 );
			Vertex::GetPosition( l_pVtx3, l_pt3 );
			Vertex::GetTexCoord( l_pVtx1, l_uv1 );
			Vertex::GetTexCoord( l_pVtx2, l_uv2 );
			Vertex::GetTexCoord( l_pVtx3, l_uv3 );
			Point3r l_vec2m1 = l_pt2 - l_pt1;
			Point3r l_vec3m1 = l_pt3 - l_pt1;
			Point3r l_vec3m2 = l_pt3 - l_pt2;
			Point3r l_tex2m1 = l_uv2 - l_uv1;
			Point3r l_tex3m1 = l_uv3 - l_uv1;
			// Calculates the triangle's area.
			real l_rDirCorrection = l_tex2m1[0] * l_tex3m1[1] - l_tex2m1[1] * l_tex3m1[0];
			Point3r l_vFaceTangent;

			if ( l_rDirCorrection )
			{
				l_rDirCorrection = 1 / l_rDirCorrection;
				// Calculates the face tangent to the current triangle.
				l_vFaceTangent[0] = l_rDirCorrection * ( ( l_vec2m1[0] * l_tex3m1[1] ) + ( l_vec3m1[0] * -l_tex2m1[1] ) );
				l_vFaceTangent[1] = l_rDirCorrection * ( ( l_vec2m1[1] * l_tex3m1[1] ) + ( l_vec3m1[1] * -l_tex2m1[1] ) );
				l_vFaceTangent[2] = l_rDirCorrection * ( ( l_vec2m1[2] * l_tex3m1[1] ) + ( l_vec3m1[2] * -l_tex2m1[1] ) );
			}

			l_arrayTangents[l_face[0]] += l_vFaceTangent;
			l_arrayTangents[l_face[1]] += l_vFaceTangent;
			l_arrayTangents[l_face[2]] += l_vFaceTangent;
		}

		uint32_t i = 0;
		//On effectue la moyennes des tangentes
		std::for_each( l_arrayTangents.begin(), l_arrayTangents.end(), [&]( Point3r & p_tangent )
		{
			Point3r l_normal;
			Vertex::GetNormal( m_points[i], l_normal );
			Point3r l_tangent 	= point::get_normalised( p_tangent );
			l_tangent -= l_normal * point::dot( l_tangent, l_normal );
			Point3r l_bitangent = l_normal ^ l_tangent;
			Vertex::SetTangent(	m_points[i], l_tangent );
			Vertex::SetBitangent(	m_points[i], l_bitangent );
			i++;
		} );
	}

	void Submesh::ComputeTangentsFromBitangents()
	{
		std::for_each( m_points.begin(), m_points.end(), [&]( BufferElementGroupSPtr p_pVertex )
		{
			Point3r l_normal;
			Point3r l_bitangent;
			Vertex::GetNormal(	p_pVertex, l_normal );
			Vertex::GetBitangent( p_pVertex, l_bitangent );
			Point3r l_tangent = l_normal ^ l_bitangent;
			Vertex::SetTangent(	p_pVertex, l_tangent );
		} );
	}

	void Submesh::ComputeBitangents()
	{
		std::for_each( m_points.begin(), m_points.end(), [&]( BufferElementGroupSPtr p_pVertex )
		{
			Point3r l_normal;
			Point3r l_tangent;
			Vertex::GetNormal( p_pVertex, l_normal );
			Vertex::GetTangent( p_pVertex, l_tangent );
			Point3r l_bitangent = l_tangent ^ l_normal;
			Vertex::SetBitangent( p_pVertex, l_bitangent );
		} );
	}

	void Submesh::SortFaces( Point3r const & p_ptCameraPosition )
	{
		ENSURE( m_initialised );
		try
		{
			if ( m_cameraPosition != p_ptCameraPosition )
			{
				if ( m_initialised && m_indexBuffer && m_vertexBuffer )
				{
					IndexBuffer & l_indices = *m_indexBuffer;
					VertexBuffer & l_vertices = *m_vertexBuffer;

					if ( l_vertices.Bind() )
					{
						if ( l_indices.Bind() )
						{
							m_cameraPosition = p_ptCameraPosition;
							uint32_t l_uiIdxSize = l_indices.GetSize();
							uint32_t * l_pIdx = l_indices.Lock( 0, l_uiIdxSize, eACCESS_TYPE_WRITE | eACCESS_TYPE_READ );

							if ( l_pIdx )
							{
								struct stFACE_DISTANCE
								{
									uint32_t m_index[3];
									double m_distance;
								};
								uint32_t l_stride = l_vertices.GetDeclaration().GetStride();
								uint8_t * l_pVtx = l_vertices.data();
								DECLARE_VECTOR( stFACE_DISTANCE, Face );
								FaceArray l_arraySorted;
								l_arraySorted.reserve( l_uiIdxSize / 3 );

								if ( l_pVtx )
								{
									for ( uint32_t * l_it = l_pIdx + 0; l_it < l_pIdx + l_uiIdxSize; l_it += 3 )
									{
										double l_dDistance = 0.0;
										Coords3r l_pVtx1( reinterpret_cast< real * >( &l_pVtx[l_it[0] * l_stride] ) );
										l_dDistance += point::distance_squared( l_pVtx1 - p_ptCameraPosition );
										Coords3r l_pVtx2( reinterpret_cast< real * >( &l_pVtx[l_it[1] * l_stride] ) );
										l_dDistance += point::distance_squared( l_pVtx2 - p_ptCameraPosition );
										Coords3r l_pVtx3( reinterpret_cast< real * >( &l_pVtx[l_it[2] * l_stride] ) );
										l_dDistance += point::distance_squared( l_pVtx3 - p_ptCameraPosition );
										stFACE_DISTANCE l_face = { {l_it[0], l_it[1], l_it[2]}, l_dDistance };
										l_arraySorted.push_back( l_face );
									}

									std::sort( l_arraySorted.begin(), l_arraySorted.end(), []( stFACE_DISTANCE const & p_left, stFACE_DISTANCE const & p_right )
									{
										return p_left.m_distance < p_right.m_distance;
									} );

									for ( FaceArrayConstIt l_it = l_arraySorted.begin(); l_it != l_arraySorted.end(); ++l_it )
									{
										*l_pIdx++ = l_it->m_index[0];
										*l_pIdx++ = l_it->m_index[1];
										*l_pIdx++ = l_it->m_index[2];
									}
								}

								l_indices.Unlock();
							}

							l_indices.Unbind();
						}

						l_vertices.Unbind();
					}
				}
			}
		}
		catch ( Exception const & p_exc )
		{
			Logger::LogError( std::stringstream() << "Submesh::SortFaces - Error: " << p_exc.what() );
		}
	}

	void Submesh::Ref( MaterialSPtr p_material )
	{
		std::map< MaterialSPtr, uint32_t >::iterator l_it = m_instanceCount.find( p_material );

		if ( l_it == m_instanceCount.end() )
		{
			m_instanceCount[p_material] = 0;
			l_it = m_instanceCount.find( p_material );
		}

		l_it->second++;
	}

	void Submesh::UnRef( MaterialSPtr p_material )
	{
		std::map< MaterialSPtr, uint32_t >::iterator l_it = m_instanceCount.find( p_material );

		if ( l_it == m_instanceCount.end() )
		{
			if ( l_it->second )
			{
				l_it->second--;
			}

			if ( !l_it->second )
			{
				m_instanceCount.erase( l_it );
			}
		}
	}

	uint32_t Submesh::GetRefCount( MaterialSPtr p_material )const
	{
		uint32_t l_return = 0;
		std::map< MaterialSPtr, uint32_t >::const_iterator l_it = m_instanceCount.find( p_material );

		if ( l_it != m_instanceCount.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	eTOPOLOGY Submesh::GetTopology()const
	{
		eTOPOLOGY l_return = eTOPOLOGY_COUNT;

		for ( auto l_buffers : m_geometryBuffers )
		{
			l_return = l_buffers->GetTopology();
		}

		return l_return;
	}

	void Submesh::SetTopology( eTOPOLOGY p_value )
	{
		for ( auto l_buffers : m_geometryBuffers )
		{
			l_buffers->SetTopology( p_value );
		}
	}

	GeometryBuffers & Submesh::GetGeometryBuffers( ShaderProgram const & p_program )
	{
		GeometryBuffersSPtr l_buffers;
		auto l_it = std::find_if( std::begin( m_geometryBuffers ), std::end( m_geometryBuffers ), [&p_program]( GeometryBuffersSPtr p_buffers )
		{
			return &p_buffers->GetProgram() == &p_program;
		} );

		if ( l_it == m_geometryBuffers.end() )
		{
			l_buffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, p_program );
			GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [this, l_buffers]()
			{
				l_buffers->Initialise( m_vertexBuffer, m_indexBuffer, m_bonesBuffer, m_matrixBuffer );
			} ) );
			m_geometryBuffers.push_back( l_buffers );
		}
		else
		{
			l_buffers = *l_it;
		}

		return *l_buffers;
	}

	void Submesh::DoCreateBuffers()
	{
		m_vertexBuffer = std::make_shared< VertexBuffer >( *GetEngine(), m_layout );
		m_indexBuffer = std::make_shared< IndexBuffer >( *GetEngine() );

		if ( CheckFlag( GetProgramFlags(), ePROGRAM_FLAG_SKINNING ) && m_parentMesh->GetSkeleton() )
		{
			m_bonesBuffer = std::make_shared< VertexBuffer >( *GetEngine(), BufferDeclaration
			{
				{
					BufferElementDeclaration{ ShaderProgram::BoneIds0, eELEMENT_USAGE_BONE_IDS0, eELEMENT_TYPE_4INTS, 0 },
					BufferElementDeclaration{ ShaderProgram::BoneIds1, eELEMENT_USAGE_BONE_IDS1, eELEMENT_TYPE_4INTS, 16 },
					BufferElementDeclaration{ ShaderProgram::Weights0, eELEMENT_USAGE_BONE_WEIGHTS0, eELEMENT_TYPE_4FLOATS, 32 },
					BufferElementDeclaration{ ShaderProgram::Weights1, eELEMENT_USAGE_BONE_WEIGHTS1, eELEMENT_TYPE_4FLOATS, 48 },
				}
			} );
			ENSURE( m_bonesBuffer->GetDeclaration().GetStride() == BonedVertex::Stride );
		}
		else if ( GetEngine()->GetRenderSystem()->GetGpuInformations().HasInstancing() )
		{
			uint32_t l_count = 0;
			l_count = std::accumulate( m_instanceCount.begin(), m_instanceCount.end(), l_count, [&]( uint32_t p_count, std::pair< MaterialSPtr, uint32_t > const & p_pair )
			{
				return p_count + p_pair.second;
			} );

			if ( l_count > 1 )
			{
				m_programFlags |= ePROGRAM_FLAG_INSTANCIATION;
				m_matrixBuffer = std::make_shared< VertexBuffer >( *GetEngine(), BufferDeclaration
				{
					{
						BufferElementDeclaration{ ShaderProgram::Transform, eELEMENT_USAGE_TRANSFORM, eELEMENT_TYPE_4x4FLOATS, 0 },
					}
				} );
			}
		}
	}

	void Submesh::DoDestroyBuffers()
	{
		m_initialised = false;

		if ( m_vertexBuffer )
		{
			m_vertexBuffer->Cleanup();
			m_vertexBuffer->Destroy();
		}

		if ( m_indexBuffer )
		{
			m_indexBuffer->Cleanup();
			m_indexBuffer->Destroy();
		}

		if ( m_matrixBuffer )
		{
			m_matrixBuffer->Cleanup();
			m_matrixBuffer->Destroy();
		}

		if ( m_bonesBuffer )
		{
			m_bonesBuffer->Cleanup();
			m_bonesBuffer->Destroy();
		}

		for ( auto l_buffers : m_geometryBuffers )
		{
			l_buffers->Cleanup();
		}

		m_geometryBuffers.clear();
	}

	void Submesh::DoGenerateBuffers()
	{
		DoCreateBuffers();
		DoGenerateVertexBuffer();
		DoGenerateIndexBuffer();
		uint32_t l_count = 0;
		l_count = std::accumulate( m_instanceCount.begin(), m_instanceCount.end(), l_count, [&]( uint32_t p_count, std::pair< MaterialSPtr, uint32_t > const & p_pair )
		{
			return p_count + p_pair.second;
		} );

		if ( l_count > 1 )
		{
			m_programFlags |= ePROGRAM_FLAG_INSTANCIATION;
			DoGenerateMatrixBuffer( l_count );
		}

		if ( !m_bones.empty() )
		{
			DoGenerateBonesBuffer();
		}
	}

	void Submesh::DoGenerateVertexBuffer()
	{
		if ( m_vertexBuffer )
		{
			uint8_t * l_buffer = nullptr;
			VertexBuffer & l_vertexBuffer = *m_vertexBuffer;
			uint32_t l_stride = m_layout.GetStride();
			uint32_t l_size = uint32_t( m_points.size() ) * l_stride;
			VertexPtrArrayIt l_itPoints = m_points.begin();

			if ( l_size )
			{
				if ( l_vertexBuffer.GetSize() != l_size )
				{
					l_vertexBuffer.Resize( l_size );
				}

				l_buffer = l_vertexBuffer.data();

				for ( auto l_it : m_pointsData )
				{
					std::memcpy( l_buffer, l_it.data(), l_it.size() );
					l_buffer += l_it.size();
				}

				l_buffer = l_vertexBuffer.data();

				for ( auto l_point : m_points )
				{
					l_point->LinkCoords( l_buffer );
					l_buffer += l_stride;
				}

				m_points.clear();
				m_pointsData.clear();
			}
		}
	}

	void Submesh::DoGenerateIndexBuffer()
	{
		if ( m_indexBuffer )
		{
			FaceSPtr l_pFace;
			uint32_t l_index = 0;
			IndexBuffer & l_indexBuffer = *m_indexBuffer;
			uint32_t l_uiSize = uint32_t( m_faces.size() * 3 );

			if ( l_uiSize )
			{
				if ( l_indexBuffer.GetSize() != l_uiSize )
				{
					l_indexBuffer.Resize( l_uiSize );
				}

				for ( auto const & l_face : m_faces )
				{
					l_indexBuffer.SetElement( l_index++, l_face[0] );
					l_indexBuffer.SetElement( l_index++, l_face[1] );
					l_indexBuffer.SetElement( l_index++, l_face[2] );
				}

				m_faces.clear();
			}
		}
	}

	void Submesh::DoGenerateBonesBuffer()
	{
		if ( m_bonesBuffer )
		{
			uint8_t * l_buffer = nullptr;
			VertexBuffer & l_bonesBuffer = *m_bonesBuffer;
			uint32_t l_stride = BonedVertex::Stride;
			uint32_t l_size = uint32_t( m_bones.size() ) * l_stride;
			auto l_itbones = m_bones.begin();

			if ( l_size )
			{
				if ( l_bonesBuffer.GetSize() != l_size )
				{
					l_bonesBuffer.Resize( l_size );
				}

				l_buffer = l_bonesBuffer.data();

				for ( auto l_it : m_bonesData )
				{
					std::memcpy( l_buffer, l_it.data(), l_it.size() );
					l_buffer += l_it.size();
				}

				l_buffer = l_bonesBuffer.data();

				for ( auto l_point : m_bones )
				{
					l_point->LinkCoords( l_buffer );
					l_buffer += l_stride;
				}

				m_bones.clear();
				m_bonesData.clear();
			}
		}
	}

	void Submesh::DoGenerateMatrixBuffer( uint32_t p_count )
	{
		if ( m_matrixBuffer )
		{
			VertexBuffer & l_matrixBuffer = *m_matrixBuffer;
			uint32_t l_uiSize = p_count * 16 * sizeof( real );

			if ( l_matrixBuffer.GetSize() != l_uiSize )
			{
				l_matrixBuffer.Resize( l_uiSize );
			}
		}
	}
}
