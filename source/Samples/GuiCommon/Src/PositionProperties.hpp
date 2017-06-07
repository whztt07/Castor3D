/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GUICOMMON_POSITION_PROPERTIES_H___
#define ___GUICOMMON_POSITION_PROPERTIES_H___

#include "AdditionalProperties.hpp"

#include <Graphics/Position.hpp>

GC_PG_NS_DECLARE_VARIANT_DATA( Castor, Position );

namespace GuiCommon
{
	class PositionProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( PositionProperty )

	public:
		PositionProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::Position const & value = Castor::Position() );
		virtual ~PositionProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		inline void SetValueI( Castor::Position const & value )
		{
			m_value = WXVARIANT( value );
		}
	};
}

#endif