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

#ifndef WXDLGANIMATION_HTLB_H_INCLUDED
#define WXDLGANIMATION_HTLB_H_INCLUDED

#include "colhtmllbox.h"


class cAnimation;
class cBoneAnimation;

////////////////////////////////////////////////////////////////////////
//
//  wxBonesListBox
//
////////////////////////////////////////////////////////////////////////

class wxBonesListBox: public wxColourHtmlListBox {
private:
    cAnimation* m_contents;
public:
    wxBonesListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0):
        wxColourHtmlListBox(parent, id, pos, size, style),
        m_contents(NULL) {}
    virtual void Init(cAnimation *contents) {
        m_contents = contents;
        UpdateContents();
    }
    void UpdateContents();
    virtual wxString OnGetItem(size_t n) const;
};


////////////////////////////////////////////////////////////////////////
//
//  wxTranslationListBox
//
////////////////////////////////////////////////////////////////////////

class wxTranslationListBox: public wxColourHtmlListBox {
private:
    cBoneAnimation** m_contents;
public:
    wxTranslationListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0):
        wxColourHtmlListBox(parent, id, pos, size, style),
        m_contents(NULL) {}
    virtual void Init(cBoneAnimation **contents) {
        m_contents = contents;
        UpdateContents();
    }
    void UpdateContents();
    virtual wxString OnGetItem(size_t n) const;
};


////////////////////////////////////////////////////////////////////////
//
//  wxRotationListBox
//
////////////////////////////////////////////////////////////////////////

class wxRotationListBox: public wxColourHtmlListBox {
private:
    cBoneAnimation** m_contents;
public:
    wxRotationListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0):
        wxColourHtmlListBox(parent, id, pos, size, style),
        m_contents(NULL) {}
    virtual void Init(cBoneAnimation **contents) {
        m_contents = contents;
        UpdateContents();
    }
    void UpdateContents();
    virtual wxString OnGetItem(size_t n) const;
};


#endif // WXDLGANIMATION_HTLB_H_INCLUDED
