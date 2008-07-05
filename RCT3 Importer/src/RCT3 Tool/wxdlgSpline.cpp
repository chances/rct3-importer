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

#include "wxdlgSpline.h"

#include <wx/valgen.h>
#include "valext.h"
#include "valchb.h"

dlgSpline::dlgSpline( wxWindow* parent, const cImpSpline& spline ): rcdlgSpline(parent), m_spline(spline) {
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

    m_textName->SetValidator(wxExtendedValidator(&m_spline.spline.m_name, false));
    m_choiceCoordinateSystem->SetValidator(wxGenericValidator(reinterpret_cast<int*>(&m_spline.usedorientation)));
    m_cbCyclic->SetValidator(wxCheckBoxValidator<unsigned long>(&m_spline.spline.m_cyclic));
    m_staticPoints->SetLabel(wxString::Format(_("Nodes: %d"), m_spline.spline.m_nodes.size()));

    Fit();
    Layout();
    GetSizer()->SetSizeHints(this);
}
