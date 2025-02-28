///////////////////////////////////////////////////////////////////////////////
//
// Spline Dialog
// Copyright (C) 2008 Tobias Minch
//
// Part of rct3tool
// Copyright 2005 Jonathan Wilson
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
// Parts of this code are based on code written for rct3tool (Copyright 2005
// Jonathan Wilson).
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __wxdlgSpline__
#define __wxdlgSpline__

/**
@file
Subclass of rcdlgSpline, which is generated by wxFormBuilder.
*/

#include "xrc/res_spline.h"

#include "RCT3Structs.h"

/** Implementing rcdlgSpline */
class dlgSpline : public rcdlgSpline {
public:
	/** Constructor */
	dlgSpline( wxWindow* parent, const cImpSpline& spline );
	const cImpSpline& GetSpline() const {
	    return m_spline;
	}
private:
    cImpSpline m_spline;
};

#endif // __wxdlgSpline__
