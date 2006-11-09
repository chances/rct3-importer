///////////////////////////////////////////////////////////////////////////////
//
// Create Scenery OVL Dialog
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

#ifndef WXDLGCREATESCENERY_H_INCLUDED
#define WXDLGCREATESCENERY_H_INCLUDED

#include "wx_pch.h"

#include <wx/xrc/xmlres.h>
#include <wx/htmllbox.h>
#include <wx/spinbutt.h>

#include "colhtmllbox.h"
#include "SCNFile.h"

class wxSceneryListBox : public wxColourHtmlListBox {
public:
    wxSceneryListBox(wxWindow *parent, cSCNFile *contents);
    virtual void UpdateContents() = 0;
protected:
    cSCNFile* m_contents;
};

class wxTextureListBox : public wxSceneryListBox {
public:
    wxTextureListBox(wxWindow *parent, cSCNFile *contents);
    virtual void UpdateContents();
protected:
    virtual wxString OnGetItem(size_t n) const;
};

class wxModelListBox : public wxSceneryListBox {
public:
    wxModelListBox(wxWindow *parent, cSCNFile *contents);
    virtual void UpdateContents();
protected:
    virtual wxString OnGetItem(size_t n) const;
};

class wxLODListBox : public wxSceneryListBox {
public:
    wxLODListBox(wxWindow *parent, cSCNFile *contents);
    virtual void UpdateContents();
protected:
    virtual wxString OnGetItem(size_t n) const;
};

class wxReferenceListBox : public wxSceneryListBox {
public:
    wxReferenceListBox(wxWindow *parent, cSCNFile *contents);
    virtual void UpdateContents();
protected:
    virtual wxString OnGetItem(size_t n) const;
};


class dlgCreateScenery : public wxDialog {
protected:
    wxSpinButton* m_spinTexture;
    wxButton* m_btTextureEdit;
    wxButton* m_btTextureCopy;
    wxButton* m_btTextureDel;
    wxButton* m_btTextureClear;
    wxSpinButton* m_spinModel;
    wxButton* m_btModelEdit;
    wxButton* m_btModelCopy;
    wxButton* m_btModelDel;
    wxButton* m_btModelClear;
    wxButton* m_btLODAdd;
    wxButton* m_btLODEdit;
    wxButton* m_btLODCopy;
    wxButton* m_btLODDel;
    wxButton* m_btLODAuto;
    wxButton* m_btLODClear;
    wxChoice* m_choiceExpansion;
    wxCheckBox* m_checkGreenery;
    wxTextCtrl* m_textSway;
    wxCheckBox* m_checkShadow;
    wxTextCtrl* m_textBrightness;
    wxCheckBox* m_checkRotation;
    wxTextCtrl* m_textScale;
    wxCheckBox* m_checkShowUnknown;
    wxPanel* m_panelUnknown;
    wxCheckBox* m_checkUnknown;
    wxTextCtrl* m_textUnknownFactor;
    wxTextCtrl* m_textUnknown6;
    wxTextCtrl* m_textUnknown7;
    wxTextCtrl* m_textUnknown8;
    wxTextCtrl* m_textUnknown9;
    wxTextCtrl* m_textUnknown10;
    wxTextCtrl* m_textUnknown11;
    wxTextCtrl* m_textReference;
    wxButton* m_btReferenceDel;
    wxButton* m_btReferenceClear;
    wxButton* m_btCreate;

    wxToolBar *m_tbCS;
    wxTextureListBox *m_htlbTexture;
    wxModelListBox *m_htlbModel;
    wxLODListBox *m_htlbLOD;
    wxReferenceListBox *m_htlbReferences;

    virtual bool TransferDataFromWindow();
    virtual bool TransferDataToWindow();

    void MakeDirty(bool dirty = true);

