#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/DxBuffer.hpp"
#include "Dx9RenderSystem/DxRenderSystem.hpp"
//#include "Dx9RenderSystem/DirectX9.hpp"

using namespace Castor3D;
using namespace Dx9Render;

//*************************************************************************************************

uint32_t DxBufferObjectBase::BuffersCount = 0;

//*************************************************************************************************

DxIndexBuffer :: DxIndexBuffer( HardwareBufferPtr p_pBuffer)
	:	DxBufferObject< uint32_t, IDirect3DIndexBuffer9 >( p_pBuffer)
{
}

DxIndexBuffer :: ~DxIndexBuffer()
{
}

bool DxIndexBuffer :: Create()
{
	return true;
}

void DxIndexBuffer :: Destroy()
{
	DxBufferObject< uint32_t, IDirect3DIndexBuffer9 >::DoDestroy();
}

void DxIndexBuffer :: Cleanup()
{
	DxBufferObject< uint32_t, IDirect3DIndexBuffer9 >::DoCleanup();
}

bool DxIndexBuffer :: Initialise( eBUFFER_ACCESS_TYPE p_eType, eBUFFER_ACCESS_NATURE p_eNature, Castor3D::ShaderProgramBaseSPtr CU_PARAM_UNUSED( p_pProgram ) )
{
	bool l_bReturn = false;

	if( m_pBuffer )
	{
		m_pDevice = static_cast< DxRenderSystem *>( m_pBuffer->GetRenderSystem() )->GetDevice();
		UINT l_uiSize = UINT( m_pBuffer->GetSize() );

		if( l_uiSize )
		{
			l_bReturn = dxCheckError( m_pDevice->CreateIndexBuffer( UINT( l_uiSize * sizeof( uint32_t ) ), DirectX9::GetBufferFlags( p_eType | p_eNature ), D3DFMT_INDEX32, D3DPOOL_DEFAULT, &m_pBufferObject, NULL ), "IDirect3DDevice9 :: CreateIndexBuffer" );
		}
		else
		{
			l_bReturn = false;
		}

		if( l_bReturn && l_uiSize )
		{
			DxBufferObject< uint32_t, IDirect3DIndexBuffer9 >::DoFill( m_pBuffer->data(), m_pBuffer->GetSize() );
			m_pBuffer->Assign();
			l_bReturn = true;
		}
	}

	return l_bReturn;
}

bool DxIndexBuffer :: Bind()
{
	bool l_bReturn = true;

	if( m_pBuffer && m_pBuffer->IsAssigned() )
	{
		l_bReturn = dxCheckError( m_pDevice->SetIndices( m_pBufferObject ), "IDirect3DDevice9 :: SetIndices" );
	}

	return l_bReturn;
}

void DxIndexBuffer :: Unbind()
{
}

uint32_t * DxIndexBuffer :: Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags)
{
	return DxBufferObject< uint32_t, IDirect3DIndexBuffer9 >::DoLock( p_uiOffset, p_uiCount, p_uiFlags );
}

void DxIndexBuffer :: Unlock()
{
	DxBufferObject< uint32_t, IDirect3DIndexBuffer9 >::DoUnlock();
}

//*************************************************************************************************

DxVertexBuffer :: DxVertexBuffer( BufferDeclaration const & p_declaration, HardwareBufferPtr p_pBuffer)
	:	DxBufferObject< uint8_t, IDirect3DVertexBuffer9 >( p_pBuffer)
	,	m_pDx9Declaration	( NULL)
	,	m_declaration		( p_declaration)
{
}

DxVertexBuffer :: ~DxVertexBuffer()
{
}

bool DxVertexBuffer :: Create()
{
	return true;
}

void DxVertexBuffer :: Destroy()
{
	DxBufferObject< uint8_t, IDirect3DVertexBuffer9 >::DoDestroy();
}

