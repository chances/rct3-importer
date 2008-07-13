///////////////////////////////////////////////////////////////////////////////
//
// Model Settings Dialog
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

#ifndef WXDLGMODEL_H_INCLUDED
#define WXDLGMODEL_H_INCLUDED

#include <wx/aui/aui.h>
#include <wx/xrc/xmlres.h>
#include <wx/htmllbox.h>
#include <wx/spinbutt.h>

#include "colhtmllbox.h"
#include "exdataview.h"
#include "fileselectorcombo.h"
#include "RCT3Structs.h"
#include "starcomboctrl.h"

class wxMeshListModel;
/*
class wxMeshListBox : public wxColourHtmlListBox {
public:
    wxMeshListBox(wxWindow *parent, cModel *contents);
    void UpdateContents();
private:
    cModel* m_contents;
protected:
    virtual wxString OnGetItem(size_t n) const;
};
*/
class wxEffectListBox : public wxHtmlListBox {
public:
    wxEffectListBox(wxWindow *parent);
    wxEffectListBox(wxWindow *parent, cModel *contents);
    virtual void UpdateContents();
private:
    cModel* m_contents;
protected:
    virtual wxString OnGetItem(size_t n) const;
};

class wxBoneListBox : public wxEffectListBox {
public:
    wxBoneListBox(wxWindow *parent, cModel *contents);
    virtual void UpdateContents();
private:
    cAnimatedModel* m_contents;
protected:
    virtual wxString OnGetItem(size_t n) const;
};

class dlgBone;
class dlgModel : public wxDialog {
    friend class dlgBone;
protected:
    wxStarComboCtrl* m_textModelName;
    wxChoice* m_choiceCoordinateSystem;
    wxButton* m_btMatrixEdit;
    wxButton* m_btMatrixSave;
    wxButton* m_btMatrixClear;
    wxStaticText* m_Matrix[4][4];
    wxSpinButton* m_spinEffect;
    wxButton* m_btEffectAdd;
    wxButton* m_btEffectEdit;
    wxButton* m_btEffectCopy;
    wxButton* m_btEffectDel;
    wxButton* m_btEffectAuto;
    wxButton* m_btEffectClear;
    wxButton* m_btLoad;
    wxButton* m_btOk;
    wxButton* m_btCancel;

    wxFileSelectorCombo<wxFileDialog>* m_textModelFile;

    wxPanel* m_panModel;
    wxPanel* m_panEffect;
    wxPanel* m_panMeshes;
    wxPanel* m_panButtons;

    //wxMeshListBox* m_htlbMesh;
    wxEffectListBox* m_htlbEffect;
    wxExDataViewCtrl* m_dataviewMesh;
    wxMeshListModel* m_modelMesh;

    void OnWrapperEdit(wxCommandEvent& event);
    void OnDoUpdate(wxCommandEvent& event);
    void OnChar(wxKeyEvent& event);
    //void OnPaneMaximize(wxAuiManagerEvent& event);

    void OnNameAuto(wxCommandEvent& event);
    void OnModelOpen(wxCommandEvent& event);
    void OnControlUpdate(wxCommandEvent& event);

    void OnMatrixEdit(wxCommandEvent& event);
    void OnMatrixSave(wxCommandEvent& event);
    void OnMatrixClear(wxCommandEvent& event);

    //void OnMeshEdit(wxCommandEvent& event);

    void OnEffectUp(wxSpinEvent& event);
    void OnEffectDown(wxSpinEvent& event);
    void OnEffectAdd(wxCommandEvent& event);
    void OnEffectEdit(wxCommandEvent& event);
    void OnEffectCopy(wxCommandEvent& event);
    void OnEffectDel(wxCommandEvent& event);
    void OnEffectAuto(wxCommandEvent& event);
    void OnEffectClear(wxCommandEvent& event);

    void OnBoneUp(wxSpinEvent& event);
    void OnBoneDown(wxSpinEvent& event);
    void OnBoneAdd(wxCommandEvent& event);
    void OnBoneEdit(wxCommandEvent& event);
    void OnBoneCopy(wxCommandEvent& event);
    void OnBoneDel(wxCommandEvent& event);
    void OnBoneAuto(wxCommandEvent& event);
    void OnBoneClear(wxCommandEvent& event);

    void OnLoad(wxCommandEvent& event);

    void ShowTransform(int pr = 4);
    void UpdateAll();
    void UpdateControlState();

    virtual bool ProcessEvent(wxEvent& event);

private:
    cModel m_smodel;
    cAnimatedModel m_amodel;
    cModel* m_model;
    bool m_animated;
    wxArrayString m_textures;
    wxAuiManager m_mgr;
    wxEvtHandler* m_hookhandler;
    bool m_loadoverlay;

