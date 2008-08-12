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

#ifndef WXDLGCREATESCENERY_HTLB_H_INCLUDED
#define WXDLGCREATESCENERY_HTLB_H_INCLUDED

#include "colhtmllbox.h"

class cSCNFile;

////////////////////////////////////////////////////////////////////////
//
//  wxSceneryListBox
//
////////////////////////////////////////////////////////////////////////

class wxSceneryListBox : public wxColourHtmlListBox {
public:
    wxSceneryListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0):
        wxColourHtmlListBox(parent, id, pos, size, style),
        m_contents(NULL), m_mode(1) {}
    virtual void Init(cSCNFile *contents) {
        m_contents = contents;
        UpdateContents();
    }
    void setMode(int mode) {
        m_mode = mode;
        UpdateContents();
    }
protected:
    cSCNFile* m_contents;
    int m_mode;
};

////////////////////////////////////////////////////////////////////////
//
//  wxTextureListBox
//
////////////////////////////////////////////////////////////////////////

class wxTextureListBox : public wxSceneryListBox {
public:
    wxTextureListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0);
    virtual void UpdateContents();
protected:
    virtual wxString OnGetItem(size_t n) const;
};


////////////////////////////////////////////////////////////////////////
//
//  wxReferenceListBox
//
////////////////////////////////////////////////////////////////////////

class wxReferenceListBox : public wxSceneryListBox {
public:
    wxReferenceListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0);
    virtual void UpdateContents();
protected:
    virtual wxString OnGetItem(size_t n) const ;
};


////////////////////////////////////////////////////////////////////////
//
//  wxModelListBox
//
////////////////////////////////////////////////////////////////////////

class wxModelListBox : public wxSceneryListBox {
public:
    wxModelListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0);
    virtual void UpdateContents();

protected:
    virtual wxString OnGetItem(size_t n) const;
};


////////////////////////////////////////////////////////////////////////
//
//  wxAnimatedModelListBox
//
////////////////////////////////////////////////////////////////////////

class wxAnimatedModelListBox : public wxSceneryListBox {
public:
    wxAnimatedModelListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0);
    virtual void UpdateContents();
protected:
    virtual wxString OnGetItem(size_t n) const;
};


////////////////////////////////////////////////////////////////////////
//
//  wxLODListBox
//
////////////////////////////////////////////////////////////////////////

class wxLODListBox : public wxSceneryListBox {
public:
    wxLODListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0);
    virtual void UpdateContents();
protected:
    virtual wxString OnGetItem(size_t n) const;
};


////////////////////////////////////////////////////////////////////////
//
//  wxAnimationListBox
//
////////////////////////////////////////////////////////////////////////

class wxAnimationsListBox : public wxSceneryListBox {
public:
    wxAnimationsListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0);
    virtual void UpdateContents();
protected:
    virtual wxString OnGetItem(size_t n) const ;
};


////////////////////////////////////////////////////////////////////////
//
//  wxSplineListBox
//
////////////////////////////////////////////////////////////////////////

class wxSplineListBox : public wxSceneryListBox {
public:
    wxSplineListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0);
    virtual void UpdateContents();
protected:
    virtual wxString OnGetItem(size_t n) const ;
};


#endif // WXDLGCREATESCENERY_HTLB_H_INCLUDED
