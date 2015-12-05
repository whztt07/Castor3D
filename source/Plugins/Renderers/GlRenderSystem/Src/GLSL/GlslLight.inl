/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
namespace GlRender
{
	namespace GLSL
	{
		Light::Light()
			: Type( cuT( "Light " ) )
		{
		}
		
		Light::Light( GlslWriter * p_writer, Castor::String const & p_name )
			: Type( cuT( "Light " ), p_writer, p_name )
		{
		}
		
		Light & Light::operator=( Light const & p_rhs )
		{
			if ( m_writer )
			{
				m_writer->WriteAssign( *this, p_rhs );
			}
			else
			{
				Type::operator=( p_rhs );
				m_writer = p_rhs.m_writer;
			}

			return *this;
		}

		template< typename T >
		Light & Light::operator=( T const & p_rhs )
		{
			UpdateWriter( p_rhs );
			m_writer->WriteAssign( *this, p_rhs );
			return *this;
		}
		
		Vec4 Light::m_v4Ambient()const
		{
			return Vec4( m_writer, m_name + cuT( ".m_v4Ambient" ) );
		}
		
		Vec4 Light::m_v4Diffuse()const
		{
			return Vec4( m_writer, m_name + cuT( ".m_v4Diffuse" ) );
		}
		
		Vec4 Light::m_v4Specular()const
		{
			return Vec4( m_writer, m_name + cuT( ".m_v4Specular" ) );
		}
		
		Vec4 Light::m_v4Position()const
		{
			return Vec4( m_writer, m_name + cuT( ".m_v4Position" ) );
		}
		
		Int Light::m_iType()const
		{
			return Int( m_writer, m_name + cuT( ".m_iType" ) );
		}
		
		Vec3 Light::m_v3Attenuation()const
		{
			return Vec3( m_writer, m_name + cuT( ".m_v3Attenuation" ) );
		}
		
		Mat4 Light::m_mtx4Orientation()const
		{
			return Mat4( m_writer, m_name + cuT( ".m_mtx4Orientation" ) );
		}
		
		Float Light::m_fExponent()const
		{
			return Float( m_writer, m_name + cuT( ".m_fExponent" ) );
		}
		
		Float Light::m_fCutOff()const
		{
			return Float( m_writer, m_name + cuT( ".m_fCutOff" ) );
		}
	}
}
