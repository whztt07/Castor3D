﻿#include "Colour.hpp"
#include "BinaryFile.hpp"
#include "TextFile.hpp"
#include "Math.hpp"
#include "StringUtils.hpp"

namespace Castor
{
	//*************************************************************************************************

	Colour::BinaryLoader::BinaryLoader()
		:	Loader< Colour, eFILE_TYPE_BINARY, BinaryFile >( File::eOPEN_MODE_DUMMY )
	{
	}

	bool Colour::BinaryLoader::operator()( Colour & p_colour, BinaryFile & p_file )
	{
		bool l_bReturn = true;

		for ( uint8_t i = 0; i < Colour::eCOMPONENT_COUNT && l_bReturn; i++ )
		{
			l_bReturn = p_file.Read( p_colour[Colour::eCOMPONENT( i )] ) == sizeof( float );
		}

		return l_bReturn;
	}

	bool Colour::BinaryLoader::operator()( Colour const & p_colour, BinaryFile & p_file )
	{
		bool l_bReturn = true;

		for ( uint8_t i = 0; i < Colour::eCOMPONENT_COUNT && l_bReturn; i++ )
		{
			l_bReturn = p_file.Write( p_colour[Colour::eCOMPONENT( i )] ) == sizeof( float );
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	Colour::TextLoader::TextLoader( File::eENCODING_MODE p_eEncodingMode )
		:	Loader< Colour, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
	{
	}

	bool Colour::TextLoader::operator()( Colour & p_colour, TextFile & p_file )
	{
		bool l_bReturn;
		String l_strLine;
		l_bReturn = p_file.ReadLine( l_strLine, 1024 ) > 0;
		StringArray l_arraySplitted;

		if ( l_bReturn )
		{
			l_arraySplitted = str_utils::split( l_strLine, cuT( " \t,;" ) );
			l_bReturn = l_arraySplitted.size() >= Colour::eCOMPONENT_COUNT;
		}

		if ( l_bReturn )
		{
			while ( l_arraySplitted.size() > Colour::eCOMPONENT_COUNT )
			{
				l_arraySplitted.erase( l_arraySplitted.begin() );
			}

			for ( uint8_t i = 0; i < Colour::eCOMPONENT_COUNT; i++ )
			{
				p_colour[Colour::eCOMPONENT( i )] = str_utils::to_double( l_arraySplitted[i] );
			}
		}

		return l_bReturn;
	}

	bool Colour::TextLoader::operator()( Colour const & p_colour, TextFile & p_file )
	{
		StringStream l_streamWord;
		l_streamWord.setf( std::ios::boolalpha );
		l_streamWord.setf( std::ios::showpoint );

		for ( Colour::ColourComponentArrayConstIt l_it = p_colour.begin(); l_it != p_colour.end(); ++l_it )
		{
			if ( !l_streamWord.str().empty() )
			{
				l_streamWord << cuT( " " );
			}

			l_streamWord << l_it->value();
		}

		return p_file.Print( 1024, cuT( "%s" ), l_streamWord.str().c_str() ) > 0;
	}

	//*************************************************************************************************

	String Colour::get_predefined_name( ePREDEFINED p_predefined )
	{
		static std::map< ePREDEFINED, String > l_mapNames;
		typedef std::map< ePREDEFINED, String >::iterator MapIt;

		if ( l_mapNames.empty() )
		{
			l_mapNames[	ePREDEFINED_TRANSPARENT_BLACK		] =	cuT( "transp_black"	);
			l_mapNames[	ePREDEFINED_TRANSPARENT_DARKBLUE	] =	cuT( "transp_darkblue"	);
			l_mapNames[	ePREDEFINED_TRANSPARENT_BLUE		] =	cuT( "transp_blue"	);
			l_mapNames[	ePREDEFINED_TRANSPARENT_DARKGREEN	] =	cuT( "transp_darkgreen"	);
			l_mapNames[	ePREDEFINED_TRANSPARENT_GREEN		] =	cuT( "transp_green"	);
			l_mapNames[	ePREDEFINED_TRANSPARENT_DARKRED		] =	cuT( "transp_darkred"	);
			l_mapNames[	ePREDEFINED_TRANSPARENT_RED			] =	cuT( "transp_red"	);
			l_mapNames[	ePREDEFINED_TRANSPARENT_LIGHTBLUE	] =	cuT( "transp_lightblue"	);
			l_mapNames[	ePREDEFINED_TRANSPARENT_LIGHTGREEN	] =	cuT( "transp_lightgreen"	);
			l_mapNames[	ePREDEFINED_TRANSPARENT_LIGHTRED	] =	cuT( "transp_lightred"	);
			l_mapNames[	ePREDEFINED_TRANSPARENT_WHITE		] =	cuT( "transp_white"	);
			l_mapNames[	ePREDEFINED_LOWALPHA_BLACK			] =	cuT( "lowalpha_black"	);
			l_mapNames[	ePREDEFINED_LOWALPHA_DARKBLUE		] =	cuT( "lowalpha_darkblue"	);
			l_mapNames[	ePREDEFINED_LOWALPHA_BLUE			] =	cuT( "lowalpha_blue"	);
			l_mapNames[	ePREDEFINED_LOWALPHA_DARKGREEN		] =	cuT( "lowalpha_darkgreen"	);
			l_mapNames[	ePREDEFINED_LOWALPHA_GREEN			] =	cuT( "lowalpha_green"	);
			l_mapNames[	ePREDEFINED_LOWALPHA_DARKRED		] =	cuT( "lowalpha_darkred"	);
			l_mapNames[	ePREDEFINED_LOWALPHA_RED			] =	cuT( "lowalpha_red"	);
			l_mapNames[	ePREDEFINED_LOWALPHA_LIGHTBLUE		] =	cuT( "lowalpha_lightblue"	);
			l_mapNames[	ePREDEFINED_LOWALPHA_LIGHTGREEN		] =	cuT( "lowalpha_lightgreen"	);
			l_mapNames[	ePREDEFINED_LOWALPHA_LIGHTRED		] =	cuT( "lowalpha_lightred"	);
			l_mapNames[	ePREDEFINED_LOWALPHA_WHITE			] =	cuT( "lowalpha_white"	);
			l_mapNames[	ePREDEFINED_MEDALPHA_BLACK			] =	cuT( "medalpha_black"	);
			l_mapNames[	ePREDEFINED_MEDALPHA_DARKBLUE		] =	cuT( "medalpha_darkblue"	);
			l_mapNames[	ePREDEFINED_MEDALPHA_BLUE			] =	cuT( "medalpha_blue"	);
			l_mapNames[	ePREDEFINED_MEDALPHA_DARKGREEN		] =	cuT( "medalpha_darkgreen"	);
			l_mapNames[	ePREDEFINED_MEDALPHA_GREEN			] =	cuT( "medalpha_green"	);
			l_mapNames[	ePREDEFINED_MEDALPHA_DARKRED		] =	cuT( "medalpha_darkred"	);
			l_mapNames[	ePREDEFINED_MEDALPHA_RED			] =	cuT( "medalpha_red"	);
			l_mapNames[	ePREDEFINED_MEDALPHA_LIGHTBLUE		] =	cuT( "medalpha_lightblue"	);
			l_mapNames[	ePREDEFINED_MEDALPHA_LIGHTGREEN		] =	cuT( "medalpha_lightgreen"	);
			l_mapNames[	ePREDEFINED_MEDALPHA_LIGHTRED		] =	cuT( "medalpha_lightred"	);
			l_mapNames[	ePREDEFINED_MEDALPHA_WHITE			] =	cuT( "medalpha_white"	);
			l_mapNames[	ePREDEFINED_HIGHALPHA_BLACK			] =	cuT( "highalpha_black"	);
			l_mapNames[	ePREDEFINED_HIGHALPHA_DARKBLUE		] =	cuT( "highalpha_darkblue"	);
			l_mapNames[	ePREDEFINED_HIGHALPHA_BLUE			] =	cuT( "highalpha_blue"	);
			l_mapNames[	ePREDEFINED_HIGHALPHA_DARKGREEN		] =	cuT( "highalpha_darkgreen"	);
			l_mapNames[	ePREDEFINED_HIGHALPHA_GREEN			] =	cuT( "highalpha_green"	);
			l_mapNames[	ePREDEFINED_HIGHALPHA_DARKRED		] =	cuT( "highalpha_darkred"	);
			l_mapNames[	ePREDEFINED_HIGHALPHA_RED			] =	cuT( "highalpha_red"	);
			l_mapNames[	ePREDEFINED_HIGHALPHA_LIGHTBLUE		] =	cuT( "highalpha_lightblue"	);
			l_mapNames[	ePREDEFINED_HIGHALPHA_LIGHTGREEN	] =	cuT( "highalpha_lightgreen"	);
			l_mapNames[	ePREDEFINED_HIGHALPHA_LIGHTRED		] =	cuT( "highalpha_lightred"	);
			l_mapNames[	ePREDEFINED_HIGHALPHA_WHITE			] =	cuT( "highalpha_white"	);
			l_mapNames[	ePREDEFINED_FULLALPHA_BLACK			] =	cuT( "black"	);
			l_mapNames[	ePREDEFINED_FULLALPHA_DARKBLUE		] =	cuT( "darkblue"	);
			l_mapNames[	ePREDEFINED_FULLALPHA_BLUE			] =	cuT( "blue"	);
			l_mapNames[	ePREDEFINED_FULLALPHA_DARKGREEN		] =	cuT( "darkgreen"	);
			l_mapNames[	ePREDEFINED_FULLALPHA_GREEN			] =	cuT( "green"	);
			l_mapNames[	ePREDEFINED_FULLALPHA_DARKRED		] =	cuT( "darkred"	);
			l_mapNames[	ePREDEFINED_FULLALPHA_RED			] =	cuT( "red"	);
			l_mapNames[	ePREDEFINED_FULLALPHA_LIGHTBLUE		] =	cuT( "lightblue"	);
			l_mapNames[	ePREDEFINED_FULLALPHA_LIGHTGREEN	] =	cuT( "lightgreen"	);
			l_mapNames[	ePREDEFINED_FULLALPHA_LIGHTRED		] =	cuT( "lightred"	);
			l_mapNames[	ePREDEFINED_FULLALPHA_WHITE			] =	cuT( "white"	);
		}

		MapIt l_it = l_mapNames.find( p_predefined );
		String l_return = cuT( "black" );

		if ( l_it != l_mapNames.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	Colour::ePREDEFINED Colour::get_predefined( String const & p_name )
	{
		static std::map< String, ePREDEFINED > l_mapNames;
		typedef std::map< String, ePREDEFINED >::iterator MapIt;

		if ( l_mapNames.empty() )
		{
			l_mapNames[	cuT( "transp_black"	)] =	ePREDEFINED_TRANSPARENT_BLACK		;
			l_mapNames[	cuT( "transp_darkblue"	)] =	ePREDEFINED_TRANSPARENT_DARKBLUE	;
			l_mapNames[	cuT( "transp_blue"	)] =	ePREDEFINED_TRANSPARENT_BLUE		;
			l_mapNames[	cuT( "transp_darkgreen"	)] =	ePREDEFINED_TRANSPARENT_DARKGREEN	;
			l_mapNames[	cuT( "transp_green"	)] =	ePREDEFINED_TRANSPARENT_GREEN		;
			l_mapNames[	cuT( "transp_darkred"	)] =	ePREDEFINED_TRANSPARENT_DARKRED		;
			l_mapNames[	cuT( "transp_red"	)] =	ePREDEFINED_TRANSPARENT_RED			;
			l_mapNames[	cuT( "transp_lightblue"	)] =	ePREDEFINED_TRANSPARENT_LIGHTBLUE	;
			l_mapNames[	cuT( "transp_lightgreen"	)] =	ePREDEFINED_TRANSPARENT_LIGHTGREEN	;
			l_mapNames[	cuT( "transp_lightred"	)] =	ePREDEFINED_TRANSPARENT_LIGHTRED	;
			l_mapNames[	cuT( "transp_white"	)] =	ePREDEFINED_TRANSPARENT_WHITE		;
			l_mapNames[	cuT( "lowalpha_black"	)] =	ePREDEFINED_LOWALPHA_BLACK			;
			l_mapNames[	cuT( "lowalpha_darkblue"	)] =	ePREDEFINED_LOWALPHA_DARKBLUE		;
			l_mapNames[	cuT( "lowalpha_blue"	)] =	ePREDEFINED_LOWALPHA_BLUE			;
			l_mapNames[	cuT( "lowalpha_darkgreen"	)] =	ePREDEFINED_LOWALPHA_DARKGREEN		;
			l_mapNames[	cuT( "lowalpha_green"	)] =	ePREDEFINED_LOWALPHA_GREEN			;
			l_mapNames[	cuT( "lowalpha_darkred"	)] =	ePREDEFINED_LOWALPHA_DARKRED		;
			l_mapNames[	cuT( "lowalpha_red"	)] =	ePREDEFINED_LOWALPHA_RED			;
			l_mapNames[	cuT( "lowalpha_lightblue"	)] =	ePREDEFINED_LOWALPHA_LIGHTBLUE		;
			l_mapNames[	cuT( "lowalpha_lightgreen"	)] =	ePREDEFINED_LOWALPHA_LIGHTGREEN		;
			l_mapNames[	cuT( "lowalpha_lightred"	)] =	ePREDEFINED_LOWALPHA_LIGHTRED		;
			l_mapNames[	cuT( "lowalpha_white"	)] =	ePREDEFINED_LOWALPHA_WHITE			;
			l_mapNames[	cuT( "medalpha_black"	)] =	ePREDEFINED_MEDALPHA_BLACK			;
			l_mapNames[	cuT( "medalpha_darkblue"	)] =	ePREDEFINED_MEDALPHA_DARKBLUE		;
			l_mapNames[	cuT( "medalpha_blue"	)] =	ePREDEFINED_MEDALPHA_BLUE			;
			l_mapNames[	cuT( "medalpha_darkgreen"	)] =	ePREDEFINED_MEDALPHA_DARKGREEN		;
			l_mapNames[	cuT( "medalpha_green"	)] =	ePREDEFINED_MEDALPHA_GREEN			;
			l_mapNames[	cuT( "medalpha_darkred"	)] =	ePREDEFINED_MEDALPHA_DARKRED		;
			l_mapNames[	cuT( "medalpha_red"	)] =	ePREDEFINED_MEDALPHA_RED			;
			l_mapNames[	cuT( "medalpha_lightblue"	)] =	ePREDEFINED_MEDALPHA_LIGHTBLUE		;
			l_mapNames[	cuT( "medalpha_lightgreen"	)] =	ePREDEFINED_MEDALPHA_LIGHTGREEN		;
			l_mapNames[	cuT( "medalpha_lightred"	)] =	ePREDEFINED_MEDALPHA_LIGHTRED		;
			l_mapNames[	cuT( "medalpha_white"	)] =	ePREDEFINED_MEDALPHA_WHITE			;
			l_mapNames[	cuT( "highalpha_black"	)] =	ePREDEFINED_HIGHALPHA_BLACK			;
			l_mapNames[	cuT( "highalpha_darkblue"	)] =	ePREDEFINED_HIGHALPHA_DARKBLUE		;
			l_mapNames[	cuT( "highalpha_blue"	)] =	ePREDEFINED_HIGHALPHA_BLUE			;
			l_mapNames[	cuT( "highalpha_darkgreen"	)] =	ePREDEFINED_HIGHALPHA_DARKGREEN		;
			l_mapNames[	cuT( "highalpha_green"	)] =	ePREDEFINED_HIGHALPHA_GREEN			;
			l_mapNames[	cuT( "highalpha_darkred"	)] =	ePREDEFINED_HIGHALPHA_DARKRED		;
			l_mapNames[	cuT( "highalpha_red"	)] =	ePREDEFINED_HIGHALPHA_RED			;
			l_mapNames[	cuT( "highalpha_lightblue"	)] =	ePREDEFINED_HIGHALPHA_LIGHTBLUE		;
			l_mapNames[	cuT( "highalpha_lightgreen"	)] =	ePREDEFINED_HIGHALPHA_LIGHTGREEN	;
			l_mapNames[	cuT( "highalpha_lightred"	)] =	ePREDEFINED_HIGHALPHA_LIGHTRED		;
			l_mapNames[	cuT( "highalpha_white"	)] =	ePREDEFINED_HIGHALPHA_WHITE			;
			l_mapNames[	cuT( "black"	)] =	ePREDEFINED_FULLALPHA_BLACK			;
			l_mapNames[	cuT( "darkblue"	)] =	ePREDEFINED_FULLALPHA_DARKBLUE		;
			l_mapNames[	cuT( "blue"	)] =	ePREDEFINED_FULLALPHA_BLUE			;
			l_mapNames[	cuT( "darkgreen"	)] =	ePREDEFINED_FULLALPHA_DARKGREEN		;
			l_mapNames[	cuT( "green"	)] =	ePREDEFINED_FULLALPHA_GREEN			;
			l_mapNames[	cuT( "darkred"	)] =	ePREDEFINED_FULLALPHA_DARKRED		;
			l_mapNames[	cuT( "red"	)] =	ePREDEFINED_FULLALPHA_RED			;
			l_mapNames[	cuT( "lightblue"	)] =	ePREDEFINED_FULLALPHA_LIGHTBLUE		;
			l_mapNames[	cuT( "lightgreen"	)] =	ePREDEFINED_FULLALPHA_LIGHTGREEN	;
			l_mapNames[	cuT( "lightred"	)] =	ePREDEFINED_FULLALPHA_LIGHTRED		;
			l_mapNames[	cuT( "white"	)] =	ePREDEFINED_FULLALPHA_WHITE			;
		}

		MapIt l_it = l_mapNames.find( p_name );
		ePREDEFINED l_return = ePREDEFINED_FULLALPHA_BLACK;

		if ( l_it != l_mapNames.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	Colour::Colour()
		:	m_arrayValues()
		,	m_arrayComponents()
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayValues[i] = 0;
			m_arrayComponents[i].link( &m_arrayValues[i] );
		}
	}

	Colour::Colour( Colour const & p_colour )
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayValues[i] = p_colour.m_arrayValues[i];
			m_arrayComponents[i].link( &m_arrayValues[i] );
		}

		CHECK_INVARIANTS();
	}

	Colour::Colour( Colour && p_colour )
		:	m_arrayValues( std::move( p_colour.m_arrayValues )	)
		,	m_arrayComponents()
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayComponents[i].link( &m_arrayValues[i] );
		}

		CHECK_INVARIANTS();
	}

