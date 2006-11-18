///////////////////////////////////////////////////////////////////////////////
//
// Mesh Settings Dialog
// Copyright (C) 2006 Tobias Minch, Jonathan Wilson
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

#ifndef WXDLGMESH_H_INCLUDED
#define WXDLGMESH_H_INCLUDED

#include "wx_pch.h"

#include <wx/xrc/xmlres.h>

#include "RCT3Structs.h"

class dlgMesh : public wxDialog {
protected:
    wxCheckBox* m_checkDontImport;
    wxComboBox* m_cbTextureName;
    wxChoice* m_choiceTextureStyle;
    wxChoice* m_choiceShowTexture;
    wxChoice* m_choiceTextureFlags;
    wxChoice* m_choiceUnknown;
    wxCheckBox* m_checkApplyAll;
    wxButton* m_btLoad;
    wxButton* m_btOk;
    wxButton* m_btCancel;

    cMeshStruct m_ms;
    int m_flags;
    int m_unknown;
    int m_place;
    bool m_applyall;
    bool m_rareadded;

    void OnLoad(wxCommandEvent& event);
    void OnChange(wxCommandEvent& WXUNUSED(event)) {RefreshAll();}
    void OnRDbl(wxMouseEvent& event);

private:
    void InitWidgetsFromXRC(wxWindow *parent) {
        wxXmlResource::Get()->LoadObject(this,parent,_T("dlgMesh"), _T("wxDialog"));
        m_checkDontImport = XRCCTRL(*this,"m_checkDontImport",wxCheckBox);
        m_cbTextureName = XRCCTRL(*this,"m_cbTextureName",wxComboBox);
        m_choiceTextureStyle = XRCCTRL(*this,"m_choiceTextureStyle",wxChoice);
        m_choiceShowTexture = XRCCTRL(*this,"m_choiceShowTexture",wxChoice);
        m_choiceTextureFlags = XRCCTRL(*this,"m_choiceTextureFlags",wxChoice);
        m_choiceUnknown = XRCCTRL(*this,"m_choiceUnknown",wxChoice);
        m_checkApplyAll = XRCCTRL(*this,"m_checkApplyAll",wxCheckBox);
        m_btLoad = XRCCTRL(*this,"m_btLoad",wxButton);
        m_btOk = XRCCTRL(*this,"m_btOk",wxButton);
        m_btCancel = XRCCTRL(*this,"m_btCancel",wxButton);
    }
    DECLARE_EVENT_TABLE()
public:
    dlgMesh(wxWindow *parent=NULL);

    void RefreshAll();
    bool GetApplyAll() const {return m_applyall;}
    void SetMeshStruct(const cMeshStruct& ms);
    cMeshStruct GetMeshStruct();
};


#endif // WXDLGMESH_H_INCLUDED
