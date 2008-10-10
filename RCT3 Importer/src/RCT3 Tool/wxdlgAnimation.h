///////////////////////////////////////////////////////////////////////////////
//
// Animation Dialog
// Copyright (C) 2006-2007 Tobias Minch, Jonathan Wilson
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

#ifndef WXDLGANIMATION_H_INCLUDED
#define WXDLGANIMATION_H_INCLUDED

#include "xrc\res_animation.h"

#include "RCT3Structs.h"

class wxBonesListBox;
class wxTranslationListBox;
class wxRotationListBox;

class dlgAnimation : public rcdlgAnimation {
protected:
/*
    wxTextCtrl* m_textName;
    wxChoice* m_choiceCoordinateSystem;

    wxBonesListBox* m_htlbBones;
    wxSpinButton* m_spinBone;
    wxButton* m_btBoneAdd;
    wxButton* m_btBoneCopy;
    wxButton* m_btBoneDel;
    wxButton* m_btBoneClear;

    wxPanel* m_panCurrent;
    wxComboBox* m_comboBoneName;

    wxTranslationListBox* m_htlbTranslations;
    wxButton* m_btTranslationAdd;
    wxButton* m_btTranslationCopy;
    wxButton* m_btTranslationDel;
    wxButton* m_btTranslationClear;
    wxTextCtrl* m_textTimeTranslation;
    wxSpinButton* m_spinTime1Translation;
    wxSpinButton* m_spinTime2Translation;
    wxSpinButton* m_spinTime3Translation;
    wxTextCtrl* m_textXTranslation;
    wxTextCtrl* m_textYTranslation;
    wxTextCtrl* m_textZTranslation;

    wxRotationListBox* m_htlbRotations;
    wxButton* m_btRotationAdd;
    wxButton* m_btRotationCopy;
    wxButton* m_btRotationDel;
    wxButton* m_btRotationClear;
    wxTextCtrl* m_textTimeRotation;
    wxSpinButton* m_spinTime1Rotation;
    wxSpinButton* m_spinTime2Rotation;
    wxSpinButton* m_spinTime3Rotation;
    wxTextCtrl* m_textXRotation;
    wxTextCtrl* m_textYRotation;
    wxTextCtrl* m_textZRotation;
    wxStaticText* m_staticXRot;
    wxStaticText* m_staticYRot;
    wxStaticText* m_staticZRot;

    wxButton* m_btLoad;
    wxButton* m_btOk;
    wxButton* m_btCancel;
*/

    void UpdateBone();
    int UpdateControlState();

    void OnBoneChange(wxCommandEvent& event);

    void OnBoneUp(wxSpinEvent& event);
    void OnBoneDown(wxSpinEvent& event);
//    void OnBoneAdd(wxCommandEvent& event);
    void OnBoneCopy(wxCommandEvent& event);
    void OnBoneDel(wxCommandEvent& event);
    void OnBoneLoad(wxCommandEvent& event);
    void OnBoneClear(wxCommandEvent& event);

    void OnBoneName(wxCommandEvent& event);

    virtual void OnDecimate( wxCommandEvent& event );
private:
    cAnimation m_animation;
    cBoneAnimation* m_currentbone;
/*
    void InitWidgetsFromXRC(wxWindow *parent){
        wxXmlResource::Get()->LoadObject(this,parent,_T("dlgAnimation"), _T("wxDialog"));
        m_textName = XRCCTRL(*this,"m_textName",wxTextCtrl);
        m_choiceCoordinateSystem = XRCCTRL(*this,"m_choiceCoordinateSystem",wxChoice);
        m_spinBone = XRCCTRL(*this,"m_spinBone",wxSpinButton);
        m_btBoneAdd = XRCCTRL(*this,"m_btBoneAdd",wxButton);
        m_btBoneCopy = XRCCTRL(*this,"m_btBoneCopy",wxButton);
        m_btBoneDel = XRCCTRL(*this,"m_btBoneDel",wxButton);
        m_btBoneClear = XRCCTRL(*this,"m_btBoneClear",wxButton);

        m_panCurrent = XRCCTRL(*this,"m_panCurrent",wxPanel);
        m_comboBoneName = XRCCTRL(*this,"m_comboBoneName",wxComboBox);

        m_btTranslationAdd = XRCCTRL(*this,"m_btTranslationAdd",wxButton);
        m_btTranslationCopy = XRCCTRL(*this,"m_btTranslationCopy",wxButton);
        m_btTranslationDel = XRCCTRL(*this,"m_btTranslationDel",wxButton);
        m_btTranslationClear = XRCCTRL(*this,"m_btTranslationClear",wxButton);
        m_textTimeTranslation = XRCCTRL(*this,"m_textTimeTranslation",wxTextCtrl);
        m_spinTime1Translation = XRCCTRL(*this,"m_spinTime1Translation",wxSpinButton);
        m_spinTime2Translation = XRCCTRL(*this,"m_spinTime2Translation",wxSpinButton);
        m_spinTime3Translation = XRCCTRL(*this,"m_spinTime3Translation",wxSpinButton);
        m_textXTranslation = XRCCTRL(*this,"m_textXTranslation",wxTextCtrl);
        m_textYTranslation = XRCCTRL(*this,"m_textYTranslation",wxTextCtrl);
        m_textZTranslation = XRCCTRL(*this,"m_textZTranslation",wxTextCtrl);

        m_btRotationAdd = XRCCTRL(*this,"m_btRotationAdd",wxButton);
        m_btRotationCopy = XRCCTRL(*this,"m_btRotationCopy",wxButton);
        m_btRotationDel = XRCCTRL(*this,"m_btRotationDel",wxButton);
        m_btRotationClear = XRCCTRL(*this,"m_btRotationClear",wxButton);
        m_textTimeRotation = XRCCTRL(*this,"m_textTimeRotation",wxTextCtrl);
        m_spinTime1Rotation = XRCCTRL(*this,"m_spinTime1Rotation",wxSpinButton);
        m_spinTime2Rotation = XRCCTRL(*this,"m_spinTime2Rotation",wxSpinButton);
        m_spinTime3Rotation = XRCCTRL(*this,"m_spinTime3Rotation",wxSpinButton);
        m_textXRotation = XRCCTRL(*this,"m_textXRotation",wxTextCtrl);
        m_textYRotation = XRCCTRL(*this,"m_textYRotation",wxTextCtrl);
        m_textZRotation = XRCCTRL(*this,"m_textZRotation",wxTextCtrl);
        m_staticXRot = XRCCTRL(*this,"m_staticXRot",wxStaticText);
        m_staticYRot = XRCCTRL(*this,"m_staticYRot",wxStaticText);
        m_staticZRot = XRCCTRL(*this,"m_staticZRot",wxStaticText);

        m_btLoad = XRCCTRL(*this,"m_btLoad",wxButton);
        m_btOk = XRCCTRL(*this,"m_btOk",wxButton);
        m_btCancel = XRCCTRL(*this,"m_btCancel",wxButton);
    }
    DECLARE_EVENT_TABLE()
*/
public:
    dlgAnimation(wxWindow *parent=NULL);
    void SetAnimation(const cAnimation& ani) {
        m_animation = ani;
        UpdateBone();
    }
    cAnimation GetAnimation() const {return m_animation;};
};

#endif
