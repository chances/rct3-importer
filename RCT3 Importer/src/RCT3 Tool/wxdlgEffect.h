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
WX_DECLARE_HASH_MAP( int, D3DVECTOR, wxIntegerHash, wxIntegerEqual, wxIdOVMMap );
WX_DECLARE_HASH_MAP( int, wxString, wxIntegerHash, wxIntegerEqual, wxIdStringMap );

class dlgEffectBase : public wxDialog {
protected:
    wxStaticText* m_Matrix[4][4];
    wxButton* m_btEdit1;
    wxButton* m_btQuick1;
    wxButton* m_btClear1;

    wxChoice* m_choiceParent;
    wxChoice* m_choiceMesh;
    wxListBox* m_lbMeshes;
    wxButton* m_btMeshAdd;
    wxButton* m_btMeshDel;
    wxButton* m_btMeshClear;
    wxCheckBox* m_cbUsePos2;
    wxStaticText* m_Matrix2[4][4];
    wxButton* m_btEdit2;
    wxButton* m_btQuick2;
    wxButton* m_btClear2;
    wxButton* m_btLoad;
    wxButton* m_btOk;
    wxButton* m_btCancel;

    wxMenu* m_menuPos1;
    wxMenu* m_menuPos2;

    wxEffectCombo* m_textEffectName;

    wxIdOVMMap m_IdMap[2];
    wxIdStringMap m_IdNameMap[2];

//    wxMenu* m_menuPopup;
//    std::vector<D3DVECTOR> m_points;

    void SelectNr(wxString pt, unsigned int from);
    virtual void ShowTransform(int pr = 4) = 0;
    void UpdateAll();
    virtual void UpdateMeshes(){};
    virtual void UpdateState(){};

    //void OnCreateClick(wxCommandEvent& event);

    void OnMenuLightStart(wxCommandEvent& event);
    void OnMenuLightColour(wxCommandEvent& event);
    void OnMenuLightEndNormal(wxCommandEvent& event);
    void OnMenuLightEndSimple(wxCommandEvent& event);
    void OnMenuLightEndNB(wxCommandEvent& event);
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

    virtual void OnQuickMenu1(wxCommandEvent& event) = 0;
    virtual void OnQuickMenu2(wxCommandEvent& WXUNUSED(event)){};
    void DoQuickMenu(const int id, wxArrayString& names, std::vector<D3DMATRIX>& transforms, int nr);

private:
    void InitWidgetsFromXRCEffect(wxWindow *parent) {
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
        m_btEdit1 = XRCCTRL(*this,"m_btEdit1",wxButton);
        m_btQuick1 = XRCCTRL(*this,"m_btQuick1",wxButton);
        m_btClear1 = XRCCTRL(*this,"m_btClear1",wxButton);
        //m_textEffectName = XRCCTRL(*this,"m_textEffectName",wxTextCtrl);
        //m_btCreate = XRCCTRL(*this,"m_btCreate",wxButton);
        m_btLoad = XRCCTRL(*this,"m_btLoad",wxButton);
        m_btOk = XRCCTRL(*this,"m_btOk",wxButton);
        m_btCancel = XRCCTRL(*this,"m_btCancel",wxButton);
    }
    void InitWidgetsFromXRCBone(wxWindow *parent){
        wxXmlResource::Get()->LoadObject(this,parent,_T("dlgBone"), _T("wxDialog"));
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
        m_btEdit1 = XRCCTRL(*this,"m_btEdit1",wxButton);
        m_btQuick1 = XRCCTRL(*this,"m_btQuick1",wxButton);
        m_btClear1 = XRCCTRL(*this,"m_btClear1",wxButton);
        m_Matrix2[0][0] = XRCCTRL(*this,"m_111",wxStaticText);
        m_Matrix2[0][1] = XRCCTRL(*this,"m_121",wxStaticText);
        m_Matrix2[0][2] = XRCCTRL(*this,"m_131",wxStaticText);
        m_Matrix2[0][3] = XRCCTRL(*this,"m_141",wxStaticText);
        m_Matrix2[1][0] = XRCCTRL(*this,"m_211",wxStaticText);
        m_Matrix2[1][1] = XRCCTRL(*this,"m_221",wxStaticText);
        m_Matrix2[1][2] = XRCCTRL(*this,"m_231",wxStaticText);
        m_Matrix2[1][3] = XRCCTRL(*this,"m_241",wxStaticText);
        m_Matrix2[2][0] = XRCCTRL(*this,"m_311",wxStaticText);
        m_Matrix2[2][1] = XRCCTRL(*this,"m_321",wxStaticText);
        m_Matrix2[2][2] = XRCCTRL(*this,"m_331",wxStaticText);
        m_Matrix2[2][3] = XRCCTRL(*this,"m_341",wxStaticText);
        m_Matrix2[3][0] = XRCCTRL(*this,"m_411",wxStaticText);
        m_Matrix2[3][1] = XRCCTRL(*this,"m_421",wxStaticText);
        m_Matrix2[3][2] = XRCCTRL(*this,"m_431",wxStaticText);
        m_Matrix2[3][3] = XRCCTRL(*this,"m_441",wxStaticText);
        m_btEdit2 = XRCCTRL(*this,"m_btEdit2",wxButton);
        m_btQuick2 = XRCCTRL(*this,"m_btQuick2",wxButton);
        m_btClear2 = XRCCTRL(*this,"m_btClear2",wxButton);
        m_cbUsePos2 = XRCCTRL(*this,"m_cbUsePos2",wxCheckBox);
        m_choiceParent = XRCCTRL(*this,"m_choiceParent",wxChoice);
        m_choiceMesh = XRCCTRL(*this,"m_choiceMesh",wxChoice);
        m_lbMeshes = XRCCTRL(*this,"m_lbMeshes",wxListBox);
        m_btMeshAdd = XRCCTRL(*this,"m_btMeshAdd",wxButton);
        m_btMeshDel = XRCCTRL(*this,"m_btMeshDel",wxButton);
        m_btMeshClear = XRCCTRL(*this,"m_btMeshClear",wxButton);
        m_btLoad = XRCCTRL(*this,"m_btLoad",wxButton);
        m_btOk = XRCCTRL(*this,"m_btOk",wxButton);
        m_btCancel = XRCCTRL(*this,"m_btCancel",wxButton);
    }
    DECLARE_EVENT_TABLE()
public:
    dlgEffectBase(wxWindow *parent=NULL, bool effect=true);
    virtual ~dlgEffectBase() {
        if (m_menuPos1) delete m_menuPos1;
        if (m_menuPos2) delete m_menuPos2;
    };

};

