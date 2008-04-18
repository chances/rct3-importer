///////////////////////////////////////////////////////////////////////////////
//
// Matrix Editor Dialog
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

#ifndef WXDLGMATRIX_H_INCLUDED
#define WXDLGMATRIX_H_INCLUDED

#include "wx_pch.h"

#include <wx/xrc/xmlres.h>
#include <wx/spinbutt.h>

#include "colhtmllbox.h"
#include "RCT3Structs.h"

class wxMatrixListBox : public wxColourHtmlListBox {
public:
    wxMatrixListBox(wxWindow *parent, wxArrayString *contents);
    void UpdateContents();
private:
    wxArrayString* m_contents;
protected:
    virtual wxString OnGetItem(size_t n) const;
};

class dlgMatrix : public wxDialog {
protected:
    wxSpinButton* m_spinMatrix;
    wxButton* m_btMatrixCopy;
    wxButton* m_btMatrixInvert;
    wxButton* m_btMatrixDel;
    wxButton* m_btMatrixLoadAll;
    wxButton* m_btMatrixClearAll;
    wxTextCtrl* m_textMatrixName;
    wxTextCtrl* m_textM[4][4];
    wxChoice* m_choiceTranslationPoint;
    wxTextCtrl* m_textTranslationX;
    wxTextCtrl* m_textTranslationY;
    wxTextCtrl* m_textTranslationZ;
    wxButton* m_btTranslate;
    wxButton* m_btTranslateOrigin;
    wxTextCtrl* m_textRotation;
    wxButton* m_btRotationX;
    wxButton* m_btRotationY;
    wxButton* m_btRotationZ;
    wxTextCtrl* m_textScaleX;
    wxTextCtrl* m_textScaleY;
    wxTextCtrl* m_textScaleZ;
    wxButton* m_btScale;
    wxButton* m_btMirrorX;
    wxButton* m_btMirrorY;
    wxButton* m_btMirrorZ;
    wxChoice* m_choiceBone;
    wxButton* m_btFull;
    wxChoice* m_choiceFix;
    wxButton* m_btFix;
    wxButton* m_btDefault;
    wxButton* m_btLoad;
    wxButton* m_btOk;
    wxButton* m_btCancel;
    wxStaticText* m_staticDegree;

    wxMatrixListBox* m_htlbMatrix;

    void AddMatrix(const wxString name, const r3::MATRIX mat, bool setcur = true);
    void DisplayMatrix();
    void ShowMatrix(const r3::MATRIX m, unsigned int l = 4);

    void OnMatrixChange(wxCommandEvent& event);
    void OnMatrixContentChange(wxCommandEvent& event);
    void OnMatrixMoveUp(wxSpinEvent& event);
    void OnMatrixMoveDown(wxSpinEvent& event);
    void OnMatrixCopy(wxCommandEvent& event);
    void OnMatrixInverse(wxCommandEvent& event);
    void OnMatrixDelete(wxCommandEvent& event);
    void OnMatrixDeleteAll(wxCommandEvent& event);

    void OnTranslateChoice(wxCommandEvent& event);
    void OnTranslate(wxCommandEvent& event);
    void OnTranslateOrigin(wxCommandEvent& event);

    void OnRotate(wxCommandEvent& event);

    void OnScale(wxCommandEvent& event);
    void OnMirror(wxCommandEvent& event);

    void OnSpecialBone(wxCommandEvent& event);
    void OnSpecialDefault(wxCommandEvent& event);
    void OnSpecialFix(wxCommandEvent& event);
    void OnSpecialLoad(wxCommandEvent& event);

    void OnLoad(wxCommandEvent& event);

private:
    std::vector<r3::MATRIX> m_matrices;
    wxArrayString m_matrixnames;
    const std::map<wxString, c3DBone>* m_bones;
    std::vector<const c3DBone*> m_boneId;