    void OnToolBar(wxCommandEvent& event);
    void OnShowUnknowns(wxCommandEvent& event);
    void OnControlUpdate(wxCommandEvent& event);
    void OnMakeDirty(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnInitDialog(wxInitDialogEvent& event);

    void OnTextureUp(wxSpinEvent& event);
    void OnTextureDown(wxSpinEvent& event);
    void OnTextureAdd(wxCommandEvent& event);
    void OnTextureEdit(wxCommandEvent& event);
    void OnTextureCopy(wxCommandEvent& event);
    void OnTextureDel(wxCommandEvent& event);
    void OnTextureClear(wxCommandEvent& event);

    void OnModelUp(wxSpinEvent& event);
    void OnModelDown(wxSpinEvent& event);
    void OnModelAdd(wxCommandEvent& event);
    void OnModelEdit(wxCommandEvent& event);
    void OnModelCopy(wxCommandEvent& event);
    void OnModelDel(wxCommandEvent& event);
    void OnModelClear(wxCommandEvent& event);

    void OnLODAdd(wxCommandEvent& event);
    void OnLODEdit(wxCommandEvent& event);
    void OnLODCopy(wxCommandEvent& event);
    void OnLODDel(wxCommandEvent& event);
    void OnLODAuto(wxCommandEvent& event);
    void OnLODClear(wxCommandEvent& event);

    void OnVisualNormal(wxCommandEvent& event);
    void OnVisualSwayingTree(wxCommandEvent& event);

    void OnReferenceAdd(wxCommandEvent& event);
    void OnReferenceDel(wxCommandEvent& event);
    void OnReferenceClear(wxCommandEvent& event);

    void OnCreate(wxCommandEvent& event);

private:
    bool m_dirtyfile;
    wxString m_defaultRef;
    wxString m_prefix;
    wxString m_theme;

    void UpdateAll();

    void InitWidgetsFromXRC(wxWindow *parent) {
        wxXmlResource::Get()->LoadObject(this,parent,_T("dlgCreateScenery"), _T("wxDialog"));
        m_spinTexture = XRCCTRL(*this,"m_spinTexture",wxSpinButton);
        m_btTextureEdit = XRCCTRL(*this,"m_btTextureEdit",wxButton);
        m_btTextureCopy = XRCCTRL(*this,"m_btTextureCopy",wxButton);
        m_btTextureDel = XRCCTRL(*this,"m_btTextureDel",wxButton);
        m_btTextureClear = XRCCTRL(*this,"m_btTextureClear",wxButton);
        m_spinModel = XRCCTRL(*this,"m_spinModel",wxSpinButton);
        m_btModelEdit = XRCCTRL(*this,"m_btModelEdit",wxButton);
        m_btModelCopy = XRCCTRL(*this,"m_btModelCopy",wxButton);
        m_btModelDel = XRCCTRL(*this,"m_btModelDel",wxButton);
        m_btModelClear = XRCCTRL(*this,"m_btModelClear",wxButton);
        m_btLODAdd = XRCCTRL(*this,"m_btLODAdd",wxButton);
        m_btLODEdit = XRCCTRL(*this,"m_btLODEdit",wxButton);
        m_btLODCopy = XRCCTRL(*this,"m_btLODCopy",wxButton);
        m_btLODDel = XRCCTRL(*this,"m_btLODDel",wxButton);
        m_btLODAuto = XRCCTRL(*this,"m_btLODAuto",wxButton);
        m_btLODClear = XRCCTRL(*this,"m_btLODClear",wxButton);
        m_choiceExpansion = XRCCTRL(*this,"m_choiceExpansion",wxChoice);
        m_checkGreenery = XRCCTRL(*this,"m_checkGreenery",wxCheckBox);
        m_textSway = XRCCTRL(*this,"m_textSway",wxTextCtrl);
        m_checkShadow = XRCCTRL(*this,"m_checkShadow",wxCheckBox);
        m_textBrightness = XRCCTRL(*this,"m_textBrightness",wxTextCtrl);
        m_checkRotation = XRCCTRL(*this,"m_checkRotation",wxCheckBox);
        m_textScale = XRCCTRL(*this,"m_textScale",wxTextCtrl);
        m_checkShowUnknown = XRCCTRL(*this,"m_checkShowUnknown",wxCheckBox);
        m_panelUnknown = XRCCTRL(*this,"m_panelUnknown",wxPanel);
        m_checkUnknown = XRCCTRL(*this,"m_checkUnknown",wxCheckBox);
        m_textUnknownFactor = XRCCTRL(*this,"m_textUnknownFactor",wxTextCtrl);
        m_textUnknown6 = XRCCTRL(*this,"m_textUnknown6",wxTextCtrl);
        m_textUnknown7 = XRCCTRL(*this,"m_textUnknown7",wxTextCtrl);
        m_textUnknown8 = XRCCTRL(*this,"m_textUnknown8",wxTextCtrl);
        m_textUnknown9 = XRCCTRL(*this,"m_textUnknown9",wxTextCtrl);
        m_textUnknown10 = XRCCTRL(*this,"m_textUnknown10",wxTextCtrl);
        m_textUnknown11 = XRCCTRL(*this,"m_textUnknown11",wxTextCtrl);
        m_textReference = XRCCTRL(*this,"m_textReference",wxTextCtrl);
        m_btReferenceDel = XRCCTRL(*this,"m_btReferenceDel",wxButton);
        m_btReferenceClear = XRCCTRL(*this,"m_btReferenceClear",wxButton);
        m_btCreate = XRCCTRL(*this,"m_btCreate",wxButton);
    }

    DECLARE_EVENT_TABLE()
public:
    cSCNFile m_SCN;

    dlgCreateScenery(wxWindow *parent=NULL);
    void SetDefaultRef(const wxString& prefix, const wxString& theme) {
        m_defaultRef = wxT("../shared/") + prefix + theme + wxT("-texture");
        m_prefix = prefix;
        m_theme = theme;
        UpdateAll();
    };
    wxString GetOVLPath();

    void UpdateControlState();
};
#endif // WXDLGCREATESCENERY_H_INCLUDED