    void InitWidgetsFromXRC(wxWindow *parent) {
        wxXmlResource::Get()->LoadObject(this,parent,_T("dlgModel"), _T("wxDialog"));
        /*
                m_textModelName = XRCCTRL(*this,"m_textModelName",wxTextCtrl);
                m_btMatrixEdit = XRCCTRL(*this,"m_btMatrixEdit",wxButton);
                m_btMatrixSave = XRCCTRL(*this,"m_btMatrixSave",wxButton);
                m_btMatrixClear = XRCCTRL(*this,"m_btMatrixClear",wxButton);
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
                m_spinEffect = XRCCTRL(*this,"m_spinEffect",wxSpinButton);
                m_btEffectAdd = XRCCTRL(*this,"m_btEffectAdd",wxButton);
                m_btEffectEdit = XRCCTRL(*this,"m_btEffectEdit",wxButton);
                m_btEffectCopy = XRCCTRL(*this,"m_btEffectCopy",wxButton);
                m_btEffectDel = XRCCTRL(*this,"m_btEffectDel",wxButton);
                m_btEffectAuto = XRCCTRL(*this,"m_btEffectAuto",wxButton);
                m_btEffectClear = XRCCTRL(*this,"m_btEffectClear",wxButton);
                m_btLoad = XRCCTRL(*this,"m_btLoad",wxButton);
                m_btOk = XRCCTRL(*this,"m_btOk",wxButton);
                m_btCancel = XRCCTRL(*this,"m_btCancel",wxButton);
        */
    }
    void InitModelFromXRC(wxWindow *parent) {
        wxXmlResource::Get()->LoadObject(parent,this,_T("panModelModel"), _T("wxPanel"));
        //m_textModelName = XRCCTRL(*parent,"m_textModelName",wxTextCtrl);
        m_choiceCoordinateSystem = XRCCTRL(*this,"m_choiceCoordinateSystem",wxChoice);
        m_btMatrixEdit = XRCCTRL(*parent,"m_btMatrixEdit",wxButton);
        m_btMatrixSave = XRCCTRL(*parent,"m_btMatrixSave",wxButton);
        m_btMatrixClear = XRCCTRL(*parent,"m_btMatrixClear",wxButton);
        m_Matrix[0][0] = XRCCTRL(*parent,"m_11",wxStaticText);
        m_Matrix[0][1] = XRCCTRL(*parent,"m_12",wxStaticText);
        m_Matrix[0][2] = XRCCTRL(*parent,"m_13",wxStaticText);
        m_Matrix[0][3] = XRCCTRL(*parent,"m_14",wxStaticText);
        m_Matrix[1][0] = XRCCTRL(*parent,"m_21",wxStaticText);
        m_Matrix[1][1] = XRCCTRL(*parent,"m_22",wxStaticText);
        m_Matrix[1][2] = XRCCTRL(*parent,"m_23",wxStaticText);
        m_Matrix[1][3] = XRCCTRL(*parent,"m_24",wxStaticText);
        m_Matrix[2][0] = XRCCTRL(*parent,"m_31",wxStaticText);
        m_Matrix[2][1] = XRCCTRL(*parent,"m_32",wxStaticText);
        m_Matrix[2][2] = XRCCTRL(*parent,"m_33",wxStaticText);
        m_Matrix[2][3] = XRCCTRL(*parent,"m_34",wxStaticText);
        m_Matrix[3][0] = XRCCTRL(*parent,"m_41",wxStaticText);
        m_Matrix[3][1] = XRCCTRL(*parent,"m_42",wxStaticText);
        m_Matrix[3][2] = XRCCTRL(*parent,"m_43",wxStaticText);
        m_Matrix[3][3] = XRCCTRL(*parent,"m_44",wxStaticText);
    }
    void InitEffectFromXRC(wxWindow *parent) {
        wxXmlResource::Get()->LoadObject(parent,this,_T("panModelEffect"), _T("wxPanel"));
        m_spinEffect = XRCCTRL(*parent,"m_spinEffect",wxSpinButton);
        m_btEffectAdd = XRCCTRL(*parent,"m_btEffectAdd",wxButton);
        m_btEffectEdit = XRCCTRL(*parent,"m_btEffectEdit",wxButton);
        m_btEffectCopy = XRCCTRL(*parent,"m_btEffectCopy",wxButton);
        m_btEffectDel = XRCCTRL(*parent,"m_btEffectDel",wxButton);
        m_btEffectAuto = XRCCTRL(*parent,"m_btEffectAuto",wxButton);
        m_btEffectClear = XRCCTRL(*parent,"m_btEffectClear",wxButton);
    }
    void InitButtonsFromXRC(wxWindow *parent) {
        wxXmlResource::Get()->LoadObject(parent,this,_T("panModelButtons"), _T("wxPanel"));
        m_btLoad = XRCCTRL(*parent,"m_btLoad",wxButton);
        m_btOk = XRCCTRL(*parent,"m_btOk",wxButton);
        m_btCancel = XRCCTRL(*parent,"m_btCancel",wxButton);
    }

    DECLARE_EVENT_TABLE()
public:
    bool m_Editing;

    dlgModel(wxWindow *parent=NULL, bool animated = false);
    virtual ~dlgModel();

    void CheckModel();
    void SetModel(const cModel& model) {
        m_smodel = model;
        CheckModel();
    };
    void SetAnimatedModel(const cAnimatedModel& model) {
        m_amodel = model;
        CheckModel();
    };
    const cModel& GetModel() const {
        return m_smodel;
    };
    const cAnimatedModel& GetAnimatedModel() const {
        return m_amodel;
    };
    const cModel* GetModelPtr() const {
        return m_model;
    };
    //void FetchOneVertexMeshes(wxArrayString& names, std::vector<r3::VECTOR>& points);
};


#endif // WXDLGMODEL_H_INCLUDED
