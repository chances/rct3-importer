///////////////////////////////////////////////////////////////////////////////
//
// Decimate Dialog
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

#include "wxdlgDecimate.h"

#include <wx/valgen.h>

#include "valext.h"

dlgDecimate::dlgDecimate( wxWindow* parent ):
    rcdlgDecimate( parent ), m_bones(0), m_frames(0), m_threshold(0.000005), m_rotbail(90) {

    m_choiceBones->SetValidator(wxGenericValidator(&m_bones));
    m_choiceFrames->SetValidator(wxGenericValidator(&m_frames));
    m_textThreshold->SetValidator(wxExtendedValidator(&m_threshold, 6, false, false));
    m_textBailout->SetValidator(wxExtendedValidator(&m_rotbail, 2, false, false));

}