	Colour & Colour::operator =( Colour const & p_colour )
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayValues[i] = p_colour.m_arrayValues[i];
		}

		CHECK_INVARIANTS();
		return * this;
	}

	Colour & Colour::operator =( Colour && p_colour )
	{
		if ( this != &p_colour )
		{
			m_arrayValues = std::move( p_colour.m_arrayValues	);

			for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
			{
				m_arrayComponents[i].link( &m_arrayValues[i] );
			}

			CHECK_INVARIANTS();
		}

		return *this;
	}

	Colour Colour::from_rgb( Point3ub const & p_ptColour )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], uint8_t( 255 ) );
	}

	Colour Colour::from_bgr( Point3ub const & p_ptColour )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], uint8_t( 255 ) );
	}

	Colour Colour::from_rgba( Point4ub const & p_ptColour )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], p_ptColour[3] );
	}

	Colour Colour::from_abgr( Point4ub const & p_ptColour )
	{
		return from_components( p_ptColour[3], p_ptColour[2], p_ptColour[1], p_ptColour[0] );
	}

	Colour Colour::from_bgra( Point4ub const & p_ptColour )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], p_ptColour[3] );
	}

	Colour Colour::from_argb( Point4ub const & p_ptColour )
	{
		return from_components( p_ptColour[1], p_ptColour[2], p_ptColour[3], p_ptColour[0] );
	}

	Colour Colour::from_rgb( Point3f const & p_ptColour )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], 1.0f );
	}

	Colour Colour::from_bgr( Point3f const & p_ptColour )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], 1.0f );
	}

	Colour Colour::from_rgba( Point4f const & p_ptColour )
	{
		return from_components( p_ptColour[0], p_ptColour[1], p_ptColour[2], p_ptColour[3] );
	}

	Colour Colour::from_abgr( Point4f const & p_ptColour )
	{
		return from_components( p_ptColour[3], p_ptColour[2], p_ptColour[1], p_ptColour[0] );
	}

	Colour Colour::from_bgra( Point4f const & p_ptColour )
	{
		return from_components( p_ptColour[2], p_ptColour[1], p_ptColour[0], p_ptColour[3] );
	}

	Colour Colour::from_argb( Point4f const & p_ptColour )
	{
		return from_components( p_ptColour[1], p_ptColour[2], p_ptColour[3], p_ptColour[0] );
	}

	Colour Colour::from_rgb( uint32_t p_colour )
	{
		float l_fR = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float l_fG = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float l_fB = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		float l_fA = 1.0f;
		return from_components( l_fR, l_fG, l_fB, l_fA );
	}

	Colour Colour::from_bgr( uint32_t p_colour )
	{
		float l_fB = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float l_fG = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float l_fR = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		float l_fA = 1.0f;
		return from_components( l_fR, l_fG, l_fB, l_fA );
	}

	Colour Colour::from_argb( uint32_t p_colour )
	{
		float l_fA = float( ( ( p_colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float l_fR = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float l_fG = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float l_fB = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return from_components( l_fR, l_fG, l_fB, l_fA );
	}

	Colour Colour::from_bgra( uint32_t p_colour )
	{
		float l_fB = float( ( ( p_colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float l_fG = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float l_fR = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float l_fA = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return from_components( l_fR, l_fG, l_fB, l_fA );
	}

	Colour Colour::from_rgba( uint32_t p_colour )
	{
		float l_fR = float( ( ( p_colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float l_fG = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float l_fB = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float l_fA = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return from_components( l_fR, l_fG, l_fB, l_fA );
	}

	Colour Colour::from_abgr( uint32_t p_colour )
	{
		float l_fA = float( ( ( p_colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
		float l_fB = float( ( ( p_colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
		float l_fG = float( ( ( p_colour & 0x0000FF00 ) >>  8 ) ) / 255.0f;
		float l_fR = float( ( ( p_colour & 0x000000FF ) >>  0 ) ) / 255.0f;
		return from_components( l_fR, l_fG, l_fB, l_fA );
	}

	BEGIN_INVARIANT_BLOCK( Colour )
	CHECK_INVARIANT( static_cast< double >( m_arrayValues[eCOMPONENT_RED	] ) >= 0.0 && static_cast< double >( m_arrayValues[eCOMPONENT_RED	] ) <= 1.0 );
	CHECK_INVARIANT( static_cast< double >( m_arrayValues[eCOMPONENT_GREEN	] ) >= 0.0 && static_cast< double >( m_arrayValues[eCOMPONENT_GREEN	] ) <= 1.0 );
	CHECK_INVARIANT( static_cast< double >( m_arrayValues[eCOMPONENT_BLUE	] ) >= 0.0 && static_cast< double >( m_arrayValues[eCOMPONENT_BLUE	] ) <= 1.0 );
	CHECK_INVARIANT( static_cast< double >( m_arrayValues[eCOMPONENT_ALPHA	] ) >= 0.0 && static_cast< double >( m_arrayValues[eCOMPONENT_ALPHA	] ) <= 1.0 );
	END_INVARIANT_BLOCK()

	void Colour::to_rgb( Point3ub & p_ptResult )const
	{
		get( eCOMPONENT_RED	).convert_to( p_ptResult[0] );
		get( eCOMPONENT_GREEN	).convert_to( p_ptResult[1] );
		get( eCOMPONENT_BLUE	).convert_to( p_ptResult[2] );
		CHECK_INVARIANTS();
	}

	void Colour::to_bgr( Point3ub & p_ptResult )const
	{
		get( eCOMPONENT_BLUE	).convert_to( p_ptResult[0] );
		get( eCOMPONENT_GREEN	).convert_to( p_ptResult[1] );
		get( eCOMPONENT_RED	).convert_to( p_ptResult[2] );
		CHECK_INVARIANTS();
	}

	void Colour::to_rgba( Point4ub & p_ptResult )const
	{
		get( eCOMPONENT_RED	).convert_to( p_ptResult[0] );
		get( eCOMPONENT_GREEN	).convert_to( p_ptResult[1] );
		get( eCOMPONENT_BLUE	).convert_to( p_ptResult[2] );
		get( eCOMPONENT_ALPHA	).convert_to( p_ptResult[3] );
		CHECK_INVARIANTS();
	}

	void Colour::to_bgra( Point4ub & p_ptResult )const
	{
		get( eCOMPONENT_BLUE	).convert_to( p_ptResult[0] );
		get( eCOMPONENT_GREEN	).convert_to( p_ptResult[1] );
		get( eCOMPONENT_RED	).convert_to( p_ptResult[2] );
		get( eCOMPONENT_ALPHA	).convert_to( p_ptResult[3] );
		CHECK_INVARIANTS();
	}

	void Colour::to_argb( Point4ub & p_ptResult )const
	{
		get( eCOMPONENT_ALPHA	).convert_to( p_ptResult[0] );
		get( eCOMPONENT_RED	).convert_to( p_ptResult[1] );
		get( eCOMPONENT_GREEN	).convert_to( p_ptResult[2] );
		get( eCOMPONENT_BLUE	).convert_to( p_ptResult[3] );
		CHECK_INVARIANTS();
	}

	void Colour::to_abgr( Point4ub & p_ptResult )const
	{
		get( eCOMPONENT_ALPHA	).convert_to( p_ptResult[0] );
		get( eCOMPONENT_BLUE	).convert_to( p_ptResult[1] );
		get( eCOMPONENT_GREEN	).convert_to( p_ptResult[2] );
		get( eCOMPONENT_RED	).convert_to( p_ptResult[3] );
		CHECK_INVARIANTS();
	}

	void Colour::to_rgb( Point3f & p_ptResult )const
	{
		get( eCOMPONENT_RED	).convert_to( p_ptResult[0] );
		get( eCOMPONENT_GREEN	).convert_to( p_ptResult[1] );
		get( eCOMPONENT_BLUE	).convert_to( p_ptResult[2] );
		CHECK_INVARIANTS();
	}

	void Colour::to_rgba( Point4f & p_ptResult )const
	{
		get( eCOMPONENT_RED	).convert_to( p_ptResult[0] );
		get( eCOMPONENT_GREEN	).convert_to( p_ptResult[1] );
		get( eCOMPONENT_BLUE	).convert_to( p_ptResult[2] );
		get( eCOMPONENT_ALPHA	).convert_to( p_ptResult[3] );
		CHECK_INVARIANTS();
	}

	void Colour::to_argb( Point4f & p_ptResult )const
	{
		get( eCOMPONENT_ALPHA	).convert_to( p_ptResult[0] );
		get( eCOMPONENT_RED	).convert_to( p_ptResult[1] );
		get( eCOMPONENT_GREEN	).convert_to( p_ptResult[2] );
		get( eCOMPONENT_BLUE	).convert_to( p_ptResult[3] );
		CHECK_INVARIANTS();
	}

	void Colour::to_abgr( Point4f & p_ptResult )const
	{
		get( eCOMPONENT_ALPHA	).convert_to( p_ptResult[0] );
		get( eCOMPONENT_BLUE	).convert_to( p_ptResult[1] );
		get( eCOMPONENT_GREEN	).convert_to( p_ptResult[2] );
		get( eCOMPONENT_RED	).convert_to( p_ptResult[3] );
		CHECK_INVARIANTS();
	}

	void Colour::to_bgra( Point4f & p_ptResult )const
	{
		get( eCOMPONENT_BLUE	).convert_to( p_ptResult[0] );
		get( eCOMPONENT_GREEN	).convert_to( p_ptResult[1] );
		get( eCOMPONENT_RED	).convert_to( p_ptResult[2] );
		get( eCOMPONENT_ALPHA	).convert_to( p_ptResult[3] );
		CHECK_INVARIANTS();
	}

	uint32_t Colour::to_rgb()const
	{
		uint32_t l_ulR = 0;
		uint32_t l_ulG = 0;
		uint32_t l_ulB = 0;
		get( eCOMPONENT_RED	).convert_to( l_ulR );
		get( eCOMPONENT_GREEN	).convert_to( l_ulG );
		get( eCOMPONENT_BLUE	).convert_to( l_ulB );
		l_ulR = static_cast< uint32_t >( l_ulR ) << 16;
		l_ulG = static_cast< uint32_t >( l_ulG ) << 8;
		l_ulB = static_cast< uint32_t >( l_ulB ) << 0;
		uint32_t l_ulReturn = l_ulR | l_ulG | l_ulB;
		CHECK_INVARIANTS();
		return l_ulReturn;
	}

	uint32_t Colour::to_bgr()const
	{
		uint32_t l_ulR = 0;
		uint32_t l_ulG = 0;
		uint32_t l_ulB = 0;
		get( eCOMPONENT_RED	).convert_to( l_ulR );
		get( eCOMPONENT_GREEN	).convert_to( l_ulG );
		get( eCOMPONENT_BLUE	).convert_to( l_ulB );
		l_ulR = static_cast< uint32_t >( l_ulR ) << 0;
		l_ulG = static_cast< uint32_t >( l_ulG ) << 8;
		l_ulB = static_cast< uint32_t >( l_ulB ) << 16;
		uint32_t l_ulReturn = l_ulR | l_ulG | l_ulB;
		CHECK_INVARIANTS();
		return l_ulReturn;
	}

	uint32_t Colour::to_argb()const
	{
		uint32_t l_ulR = 0;
		uint32_t l_ulG = 0;
		uint32_t l_ulB = 0;
		uint32_t l_ulA = 0;
		get( eCOMPONENT_ALPHA	).convert_to( l_ulA );
		get( eCOMPONENT_RED	).convert_to( l_ulR );
		get( eCOMPONENT_GREEN	).convert_to( l_ulG );
		get( eCOMPONENT_BLUE	).convert_to( l_ulB );
		l_ulA = static_cast< uint32_t >( l_ulA ) << 24;
		l_ulR = static_cast< uint32_t >( l_ulR ) << 16;
		l_ulG = static_cast< uint32_t >( l_ulG ) << 8;
		l_ulB = static_cast< uint32_t >( l_ulB ) << 0;
		uint32_t l_ulReturn = l_ulR | l_ulG | l_ulB | l_ulA;
		CHECK_INVARIANTS();
		return l_ulReturn;
	}

	uint32_t Colour::to_rgba()const
	{
		uint32_t l_ulR = 0;
		uint32_t l_ulG = 0;
		uint32_t l_ulB = 0;
		uint32_t l_ulA = 0;
		get( eCOMPONENT_RED	).convert_to( l_ulR );
		get( eCOMPONENT_GREEN	).convert_to( l_ulG );
		get( eCOMPONENT_BLUE	).convert_to( l_ulB );
		get( eCOMPONENT_ALPHA	).convert_to( l_ulA );
		l_ulR = static_cast< uint32_t >( l_ulR ) << 24;
		l_ulG = static_cast< uint32_t >( l_ulG ) << 16;
		l_ulB = static_cast< uint32_t >( l_ulB ) << 8;
		l_ulA = static_cast< uint32_t >( l_ulA ) << 0;
		uint32_t l_ulReturn = l_ulR | l_ulG | l_ulB | l_ulA;
		CHECK_INVARIANTS();
		return l_ulReturn;
	}

	uint32_t Colour::to_abgr()const
	{
		uint32_t l_ulR = 0;
		uint32_t l_ulG = 0;
		uint32_t l_ulB = 0;
		uint32_t l_ulA = 0;
		get( eCOMPONENT_ALPHA	).convert_to( l_ulA );
		get( eCOMPONENT_BLUE	).convert_to( l_ulB );
		get( eCOMPONENT_GREEN	).convert_to( l_ulG );
		get( eCOMPONENT_RED	).convert_to( l_ulR );
		l_ulA = static_cast< uint32_t >( l_ulA ) << 24;
		l_ulB = static_cast< uint32_t >( l_ulB ) << 16;
		l_ulG = static_cast< uint32_t >( l_ulG ) << 8;
		l_ulR = static_cast< uint32_t >( l_ulR ) << 0;
		uint32_t l_ulReturn = l_ulR | l_ulG | l_ulB | l_ulA;
		CHECK_INVARIANTS();
		return l_ulReturn;
	}

	uint32_t Colour::to_bgra()const
	{
		uint32_t l_ulR = 0;
		uint32_t l_ulG = 0;
		uint32_t l_ulB = 0;
		uint32_t l_ulA = 0;
		get( eCOMPONENT_BLUE	).convert_to( l_ulB );
		get( eCOMPONENT_GREEN	).convert_to( l_ulG );
		get( eCOMPONENT_RED	).convert_to( l_ulR );
		get( eCOMPONENT_ALPHA	).convert_to( l_ulA );
		l_ulB = static_cast< uint32_t >( l_ulB ) << 24;
		l_ulG = static_cast< uint32_t >( l_ulG ) << 16;
		l_ulR = static_cast< uint32_t >( l_ulR ) << 8;
		l_ulA = static_cast< uint32_t >( l_ulA ) << 0;
		uint32_t l_ulReturn = l_ulR | l_ulG | l_ulB | l_ulA;
		CHECK_INVARIANTS();
		return l_ulReturn;
	}

	Colour & Colour::operator +=( Colour const & p_clrB )
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayComponents[i] += p_clrB[Colour::eCOMPONENT( i )];
		}

		return *this;
	}

	Colour & Colour::operator -=( Colour const & p_clrB )
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayComponents[i] -= p_clrB[Colour::eCOMPONENT( i )];
		}

		return *this;
	}

	Colour & Colour::operator +=( ColourComponent const & p_component )
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayComponents[i] += p_component;
		}

		return *this;
	}

	Colour & Colour::operator -=( ColourComponent const & p_component )
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayComponents[i] -= p_component;
		}

		return *this;
	}

	Colour & Colour::operator *=( ColourComponent const & p_component )
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayComponents[i] *= p_component;
		}

		return *this;
	}

	Colour & Colour::operator /=( ColourComponent const & p_component )
	{
		for ( uint8_t i = 0; i < eCOMPONENT_COUNT; i++ )
		{
			m_arrayComponents[i] /= p_component;
		}

		return *this;
	}

	//*************************************************************************************************

	bool operator ==( Colour const & p_clrA, Colour const & p_clrB )
	{
		bool l_bReturn = true;

		for ( uint8_t i = 0; i < Colour::eCOMPONENT_COUNT && l_bReturn; i++ )
		{
			l_bReturn = p_clrA[Colour::eCOMPONENT( i )] == p_clrB[Colour::eCOMPONENT( i )];
		}

		return l_bReturn;
	}

	bool operator !=( Colour const & p_clrA, Colour const & p_clrB )
	{
		return !operator ==( p_clrA, p_clrB );
	}

	Colour operator +( Colour const & p_clrA, Colour const & p_clrB )
	{
		Colour l_clrResult( p_clrA );
		l_clrResult += p_clrB;
		return l_clrResult;
	}

	Colour operator -( Colour const & p_clrA, Colour const & p_clrB )
	{
		Colour l_clrResult( p_clrA );
		l_clrResult -= p_clrB;
		return l_clrResult;
	}

	//*************************************************************************************************
}