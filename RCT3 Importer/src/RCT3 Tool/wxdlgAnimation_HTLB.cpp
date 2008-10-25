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

#include "wxdlgAnimation_HTLB.h"

#include <wx/log.h>
#include "htmlentities.h"
#include "RCT3Structs.h"

////////////////////////////////////////////////////////////////////////
//
//  wxBonesListBox
//
////////////////////////////////////////////////////////////////////////

void wxBonesListBox::UpdateContents() {
    if (m_contents) {
        wxLogDebug(wxT("wxBonesListBox::UpdateContents Size:%d"), m_contents->boneanimations.size());
        SetItemCount(m_contents->boneanimations.size());
        if (GetSelection() >= m_contents->boneanimations.size())
            SetSelection(wxNOT_FOUND);
    } else {
        SetItemCount(0);
        SetSelection(wxNOT_FOUND);
    }
    RefreshAll();
    if (GetSelection() != wxNOT_FOUND)
        ScrollToLine(GetSelection());
};

wxString wxBonesListBox::OnGetItem(size_t n) const {
    if (!m_contents)
        return wxT("Internal Error");

    wxLogDebug(wxT("wxBonesListBox::OnGetItem(%d) '%d'"), n, m_contents->boneanimations.size());
    if (n < m_contents->boneanimations.size()) {
        wxString addon = wxString::Format(wxT("%d&nbsp;translations<br>%d&nbsp;rotations"), m_contents->boneanimations[n].translations.size(), m_contents->boneanimations[n].rotations.size());
        wxString ret = wxT("<font size='2'>")+wxEncodeHtmlEntities(m_contents->boneanimations[n].name)
            +HTML_INSET_START+addon+HTML_INSET_END
            +wxT("</font>");
        wxLogDebug(wxT("wxBonesListBox::OnGetItem(%d) '%s'"), n, ret.c_str());
        return ret;
    } else {
        return wxT("Internal Error");
    }
}


////////////////////////////////////////////////////////////////////////
//
//  wxTranslationListBox
//
////////////////////////////////////////////////////////////////////////

void wxTranslationListBox::UpdateContents() {
    if ((m_contents) && (*m_contents)) {
        SetItemCount((*m_contents)->translations.size());
        if (GetSelection() >= (*m_contents)->translations.size())
            SetSelection(wxNOT_FOUND);
    } else {
        SetItemCount(0);
        SetSelection(wxNOT_FOUND);
    }
    RefreshAll();
    if (GetSelection() != wxNOT_FOUND)
        ScrollToLine(GetSelection());
};

wxString wxTranslationListBox::OnGetItem(size_t n) const {
    if (!m_contents)
        return wxT("Internal Error");

    if (*m_contents) {
        if (n < (*m_contents)->translations.size()) {
            return wxT("<font size='2'>")+wxString::Format(wxT("%.6f s"), (*m_contents)->translations[n].v.Time)
                +HTML_INSET_START+wxT("&lt;")+wxString::Format(wxT("%.6f,%.6f,%.6f"), (*m_contents)->translations[n].v.X, (*m_contents)->translations[n].v.Y, (*m_contents)->translations[n].v.Z)+wxT("&gt;")
                +HTML_INSET_END+wxT("</font>");
        } else {
                return wxT("Internal Error");
        }
    } else
        return wxT("");
}

////////////////////////////////////////////////////////////////////////
//
//  wxRotationListBox
//
////////////////////////////////////////////////////////////////////////

void wxRotationListBox::UpdateContents() {
    if ((m_contents) && (*m_contents)) {
        SetItemCount((*m_contents)->rotations.size());
        if (GetSelection() >= (*m_contents)->rotations.size())
            SetSelection(wxNOT_FOUND);
    } else {
        SetItemCount(0);
        SetSelection(wxNOT_FOUND);
    }
    RefreshAll();
    if (GetSelection() != wxNOT_FOUND)
        ScrollToLine(GetSelection());
};

wxString wxRotationListBox::OnGetItem(size_t n) const {
    if (!m_contents)
        return wxT("Internal Error");

    if (*m_contents) {
        if (n < (*m_contents)->rotations.size()) {
            return wxT("<font size='2'>")+wxString::Format(wxT("%.6f s"), (*m_contents)->rotations[n].v.Time)
                +HTML_INSET_START+wxT("&lt;")+wxString::Format(wxT("%.6f,%.6f,%.6f"), (*m_contents)->rotations[n].v.X, (*m_contents)->rotations[n].v.Y, (*m_contents)->rotations[n].v.Z)+wxT("&gt;")
                +HTML_INSET_END+wxT("</font>");
        } else {
                return wxT("Internal Error");
        }
    } else
        return wxT("");
}

