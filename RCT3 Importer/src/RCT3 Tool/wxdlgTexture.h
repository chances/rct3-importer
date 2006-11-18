///////////////////////////////////////////////////////////////////////////////
//
// Texture Settings Dialog
// Copyright (C) 2006 Tobias Minch
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

#ifndef WXDLGTEXTURE_H_INCLUDED
#define WXDLGTEXTURE_H_INCLUDED

#include "wx_pch.h"

#include <wx/xrc/xmlres.h>
#include <wx/spinbutt.h>
#include <wx/tglbtn.h>

#include "colhtmllbox.h"
#include "ilpict.h"
#include "RCT3Structs.h"

class wxTextureEditListBox : public wxColourHtmlListBox {
public:
    wxTextureEditListBox(wxWindow *parent, cFlexiTexture *contents);
    virtual void UpdateContents() = 0;
protected:
    cFlexiTexture* m_contents;
};

class wxFrameListBox : public wxTextureEditListBox {
public:
    wxFrameListBox(wxWindow *parent, cFlexiTexture *contents);
    virtual void UpdateContents();
protected:
    virtual wxString OnGetItem(size_t n) const;
};

class wxAnimationListBox : public wxTextureEditListBox {
public:
    wxAnimationListBox(wxWindow *parent, cFlexiTexture *contents);
    virtual void UpdateContents();
protected:
    virtual wxString OnGetItem(size_t n) const;
};

class dlgTextureFrame : public wxDialog {
protected:
    wxTextCtrl* m_textTextureFile;
    wxButton* m_btTextureInfo;
    wxButton* m_btTextureOpen;
    wxTextCtrl* m_textAlphaFile;
    wxButton* m_btAlphaInfo;
    wxButton* m_btAlphaOpen;
    wxTextCtrl* m_textAlphaCutoff;
    wxButton* m_btLoad;
    wxButton* m_btOk;
    wxButton* m_btCancel;

    virtual bool Validate();

    void OnTextureInfo(wxCommandEvent& event);
    void OnAlphaInfo(wxCommandEvent& event);
    void OnTextureOpen(wxCommandEvent& event);
    void OnAlphaOpen(wxCommandEvent& event);
    void OnLoad(wxCommandEvent& event);

private:
    cFlexiTextureFrame m_frame;
    unsigned int m_size;

    void InitWidgetsFromXRC(wxWindow *parent) {
        wxXmlResource::Get()->LoadObject(this,parent,_T("dlgTextureFrame"), _T("wxDialog"));
        m_textTextureFile = XRCCTRL(*this,"m_textTextureFile",wxTextCtrl);
        m_btTextureInfo = XRCCTRL(*this,"m_btTextureInfo",wxButton);
        m_btTextureOpen = XRCCTRL(*this,"m_btTextureOpen",wxButton);
        m_textAlphaFile = XRCCTRL(*this,"m_textAlphaFile",wxTextCtrl);
        m_btAlphaInfo = XRCCTRL(*this,"m_btAlphaInfo",wxButton);
        m_btAlphaOpen = XRCCTRL(*this,"m_btAlphaOpen",wxButton);
        m_textAlphaCutoff = XRCCTRL(*this,"m_textAlphaCutoff",wxTextCtrl);
        m_btLoad = XRCCTRL(*this,"m_btLoad",wxButton);
        m_btOk = XRCCTRL(*this,"m_btOk",wxButton);
        m_btCancel = XRCCTRL(*this,"m_btCancel",wxButton);
    }

    DECLARE_EVENT_TABLE()
public:
    dlgTextureFrame(wxWindow *parent=NULL);

    void SetFlexiTextureFrame(const cFlexiTextureFrame& val, unsigned int force_size);
    cFlexiTextureFrame GetFlexiTextureFrame();
};

class dlgTextureBase : public wxDialog {
public:
    virtual void SetFlexiTexture(const cFlexiTexture& val) = 0;
    virtual cFlexiTexture GetFlexiTexture() = 0;
    virtual void SetTextureName(const wxString& val) = 0;
};