class dlgEffect: public dlgEffectBase {
protected:
    virtual void ShowTransform(int pr = 4);
    virtual bool Validate();

//    void OnPosChange(wxCommandEvent& event);
    virtual void OnQuickMenu1(wxCommandEvent& event);
    void OnEditClick(wxCommandEvent& event);
    void OnQuickClick(wxCommandEvent& event);
    void OnClearClick(wxCommandEvent& event);

    void OnLoad(wxCommandEvent& event);

private:
    cEffectPoint m_ef;
    DECLARE_EVENT_TABLE()
public:
    dlgEffect(wxWindow *parent=NULL);
    void SetEffect(const cEffectPoint& ep) {
        m_ef = ep;
        UpdateAll();
    }
    cEffectPoint GetEffect() const {return m_ef;};
};

class dlgBone: public dlgEffectBase {
protected:
    virtual void ShowTransform(int pr = 4);
    virtual bool Validate();
    virtual void UpdateMeshes();
    virtual void UpdateState();

    virtual void OnQuickMenu1(wxCommandEvent& event);
    virtual void OnQuickMenu2(wxCommandEvent& event);
    virtual void OnUpdate(wxCommandEvent& WXUNUSED(event)) {
        UpdateState();
    }
    void OnAddMeshClick(wxCommandEvent& event);
    void OnDelMeshClick(wxCommandEvent& event);
    void OnClearMeshClick(wxCommandEvent& event);

//    void OnPosChange(wxCommandEvent& event);
//    void OnPosClick(wxCommandEvent& event);
    void OnEditClick(wxCommandEvent& event);
    void OnQuickClick(wxCommandEvent& event);
    void OnClearClick(wxCommandEvent& event);
//    void OnPos1Change(wxCommandEvent& event);
//    void OnPos1Click(wxCommandEvent& event);
    void OnEdit1Click(wxCommandEvent& event);
    void OnQuick1Click(wxCommandEvent& event);
    void OnClear1Click(wxCommandEvent& event);
    void OnUse2Click(wxCommandEvent& event);

    void OnLoad(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
private:
    cModelBone m_bn;
    wxArrayString m_meshes;
    cAnimatedModel* m_model;
public:
    dlgBone(wxWindow *parent=NULL);
    void SetBone(const cModelBone& bn);
    cModelBone GetBone() const {
        cModelBone temp = m_bn;
        if (temp.parent == wxT("-"))
            temp.parent = wxT("");
        return temp;
    };
};

#endif // WXDLGEFFECT_H_INCLUDED
