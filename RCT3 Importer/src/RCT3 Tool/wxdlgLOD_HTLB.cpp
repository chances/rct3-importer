///////////////////////////////////////////////////////////////////////////////
//
// Subfiles for html listboxes
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
// Note: ressource in misc
//
///////////////////////////////////////////////////////////////////////////////


#include "wxdlgLOD_HTLB.h"

#include "htmlentities.h"
#include "RCT3Structs.h"

////////////////////////////////////////////////////////////////////////
//
//  wxReferenceListBox
//
////////////////////////////////////////////////////////////////////////

void wxLODAnimationListBox::UpdateContents() {
    if (m_contents) {
        SetItemCount(m_contents->animations.size());
        if (GetSelection() >= m_contents->animations.size())
            SetSelection(wxNOT_FOUND);
    } else {
        SetItemCount(0);
        SetSelection(wxNOT_FOUND);
    }
    RefreshAll();
}

wxString wxLODAnimationListBox::OnGetItem(size_t n) const {
    if (!m_contents)
            return wxT("Internal Error");
    return wxT("<font size='2'>")+wxEncodeHtmlEntities(m_contents->animations[n])+wxT("</font>");
}