class dlgTexture : public dlgTextureBase {
protected:
    wxTextCtrl* m_textTextureName;
    wxToggleButton* m_togglePreview;
    wxCheckBox* m_checkCustom1;
    wxCheckBox* m_checkCustom2;
    wxCheckBox* m_checkCustom3;
    wxTextCtrl* m_textFramerate;
    wxPanel* m_panelPreview;
    wxSpinButton* m_spinFrame;
    wxButton* m_btFrameAdd;
    wxButton* m_btFrameEdit;
    wxButton* m_btFrameCopy;
    wxButton* m_btFrameDel;
    wxButton* m_btFrameClear;
    wxSpinButton* m_spinAnimation;
    wxButton* m_btAnimationAdd;
    wxButton* m_btAnimationCopy;
    wxButton* m_btAnimationDel;
    wxToggleButton* m_toggleAnimationFreeze;
    wxButton* m_btAnimationClear;
    wxButton* m_btLoad;
    wxButton* m_btOk;
    wxButton* m_btCancel;

    wxFrameListBox* m_htlbFrame;
    wxAnimationListBox* m_htlbAnimation;
    wxILPicture* m_ilpictPreview;

    void UpdatePreview();
    void FramePreview();
    void AnimationPreview();

    void OnLoad(wxCommandEvent& event);
    void OnPreview(wxCommandEvent& event);

    void OnFrameListUpdate(wxCommandEvent& event);
    void OnFrameUp(wxSpinEvent& event);
    void OnFrameDown(wxSpinEvent& event);
    void OnFrameAdd(wxCommandEvent& event);
    void OnFrameEdit(wxCommandEvent& event);
    void OnFrameCopy(wxCommandEvent& event);
    void OnFrameDel(wxCommandEvent& event);
    void OnFrameClear(wxCommandEvent& event);

    void OnAnimationListUpdate(wxCommandEvent& event);
    void OnAnimationUp(wxSpinEvent& event);
    void OnAnimationDown(wxSpinEvent& event);
    void OnAnimationAdd(wxCommandEvent& event);
    void OnAnimationCopy(wxCommandEvent& event);
    void OnAnimationDel(wxCommandEvent& event);
    void OnAnimationFreeze(wxCommandEvent& event);
    void OnAnimationClear(wxCommandEvent& event);

private:
    cFlexiTexture m_ft;
    unsigned int m_size;
    wxString m_previewFile;

    void InitWidgetsFromXRC(wxWindow *parent) {
        wxXmlResource::Get()->LoadObject(this,parent,_T("dlgTexture"), _T("wxDialog"));
        m_textTextureName = XRCCTRL(*this,"m_textTextureName",wxTextCtrl);
        m_togglePreview = XRCCTRL(*this,"m_togglePreview",wxToggleButton);
        m_checkCustom1 = XRCCTRL(*this,"m_checkCustom1",wxCheckBox);
        m_checkCustom2 = XRCCTRL(*this,"m_checkCustom2",wxCheckBox);
        m_checkCustom3 = XRCCTRL(*this,"m_checkCustom3",wxCheckBox);
        m_textFramerate = XRCCTRL(*this,"m_textFramerate",wxTextCtrl);
        m_panelPreview = XRCCTRL(*this,"m_panelPreview",wxPanel);
        m_spinFrame = XRCCTRL(*this,"m_spinFrame",wxSpinButton);
        m_btFrameAdd = XRCCTRL(*this,"m_btFrameAdd",wxButton);
        m_btFrameEdit = XRCCTRL(*this,"m_btFrameEdit",wxButton);
        m_btFrameCopy = XRCCTRL(*this,"m_btFrameCopy",wxButton);
        m_btFrameDel = XRCCTRL(*this,"m_btFrameDel",wxButton);
        m_btFrameClear = XRCCTRL(*this,"m_btFrameClear",wxButton);
        m_spinAnimation = XRCCTRL(*this,"m_spinAnimation",wxSpinButton);
        m_btAnimationAdd = XRCCTRL(*this,"m_btAnimationAdd",wxButton);
        m_btAnimationCopy = XRCCTRL(*this,"m_btAnimationCopy",wxButton);
        m_btAnimationDel = XRCCTRL(*this,"m_btAnimationDel",wxButton);
        m_toggleAnimationFreeze = XRCCTRL(*this,"m_toggleAnimationFreeze",wxToggleButton);
        m_btAnimationClear = XRCCTRL(*this,"m_btAnimationClear",wxButton);
        m_btLoad = XRCCTRL(*this,"m_btLoad",wxButton);
        m_btOk = XRCCTRL(*this,"m_btOk",wxButton);
        m_btCancel = XRCCTRL(*this,"m_btCancel",wxButton);
    }

