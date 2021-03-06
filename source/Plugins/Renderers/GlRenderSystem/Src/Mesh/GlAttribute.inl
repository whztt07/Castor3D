namespace GlRender
{
	//**********************************************************************************************

	template< typename T > struct GlTyper;

	template <> struct GlTyper< int >
	{
		constexpr static GlType Value = GlType::eInt;
	};
	template <> struct GlTyper< uint32_t >
	{
		constexpr static GlType Value = GlType::eUnsignedInt;
	};
	template <> struct GlTyper< short >
	{
		constexpr static GlType Value = GlType::eShort;
	};
	template <> struct GlTyper< uint16_t >
	{
		constexpr static GlType Value = GlType::eUnsignedShort;
	};
	template <> struct GlTyper< char >
	{
		constexpr static GlType Value = GlType::eByte;
	};
	template <> struct GlTyper< uint8_t >
	{
		constexpr static GlType Value = GlType::eUnsignedByte;
	};
	template <> struct GlTyper< float >
	{
		constexpr static GlType Value = GlType::eFloat;
	};
	template <> struct GlTyper< double >
	{
		constexpr static GlType Value = GlType::eDouble;
	};

	//**********************************************************************************************

	template< typename T, uint32_t Count >
	GlVecAttribute< T, Count >::GlVecAttribute( OpenGl & p_gl, castor3d::ShaderProgram const & p_program, uint32_t p_stride, castor::String const & p_attributeName, uint32_t p_divisor )
		: GlAttributeBase( p_gl, p_program, p_stride, p_attributeName, GlTyper< T >::Value, Count, p_divisor )
	{
	}

	template< typename T, uint32_t Count >
	GlVecAttribute< T, Count >::~GlVecAttribute()
	{
	}

	template< typename T, uint32_t Count >
	void GlVecAttribute< T, Count >::bind( bool p_bNormalised )
	{
		getOpenGl().EnableVertexAttribArray( m_attributeLocation );

		if ( m_glType == GlType::eFloat
			 || m_glType == GlType::eDouble
			 || m_glType == GlType::eHalfFloat )
		{
			getOpenGl().VertexAttribPointer( m_attributeLocation, m_count, m_glType, p_bNormalised, m_stride, BUFFER_OFFSET( m_offset ) );
		}
		else
		{
			getOpenGl().VertexAttribPointer( m_attributeLocation, m_count, m_glType, m_stride, BUFFER_OFFSET( m_offset ) );
		}

		if ( m_divisor )
		{
			getOpenGl().VertexAttribDivisor( m_attributeLocation, m_divisor );
		}
	}

	//**********************************************************************************************

	template< typename T, uint32_t Columns, uint32_t Rows >
	GlMatAttribute< T, Columns, Rows >::GlMatAttribute( OpenGl & p_gl, castor3d::ShaderProgram const & p_program, uint32_t p_stride, castor::String const & p_attributeName )
		: GlAttributeBase( p_gl, p_program, p_stride, p_attributeName, GlTyper< T >::Value, Columns, 1 )
	{
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	GlMatAttribute< T, Columns, Rows >::~GlMatAttribute()
	{
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	void GlMatAttribute< T, Columns, Rows >::bind( bool p_bNormalised )
	{
		uint32_t offset = m_offset;
		const uint32_t off = Rows * sizeof( T );

		if ( m_glType == GlType::eInt )
		{
			for ( int i = 0; i < Columns; ++i )
			{
				uint32_t location = uint32_t( m_attributeLocation + i );
				getOpenGl().EnableVertexAttribArray( location );
				getOpenGl().VertexAttribPointer( location, int( Rows ), m_glType, int( m_stride ), BUFFER_OFFSET( offset ) );
				getOpenGl().VertexAttribDivisor( location, m_divisor );
				offset += off;
			}
		}
		else
		{
			for ( int i = 0; i < Columns; ++i )
			{
				uint32_t location = uint32_t( m_attributeLocation + i );
				getOpenGl().EnableVertexAttribArray( location );
				getOpenGl().VertexAttribPointer( location, int( Rows ), m_glType, p_bNormalised, int( m_stride ), BUFFER_OFFSET( offset ) );
				getOpenGl().VertexAttribDivisor( location, m_divisor );
				offset += off;
			}
		}
	}

	//**********************************************************************************************
}
