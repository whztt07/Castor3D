#include "Uniform.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	template<> const castor::String UniformTypeName< UniformType::eBool >::data_type_name = cuT( "bool" );
	template<> const castor::String UniformTypeName< UniformType::eVec2b >::data_type_name = cuT( "bool" );
	template<> const castor::String UniformTypeName< UniformType::eVec3b >::data_type_name = cuT( "bool" );
	template<> const castor::String UniformTypeName< UniformType::eVec4b >::data_type_name = cuT( "bool" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x2b >::data_type_name = cuT( "bool" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x3b >::data_type_name = cuT( "bool" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x4b >::data_type_name = cuT( "bool" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x2b >::data_type_name = cuT( "bool" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x3b >::data_type_name = cuT( "bool" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x4b >::data_type_name = cuT( "bool" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x2b >::data_type_name = cuT( "bool" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x3b >::data_type_name = cuT( "bool" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x4b >::data_type_name = cuT( "bool" );
	template<> const castor::String UniformTypeName< UniformType::eInt >::data_type_name = cuT( "int" );
	template<> const castor::String UniformTypeName< UniformType::eVec2i >::data_type_name = cuT( "int" );
	template<> const castor::String UniformTypeName< UniformType::eVec3i >::data_type_name = cuT( "int" );
	template<> const castor::String UniformTypeName< UniformType::eVec4i >::data_type_name = cuT( "int" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x2i >::data_type_name = cuT( "int" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x3i >::data_type_name = cuT( "int" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x4i >::data_type_name = cuT( "int" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x2i >::data_type_name = cuT( "int" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x3i >::data_type_name = cuT( "int" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x4i >::data_type_name = cuT( "int" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x2i >::data_type_name = cuT( "int" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x3i >::data_type_name = cuT( "int" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x4i >::data_type_name = cuT( "int" );
	template<> const castor::String UniformTypeName< UniformType::eUInt >::data_type_name = cuT( "uint" );
	template<> const castor::String UniformTypeName< UniformType::eVec2ui >::data_type_name = cuT( "uint" );
	template<> const castor::String UniformTypeName< UniformType::eVec3ui >::data_type_name = cuT( "uint" );
	template<> const castor::String UniformTypeName< UniformType::eVec4ui >::data_type_name = cuT( "uint" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x2ui >::data_type_name = cuT( "uint" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x3ui >::data_type_name = cuT( "uint" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x4ui >::data_type_name = cuT( "uint" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x2ui >::data_type_name = cuT( "uint" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x3ui >::data_type_name = cuT( "uint" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x4ui >::data_type_name = cuT( "uint" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x2ui >::data_type_name = cuT( "uint" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x3ui >::data_type_name = cuT( "uint" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x4ui >::data_type_name = cuT( "uint" );
	template<> const castor::String UniformTypeName< UniformType::eFloat >::data_type_name = cuT( "float" );
	template<> const castor::String UniformTypeName< UniformType::eVec2f >::data_type_name = cuT( "float" );
	template<> const castor::String UniformTypeName< UniformType::eVec3f >::data_type_name = cuT( "float" );
	template<> const castor::String UniformTypeName< UniformType::eVec4f >::data_type_name = cuT( "float" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x2f >::data_type_name = cuT( "float" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x3f >::data_type_name = cuT( "float" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x4f >::data_type_name = cuT( "float" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x2f >::data_type_name = cuT( "float" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x3f >::data_type_name = cuT( "float" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x4f >::data_type_name = cuT( "float" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x2f >::data_type_name = cuT( "float" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x3f >::data_type_name = cuT( "float" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x4f >::data_type_name = cuT( "float" );
	template<> const castor::String UniformTypeName< UniformType::eDouble >::data_type_name = cuT( "double" );
	template<> const castor::String UniformTypeName< UniformType::eVec2d >::data_type_name = cuT( "double" );
	template<> const castor::String UniformTypeName< UniformType::eVec3d >::data_type_name = cuT( "double" );
	template<> const castor::String UniformTypeName< UniformType::eVec4d >::data_type_name = cuT( "double" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x2d >::data_type_name = cuT( "double" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x3d >::data_type_name = cuT( "double" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x4d >::data_type_name = cuT( "double" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x2d >::data_type_name = cuT( "double" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x3d >::data_type_name = cuT( "double" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x4d >::data_type_name = cuT( "double" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x2d >::data_type_name = cuT( "double" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x3d >::data_type_name = cuT( "double" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x4d >::data_type_name = cuT( "double" );
	template<> const castor::String UniformTypeName< UniformType::eSampler >::data_type_name = cuT( "int" );

	//*************************************************************************************************

	template<> const castor::String UniformTypeName< UniformType::eBool >::full_type_name = cuT( "bool" );
	template<> const castor::String UniformTypeName< UniformType::eInt >::full_type_name = cuT( "int" );
	template<> const castor::String UniformTypeName< UniformType::eUInt >::full_type_name = cuT( "uint" );
	template<> const castor::String UniformTypeName< UniformType::eFloat >::full_type_name = cuT( "float" );
	template<> const castor::String UniformTypeName< UniformType::eDouble >::full_type_name = cuT( "double" );
	template<> const castor::String UniformTypeName< UniformType::eSampler >::full_type_name = cuT( "sampler" );

	//*************************************************************************************************

	template<> const castor::String UniformTypeName< UniformType::eVec2b >::full_type_name = cuT( "vec2b" );
	template<> const castor::String UniformTypeName< UniformType::eVec3b >::full_type_name = cuT( "vec3b" );
	template<> const castor::String UniformTypeName< UniformType::eVec4b >::full_type_name = cuT( "vec4b" );
	template<> const castor::String UniformTypeName< UniformType::eVec2i >::full_type_name = cuT( "vec2i" );
	template<> const castor::String UniformTypeName< UniformType::eVec3i >::full_type_name = cuT( "vec3i" );
	template<> const castor::String UniformTypeName< UniformType::eVec4i >::full_type_name = cuT( "vec4i" );
	template<> const castor::String UniformTypeName< UniformType::eVec2ui >::full_type_name = cuT( "vec2ui" );
	template<> const castor::String UniformTypeName< UniformType::eVec3ui >::full_type_name = cuT( "vec3ui" );
	template<> const castor::String UniformTypeName< UniformType::eVec4ui >::full_type_name = cuT( "vec4ui" );
	template<> const castor::String UniformTypeName< UniformType::eVec2f >::full_type_name = cuT( "vec2f" );
	template<> const castor::String UniformTypeName< UniformType::eVec3f >::full_type_name = cuT( "vec3f" );
	template<> const castor::String UniformTypeName< UniformType::eVec4f >::full_type_name = cuT( "vec4f" );
	template<> const castor::String UniformTypeName< UniformType::eVec2d >::full_type_name = cuT( "vec2d" );
	template<> const castor::String UniformTypeName< UniformType::eVec3d >::full_type_name = cuT( "vec3d" );
	template<> const castor::String UniformTypeName< UniformType::eVec4d >::full_type_name = cuT( "vec4d" );

	//*************************************************************************************************

	template<> const castor::String UniformTypeName< UniformType::eMat2x2b >::full_type_name = cuT( "mat2x2b" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x3b >::full_type_name = cuT( "mat2x3b" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x4b >::full_type_name = cuT( "mat2x4b" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x2b >::full_type_name = cuT( "mat3x2b" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x3b >::full_type_name = cuT( "mat3x3b" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x4b >::full_type_name = cuT( "mat3x4b" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x2b >::full_type_name = cuT( "mat4x2b" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x3b >::full_type_name = cuT( "mat4x3b" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x4b >::full_type_name = cuT( "mat4x4b" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x2i >::full_type_name = cuT( "mat2x2i" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x3i >::full_type_name = cuT( "mat2x3i" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x4i >::full_type_name = cuT( "mat2x4i" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x2i >::full_type_name = cuT( "mat3x2i" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x3i >::full_type_name = cuT( "mat3x3i" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x4i >::full_type_name = cuT( "mat3x4i" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x2i >::full_type_name = cuT( "mat4x2i" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x3i >::full_type_name = cuT( "mat4x3i" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x4i >::full_type_name = cuT( "mat4x4i" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x2ui >::full_type_name = cuT( "mat2x2ui" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x3ui >::full_type_name = cuT( "mat2x3ui" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x4ui >::full_type_name = cuT( "mat2x4ui" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x2ui >::full_type_name = cuT( "mat3x2ui" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x3ui >::full_type_name = cuT( "mat3x3ui" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x4ui >::full_type_name = cuT( "mat3x4ui" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x2ui >::full_type_name = cuT( "mat4x2ui" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x3ui >::full_type_name = cuT( "mat4x3ui" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x4ui >::full_type_name = cuT( "mat4x4ui" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x2f >::full_type_name = cuT( "mat2x2f" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x3f >::full_type_name = cuT( "mat2x3f" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x4f >::full_type_name = cuT( "mat2x4f" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x2f >::full_type_name = cuT( "mat3x2f" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x3f >::full_type_name = cuT( "mat3x3f" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x4f >::full_type_name = cuT( "mat3x4f" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x2f >::full_type_name = cuT( "mat4x2f" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x3f >::full_type_name = cuT( "mat4x3f" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x4f >::full_type_name = cuT( "mat4x4f" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x2d >::full_type_name = cuT( "mat2x2d" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x3d >::full_type_name = cuT( "mat2x3d" );
	template<> const castor::String UniformTypeName< UniformType::eMat2x4d >::full_type_name = cuT( "mat2x4d" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x2d >::full_type_name = cuT( "mat3x2d" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x3d >::full_type_name = cuT( "mat3x3d" );
	template<> const castor::String UniformTypeName< UniformType::eMat3x4d >::full_type_name = cuT( "mat3x4d" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x2d >::full_type_name = cuT( "mat4x2d" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x3d >::full_type_name = cuT( "mat4x3d" );
	template<> const castor::String UniformTypeName< UniformType::eMat4x4d >::full_type_name = cuT( "mat4x4d" );

	//*************************************************************************************************

	Uniform::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< Uniform >{ p_tabs }
	{
	}

	bool Uniform::TextWriter::operator()( Uniform const & p_variable, TextFile & p_file )
	{
		bool result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "variable\n" ) ) > 0
						&& p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\ttype " ) + p_variable.getFullTypeName() + cuT( "\n" ) ) > 0;
			castor::TextWriter< Uniform >::checkError( result, "Uniform type" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\tname " ) + p_variable.getName() + cuT( "\n" ) ) > 0;
			castor::TextWriter< Uniform >::checkError( result, "Uniform name" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\tvalue " ) + p_variable.getStrValue() + cuT( "\n" ) ) > 0;
			castor::TextWriter< Uniform >::checkError( result, "Uniform value" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*************************************************************************************************

	Uniform::Uniform( uint32_t p_occurences )
		: m_changed( true )
		, m_occurences( p_occurences )
	{
	}

	Uniform::~Uniform()
	{
	}

	void Uniform::setStrValue( String const & p_value, uint32_t p_index )
	{
		REQUIRE( p_index < m_occurences );
		doSetStrValue( p_value, p_index );
		m_changed = true;
	}

	String Uniform::getStrValue( uint32_t p_index )const
	{
		REQUIRE( p_index < m_occurences );
		return doGetStrValue( p_index );
	}

	void Uniform::setStrValues( String const & p_value )
	{
		StringArray values = string::split( p_value, cuT( "|" ) );
		REQUIRE( values.size() == m_occurences );
		uint32_t index = 0u;

		for ( auto value : values )
		{
			if ( index < m_occurences )
			{
				setStrValue( value, index++ );
			}
		}

		m_changed = true;
	}

	String Uniform::getStrValues()const
	{
		String result;
		String separator;

		for ( uint32_t i = 0u; i < m_occurences; ++i )
		{
			result += separator + getStrValue( i );
			separator = "|";
		}

		return result;
	}
}