bool DxVertexBuffer :: Initialise( eBUFFER_ACCESS_TYPE p_eType, eBUFFER_ACCESS_NATURE p_eNature, Castor3D::ShaderProgramBaseSPtr p_pProgram )
{
	bool l_bReturn = m_pBuffer != nullptr;

	if( l_bReturn )
	{
		UINT l_uiSize = UINT( m_pBuffer->GetSize() );

		m_pDevice = static_cast< DxRenderSystem *>( m_pBuffer->GetRenderSystem() )->GetDevice();
		m_pBuffer->Unassign();
		l_bReturn = DoCreateLayout();

		// Cr�ation du vertex buffer
		if( l_bReturn )
		{
			l_bReturn = dxCheckError( m_pDevice->CreateVertexBuffer( l_uiSize * sizeof( uint8_t ), DirectX9::GetBufferFlags( p_eType | p_eNature ), 0, D3DPOOL_DEFAULT, &m_pBufferObject, NULL ), "IDirect3DDevice9 :: CreateVertexBuffer" );
		}

		if( l_bReturn && l_uiSize )
		{
			DxBufferObject< uint8_t, IDirect3DVertexBuffer9 >::DoFill( m_pBuffer->data(), m_pBuffer->GetSize() );
			m_pBuffer->Assign();
			l_bReturn = true;
		}
	}

	return l_bReturn;
}

void DxVertexBuffer :: Cleanup()
{
	DxBufferObject< uint8_t, IDirect3DVertexBuffer9 >::DoCleanup();
	SafeRelease( m_pDx9Declaration );
}

bool DxVertexBuffer :: Bind()
{
	bool l_bReturn = m_pBuffer && m_pBuffer->IsAssigned();
	
	if( l_bReturn )
	{
		l_bReturn = dxCheckError( m_pDevice->SetStreamSource( 0, m_pBufferObject, 0, m_declaration.GetStride() ), "IDirect3DDevice9 :: SetStreamSource" );
	}

	if( l_bReturn )
	{
		l_bReturn = dxCheckError( m_pDevice->SetVertexDeclaration( m_pDx9Declaration ), "IDirect3DDevice9 :: SetVertexDeclaration" );
	}

	return l_bReturn;
}

void DxVertexBuffer :: Unbind()
{
}

uint8_t * DxVertexBuffer :: Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags )
{
	return DxBufferObject< uint8_t, IDirect3DVertexBuffer9 >::DoLock( p_uiOffset, p_uiCount, p_uiFlags );
}

void DxVertexBuffer :: Unlock()
{
	DxBufferObject< uint8_t, IDirect3DVertexBuffer9 >::DoUnlock();
}

bool DxVertexBuffer :: DoCreateLayout()
{
	std::vector< D3DVERTEXELEMENT9 > l_arrayDxElements;
	D3DVERTEXELEMENT9 l_dx9EndElement = D3DDECL_END();

	for( BufferDeclaration::BufferElementDeclarationArrayConstIt l_it = m_declaration.Begin(); l_it != m_declaration.End(); ++l_it )
	{
		D3DVERTEXELEMENT9 l_dx9CurrentElement;
		l_dx9CurrentElement.Stream = WORD( l_it->m_uiStream );
		l_dx9CurrentElement.Offset = WORD( l_it->m_uiOffset );
		l_dx9CurrentElement.Method = D3DDECLMETHOD_DEFAULT;

		switch( l_it->m_eUsage )
		{
		case eELEMENT_USAGE_POSITION:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_POSITION;	l_dx9CurrentElement.UsageIndex = 0;	break;
		case eELEMENT_USAGE_NORMAL:		l_dx9CurrentElement.Usage = D3DDECLUSAGE_NORMAL;	l_dx9CurrentElement.UsageIndex = 0;	break;
		case eELEMENT_USAGE_TANGENT:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_TANGENT;	l_dx9CurrentElement.UsageIndex = 0;	break;
		case eELEMENT_USAGE_BITANGENT:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_BINORMAL;	l_dx9CurrentElement.UsageIndex = 0;	break;
		case eELEMENT_USAGE_DIFFUSE:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_COLOR;		l_dx9CurrentElement.UsageIndex = 0;	break;
		case eELEMENT_USAGE_TEXCOORDS0:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_TEXCOORD;	l_dx9CurrentElement.UsageIndex = 0;	break;
		case eELEMENT_USAGE_TEXCOORDS1:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_TEXCOORD;	l_dx9CurrentElement.UsageIndex = 1;	break;
		case eELEMENT_USAGE_TEXCOORDS2:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_TEXCOORD;	l_dx9CurrentElement.UsageIndex = 2;	break;
		case eELEMENT_USAGE_TEXCOORDS3:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_TEXCOORD;	l_dx9CurrentElement.UsageIndex = 3;	break;
		}

		switch( l_it->m_eDataType )
		{
		case eELEMENT_TYPE_1FLOAT:	l_dx9CurrentElement.Type = D3DDECLTYPE_FLOAT1;		break;
		case eELEMENT_TYPE_2FLOATS:	l_dx9CurrentElement.Type = D3DDECLTYPE_FLOAT2;		break;
		case eELEMENT_TYPE_3FLOATS:	l_dx9CurrentElement.Type = D3DDECLTYPE_FLOAT3;		break;
		case eELEMENT_TYPE_4FLOATS:	l_dx9CurrentElement.Type = D3DDECLTYPE_FLOAT4;		break;
		case eELEMENT_TYPE_COLOUR:	l_dx9CurrentElement.Type = D3DDECLTYPE_D3DCOLOR;	break;
		}

		l_arrayDxElements.push_back( l_dx9CurrentElement );
	}

	// Ajout de l'�l�ment marquant la fin du tableau
	l_arrayDxElements.push_back( l_dx9EndElement );
	// Cr�ation de la d�claration
	return dxCheckError( m_pDevice->CreateVertexDeclaration( &l_arrayDxElements[0], &m_pDx9Declaration ), "IDirect3DDevice9 :: CreateVertexDeclaration" );
}

