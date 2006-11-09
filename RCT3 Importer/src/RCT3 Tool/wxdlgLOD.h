///////////////////////////////////////////////////////////////////////////////
//
// Level of Detail Dialog
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

#ifndef WXDLGLOD_H_INCLUDED
#define WXDLGLOD_H_INCLUDED

#include "wx_pch.h"

#include <wx/xrc/xmlres.h>

#include "RCT3Structs.h"

class dlgLOD : public wxDialog {
protected:
    wxChoice* m_choiceModel;
    wxTextCtrl* m_textDistance;
    wxButton* m_btDist40;
    wxButton* m_btDist100;
    wxButton* m_btDist4000;
    wxCheckBox* m_checkShowUnknown;
    wxPanel* m_panelUnknown;
    wxTextCtrl* m_textUnknown2;
    wxTextCtrl* m_textUnknown4;
    wxTextCtrl* m_textUnknown14;
    wxButton* m_btOk;
    wxButton* m_btCancel;

    void OnShowUnknowns(wxCommandEvent& event);
    void OnDist40(wxCommandEvent& event);
    void OnDist100(wxCommandEvent& event);
    void OnDist4000(wxCommandEvent& event);

private:
    cLOD m_lod;

    void InitWidgetsFromXRC(wxWindow *parent) {
        wxXmlResource::Get()->LoadObject(this,parent,_T("dlgLOD"), _T("wxDialog"));
        m_choiceModel = XRCCTRL(*this,"m_choiceModel",wxChoice);
        m_textDistance = XRCCTRL(*this,"m_textDistance",wxTextCtrl);
        m_btDist40 = XRCCTRL(*this,"m_btDist40",wxButton);
        m_btDist100 = XRCCTRL(*this,"m_btDist100",wxButton);
        m_btDist4000 = XRCCTRL(*this,"m_btDist4000",wxButton);
        m_checkShowUnknown = XRCCTRL(*this,"m_checkShowUnknown",wxCheckBox);
        m_panelUnknown = XRCCTRL(*this,"m_panelUnknown",wxPanel);
        m_textUnknown2 = XRCCTRL(*this,"m_textUnknown2",wxTextCtrl);
        m_textUnknown4 = XRCCTRL(*this,"m_textUnknown4",wxTextCtrl);
        m_textUnknown14 = XRCCTRL(*this,"m_textUnknown14",wxTextCtrl);
        m_btOk = XRCCTRL(*this,"m_btOk",wxButton);
        m_btCancel = XRCCTRL(*this,"m_btCancel",wxButton);
    }

    DECLARE_EVENT_TABLE()
public:
    dlgLOD(const std::vector<cModel>& mods, wxWindow *parent=NULL);
    void SetLOD(const cLOD& lod) {m_lod = lod;};
    cLOD GetLOD() const {return m_lod;};
};

#endif // WXDLGLOD_H_INCLUDED