    DECLARE_EVENT_TABLE()
public:
    dlgTexture(wxWindow *parent=NULL);

    virtual void SetFlexiTexture(const cFlexiTexture& val);
    virtual cFlexiTexture GetFlexiTexture();
    virtual void SetTextureName(const wxString& WXUNUSED(val)) {
        ::wxMessageBox(wxT("SetTextureName is not supposed to be called on this subclass"), wxT("Internal Error"));
    };

    void UpdateControlState();

};

class dlgTextureSimple : public dlgTextureBase {
public:
    dlgTextureSimple(wxWindow *parent=NULL);

    virtual void SetFlexiTexture(const cFlexiTexture& WXUNUSED(val)) {
        ::wxMessageBox(wxT("SetFlexiTexture is not supposed to be called on this subclass"), wxT("Internal Error"));
    };
    virtual cFlexiTexture GetFlexiTexture();
    virtual void SetTextureName(const wxString& val);

protected:
    wxTextCtrl* m_textTextureName;
    wxTextCtrl* m_textTextureFile;
    wxButton* m_btTextureInfo;
    wxButton* m_btTextureOpen;
    wxTextCtrl* m_textAlphaFile;
    wxButton* m_btAlphaInfo;
    wxButton* m_btAlphaOpen;
    wxChoice* m_choiceColors;
    wxButton* m_btAdvanced;
    wxButton* m_btOk;
    wxButton* m_btCancel;

    virtual bool Validate();

    void OnTextureInfo(wxCommandEvent& event);
    void OnAlphaInfo(wxCommandEvent& event);
    void OnTextureOpen(wxCommandEvent& event);
    void OnAlphaOpen(wxCommandEvent& event);
    void OnAdvanced(wxCommandEvent& event);

private:
    cFlexiTexture m_ft;
    cFlexiTextureFrame m_frame;
    int m_recol;

    void InitWidgetsFromXRC(wxWindow *parent) {
        wxXmlResource::Get()->LoadObject(this,parent,_T("dlgTextureSimple"), _T("wxDialog"));
        m_textTextureName = XRCCTRL(*this,"m_textTextureName",wxTextCtrl);
        m_textTextureFile = XRCCTRL(*this,"m_textTextureFile",wxTextCtrl);
        m_btTextureInfo = XRCCTRL(*this,"m_btTextureInfo",wxButton);
        m_btTextureOpen = XRCCTRL(*this,"m_btTextureOpen",wxButton);
        m_textAlphaFile = XRCCTRL(*this,"m_textAlphaFile",wxTextCtrl);
        m_btAlphaInfo = XRCCTRL(*this,"m_btAlphaInfo",wxButton);
        m_btAlphaOpen = XRCCTRL(*this,"m_btAlphaOpen",wxButton);
        m_choiceColors = XRCCTRL(*this,"m_choiceColors",wxChoice);
        m_btAdvanced = XRCCTRL(*this,"m_btAdvanced",wxButton);
        m_btOk = XRCCTRL(*this,"m_btOk",wxButton);
        m_btCancel = XRCCTRL(*this,"m_btCancel",wxButton);
    }

    DECLARE_EVENT_TABLE()
};

#endif // WXDLGTEXTURE_H_INCLUDED
