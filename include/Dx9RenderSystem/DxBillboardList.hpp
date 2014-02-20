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
#ifndef ___Dx9_BillboardList___
#define ___Dx9_BillboardList___

#include "Module_Dx9Render.hpp"
#include <Castor3D/BillboardList.hpp>

namespace Dx9Render
{
	class DxBillboardList : public Castor3D::BillboardList
	{
	public:
		DxBillboardList( Castor3D::SceneRPtr p_pScene, DxRenderSystem * p_pRenderSystem );
		virtual ~DxBillboardList();

	private:
		virtual bool DoInitialise();
	};
}

#endif