    void InitWidgetsFromXRC(wxWindow *parent) {
        wxXmlResource::Get()->LoadObject(this,parent,_T("dlgMatrix"), _T("wxDialog"));
        m_spinMatrix = XRCCTRL(*this,"m_spinMatrix",wxSpinButton);
        m_btMatrixCopy = XRCCTRL(*this,"m_btMatrixCopy",wxButton);
        m_btMatrixInvert = XRCCTRL(*this,"m_btMatrixInvert",wxButton);
        m_btMatrixDel = XRCCTRL(*this,"m_btMatrixDel",wxButton);
        m_btMatrixLoadAll = XRCCTRL(*this,"m_btMatrixLoadAll",wxButton);
        m_btMatrixClearAll = XRCCTRL(*this,"m_btMatrixClearAll",wxButton);
        m_textMatrixName = XRCCTRL(*this,"m_textMatrixName",wxTextCtrl);
        m_textM[0][0] = XRCCTRL(*this,"m_text11",wxTextCtrl);
        m_textM[0][1] = XRCCTRL(*this,"m_text12",wxTextCtrl);
        m_textM[0][2] = XRCCTRL(*this,"m_text13",wxTextCtrl);
        m_textM[0][3] = XRCCTRL(*this,"m_text14",wxTextCtrl);
        m_textM[1][0] = XRCCTRL(*this,"m_text21",wxTextCtrl);
        m_textM[1][1] = XRCCTRL(*this,"m_text22",wxTextCtrl);
        m_textM[1][2] = XRCCTRL(*this,"m_text23",wxTextCtrl);
        m_textM[1][3] = XRCCTRL(*this,"m_text24",wxTextCtrl);
        m_textM[2][0] = XRCCTRL(*this,"m_text31",wxTextCtrl);
        m_textM[2][1] = XRCCTRL(*this,"m_text32",wxTextCtrl);
        m_textM[2][2] = XRCCTRL(*this,"m_text33",wxTextCtrl);
        m_textM[2][3] = XRCCTRL(*this,"m_text34",wxTextCtrl);
        m_textM[3][0] = XRCCTRL(*this,"m_text41",wxTextCtrl);
        m_textM[3][1] = XRCCTRL(*this,"m_text42",wxTextCtrl);
        m_textM[3][2] = XRCCTRL(*this,"m_text43",wxTextCtrl);
        m_textM[3][3] = XRCCTRL(*this,"m_text44",wxTextCtrl);
        m_choiceTranslationPoint = XRCCTRL(*this,"m_choiceTranslationPoint",wxChoice);
        m_textTranslationX = XRCCTRL(*this,"m_textTranslationX",wxTextCtrl);
        m_textTranslationY = XRCCTRL(*this,"m_textTranslationY",wxTextCtrl);
        m_textTranslationZ = XRCCTRL(*this,"m_textTranslationZ",wxTextCtrl);
        m_btTranslate = XRCCTRL(*this,"m_btTranslate",wxButton);
        m_btTranslateOrigin = XRCCTRL(*this,"m_btTranslateOrigin",wxButton);
        m_textRotation = XRCCTRL(*this,"m_textRotation",wxTextCtrl);
        m_btRotationX = XRCCTRL(*this,"m_btRotationX",wxButton);
        m_btRotationY = XRCCTRL(*this,"m_btRotationY",wxButton);
        m_btRotationZ = XRCCTRL(*this,"m_btRotationZ",wxButton);
        m_textScaleX = XRCCTRL(*this,"m_textScaleX",wxTextCtrl);
        m_textScaleY = XRCCTRL(*this,"m_textScaleY",wxTextCtrl);
        m_textScaleZ = XRCCTRL(*this,"m_textScaleZ",wxTextCtrl);
        m_btScale = XRCCTRL(*this,"m_btScale",wxButton);
        m_btMirrorX = XRCCTRL(*this,"m_btMirrorX",wxButton);
        m_btMirrorY = XRCCTRL(*this,"m_btMirrorY",wxButton);
        m_btMirrorZ = XRCCTRL(*this,"m_btMirrorZ",wxButton);
        m_choiceBone = XRCCTRL(*this,"m_choiceBone",wxChoice);
        m_btFull = XRCCTRL(*this,"m_btFull",wxButton);
        m_choiceFix = XRCCTRL(*this,"m_choiceFix",wxChoice);
        m_btFix = XRCCTRL(*this,"m_btFix",wxButton);
        m_btDefault = XRCCTRL(*this,"m_btDefault",wxButton);
        m_btLoad = XRCCTRL(*this,"m_btLoad",wxButton);
        m_btOk = XRCCTRL(*this,"m_btOk",wxButton);
        m_btCancel = XRCCTRL(*this,"m_btCancel",wxButton);
        m_staticDegree = XRCCTRL(*this,"m_staticDegree",wxStaticText);
    }
    DECLARE_EVENT_TABLE()
public:
    dlgMatrix(wxWindow *parent=NULL);
    void SetStack(const std::vector<r3::MATRIX>& mat, const wxArrayString& nam) {
        m_matrices = mat;
        m_matrixnames = nam;
        m_htlbMatrix->UpdateContents();
        DisplayMatrix();
    };
    std::vector<r3::MATRIX> GetMatrices() const {return m_matrices;};
    wxArrayString GetMatrixNames() const {return m_matrixnames;};
};


#endif // WXDLGMATRIX_H_INCLUDED
