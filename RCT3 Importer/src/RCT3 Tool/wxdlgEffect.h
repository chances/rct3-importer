///////////////////////////////////////////////////////////////////////////////
//
// Effect Point Dialog
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

#ifndef WXDLGEFFECT_H_INCLUDED
#define WXDLGEFFECT_H_INCLUDED

#include "wx_pch.h"

#include <wx/xrc/xmlres.h>

#include "RCT3Structs.h"

class wxEffectCombo;

class dlgEffect : public wxDialog {
protected:
    wxStaticText* m_Matrix[4][4];
    wxChoice* m_choicePoint;
    wxTextCtrl* m_textX;
    wxTextCtrl* m_textY;
    wxTextCtrl* m_textZ;
    wxButton* m_btPos;
    wxButton* m_btEdit;
    wxButton* m_btClear;
    wxButton* m_btCreate;
    wxButton* m_btLoad;
    wxButton* m_btOk;
    wxButton* m_btCancel;

    wxEffectCombo* m_textEffectName;

//    wxMenu* m_menuPopup;

    void SelectNr(wxString pt, unsigned int from);
    void ShowTransform(int pr = 4);
    void UpdateAll();
    virtual bool Validate();

    void OnCreateClick(wxCommandEvent& event);

    void OnPosChange(wxCommandEvent& event);
    void OnPosClick(wxCommandEvent& event);
    void OnEditClick(wxCommandEvent& event);
    void OnClearClick(wxCommandEvent& event);

    void OnMenuLightStart(wxCommandEvent& event);
    void OnMenuParticleStart(wxCommandEvent& event);
    void OnMenuParticleEnd(wxCommandEvent& event);
    void OnMenuLauncherStart(wxCommandEvent& event);
    void OnMenuLauncherEnd(wxCommandEvent& event);
    void OnMenuPeepPeep(wxCommandEvent& event);
    void OnMenuPeepVendor(wxCommandEvent& event);
    void OnMenuFountainNOZBL(wxCommandEvent& event);
    void OnMenuFountainNOZBR(wxCommandEvent& event);
    void OnMenuFountainNOZTL(wxCommandEvent& event);
    void OnMenuFountainNOZTR(wxCommandEvent& event);

    void OnLoad(wxCommandEvent& event);

private:
    cEffectPoint m_ef;
    std::vector<D3DVECTOR> m_points;

    void InitWidgetsFromXRC(wxWindow *parent) {
        wxXmlResource::Get()->LoadObject(this,parent,_T("dlgEffect"), _T("wxDialog"));
        m_Matrix[0][0] = XRCCTRL(*this,"m_11",wxStaticText);
        m_Matrix[0][1] = XRCCTRL(*this,"m_12",wxStaticText);
        m_Matrix[0][2] = XRCCTRL(*this,"m_13",wxStaticText);
        m_Matrix[0][3] = XRCCTRL(*this,"m_14",wxStaticText);
        m_Matrix[1][0] = XRCCTRL(*this,"m_21",wxStaticText);
        m_Matrix[1][1] = XRCCTRL(*this,"m_22",wxStaticText);
        m_Matrix[1][2] = XRCCTRL(*this,"m_23",wxStaticText);
        m_Matrix[1][3] = XRCCTRL(*this,"m_24",wxStaticText);
        m_Matrix[2][0] = XRCCTRL(*this,"m_31",wxStaticText);
        m_Matrix[2][1] = XRCCTRL(*this,"m_32",wxStaticText);
        m_Matrix[2][2] = XRCCTRL(*this,"m_33",wxStaticText);
        m_Matrix[2][3] = XRCCTRL(*this,"m_34",wxStaticText);
        m_Matrix[3][0] = XRCCTRL(*this,"m_41",wxStaticText);
        m_Matrix[3][1] = XRCCTRL(*this,"m_42",wxStaticText);
        m_Matrix[3][2] = XRCCTRL(*this,"m_43",wxStaticText);
        m_Matrix[3][3] = XRCCTRL(*this,"m_44",wxStaticText);
        m_choicePoint = XRCCTRL(*this,"m_choicePoint",wxChoice);
        m_textX = XRCCTRL(*this,"m_textX",wxTextCtrl);
        m_textY = XRCCTRL(*this,"m_textY",wxTextCtrl);
        m_textZ = XRCCTRL(*this,"m_textZ",wxTextCtrl);
        m_btPos = XRCCTRL(*this,"m_btPos",wxButton);
        m_btEdit = XRCCTRL(*this,"m_btEdit",wxButton);
        m_btClear = XRCCTRL(*this,"m_btClear",wxButton);
        //m_textEffectName = XRCCTRL(*this,"m_textEffectName",wxTextCtrl);
        m_btCreate = XRCCTRL(*this,"m_btCreate",wxButton);
        m_btLoad = XRCCTRL(*this,"m_btLoad",wxButton);
        m_btOk = XRCCTRL(*this,"m_btOk",wxButton);
        m_btCancel = XRCCTRL(*this,"m_btCancel",wxButton);
    }
    DECLARE_EVENT_TABLE()
public:
    dlgEffect(wxWindow *parent=NULL);
//    virtual ~dlgEffect() {delete m_menuPopup;};

    void SetEffect(const cEffectPoint& ep) {
        m_ef = ep;
        UpdateAll();
    }
    cEffectPoint GetEffect() const {return m_ef;};
};

#endif // WXDLGEFFECT_H_INCLUDED