//******************************************************************************

DxGeometryBuffers :: DxGeometryBuffers( DxRenderSystem * p_pRenderSystem, VertexBufferSPtr p_pVertexBuffer, IndexBufferSPtr p_pIndexBuffer )
	:	GeometryBuffers	( p_pVertexBuffer, p_pIndexBuffer, nullptr	)
	,	m_pRenderSystem	( p_pRenderSystem							)
{
}

DxGeometryBuffers :: ~DxGeometryBuffers()
{
}

bool DxGeometryBuffers :: Draw( eTOPOLOGY p_ePrimitiveType, ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex )
{
	VertexBufferSPtr l_pVtxBuffer( m_pVertexBuffer );
	IndexBufferSPtr l_pIdxBuffer( m_pIndexBuffer );

	bool l_bReturn = l_pVtxBuffer->Bind();

	if( l_bReturn )
	{
		UINT l_uiSize = UINT( l_pVtxBuffer->GetSize() / l_pVtxBuffer->GetDeclaration().GetStride() );

		if( l_pIdxBuffer )
		{
			l_bReturn = l_pIdxBuffer->Bind();

			if( l_bReturn )
			{
				m_pRenderSystem->GetDevice()->DrawIndexedPrimitive( DirectX9::Get( p_ePrimitiveType ), 0, 0, l_uiSize, p_uiIndex, p_uiSize / 3 );
				l_pIdxBuffer->Unbind();
			}
		}
		else
		{
			m_pRenderSystem->GetDevice()->DrawPrimitive( DirectX9::Get( p_ePrimitiveType ), p_uiIndex, p_uiSize / 3 );
		}

		l_pVtxBuffer->Unbind();
	}

	return l_bReturn;
}

bool DxGeometryBuffers :: DrawInstanced( eTOPOLOGY p_ePrimitiveType, ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex, uint32_t p_uiCount )
{
	VertexBufferSPtr l_pVtxBuffer( m_pVertexBuffer );
	IndexBufferSPtr l_pIdxBuffer( m_pIndexBuffer );

	bool l_bReturn = l_pVtxBuffer->Bind();

	if( l_bReturn )
	{
		UINT l_uiSize = UINT( l_pVtxBuffer->GetSize() / l_pVtxBuffer->GetDeclaration().GetStride() );

		if( l_pIdxBuffer )
		{
			l_bReturn = l_pIdxBuffer->Bind();

			if( l_bReturn )
			{
				m_pRenderSystem->GetDevice()->DrawIndexedPrimitive( DirectX9::Get( p_ePrimitiveType ), 0, 0, l_uiSize, p_uiIndex, p_uiSize / 3 );
				l_pIdxBuffer->Unbind();
			}
		}
		else
		{
			m_pRenderSystem->GetDevice()->DrawPrimitive( DirectX9::Get( p_ePrimitiveType ), p_uiIndex, p_uiSize / 3 );
		}

		l_pVtxBuffer->Unbind();
	}

	return l_bReturn;
}

bool DxGeometryBuffers :: Initialise()
{
	return true;
}

void DxGeometryBuffers :: Cleanup()
{
}

//*************************************************************************************************