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

#include "wx_pch.h"

#include <wx/busyinfo.h>
#include <wx/valgen.h>

#include "colhtmllbox.h"
#include "htmlentities.h"
#include "matrix.h"
#include "valext.h"
#include "wxInputBox.h"

#include "wxdlgCreateScenery.h"

#include "wxdlgAnimation.h"
#include "wxdlgAnimation_HTLB.h"
#include "wxdlgDecimate.h"

using namespace std;

////////////////////////////////////////////////////////////////////////
//
//  dlgAnimation
//
////////////////////////////////////////////////////////////////////////
/*
BEGIN_EVENT_TABLE(dlgAnimation,wxDialog)
EVT_LISTBOX(XRCID("m_htlbBones"), dlgAnimation::OnBoneChange)
EVT_SPIN_UP(XRCID("m_spinBone"), dlgAnimation::OnBoneUp)
EVT_SPIN_DOWN(XRCID("m_spinBone"), dlgAnimation::OnBoneDown)
EVT_BUTTON(XRCID("m_btBoneAdd"), dlgAnimation::OnBoneAdd)
EVT_BUTTON(XRCID("m_btBoneCopy"), dlgAnimation::OnBoneCopy)
EVT_BUTTON(XRCID("m_btBoneDel"), dlgAnimation::OnBoneDel)
EVT_BUTTON(XRCID("m_btBoneClear"), dlgAnimation::OnBoneClear)

EVT_TEXT(XRCID("m_comboBoneName"), dlgAnimation::OnBoneName)

END_EVENT_TABLE()
*/

dlgAnimation::dlgAnimation(wxWindow *parent):rcdlgAnimation(parent), m_currentbone(NULL) {
    wxLogDebug(wxT("Trace, dlgAnimation::Create"));
//    InitWidgetsFromXRC((wxWindow *)parent);

    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

/*
    m_staticXRot->SetLabel(wxT("X (°)"));
    m_staticYRot->SetLabel(wxT("Y (°)"));
    m_staticZRot->SetLabel(wxT("Z (°)"));
*/

    dlgCreateScenery* cs = dynamic_cast<dlgCreateScenery*>(parent);
    if (cs) {
        wxArrayString bonelist;
        for (cAnimatedModel::iterator am = cs->m_SCN.animatedmodels.begin(); am != cs->m_SCN.animatedmodels.end(); am++) {
            for (cModelBone::iterator mb = am->modelbones.begin(); mb != am->modelbones.end(); mb++) {
                if (bonelist.Index(mb->name) == wxNOT_FOUND) {
                    bonelist.Add(mb->name);
                }
            }
        }
        m_comboBoneName->Append(bonelist);
    }

    m_textName->SetValidator(wxExtendedValidator(&m_animation.name, false));
    m_choiceCoordinateSystem->SetValidator(wxGenericValidator(reinterpret_cast<int*>(&m_animation.usedorientation)));

    wxInputBox *t_ibName = new wxInputBox(this, wxID_ANY);
    t_ibName->SetEditor(m_textName);

/*
    m_htlbBones = new wxBonesListBox(this, &m_animation);
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbBones"), m_htlbBones, this);
    m_htlbTranslations = new wxTranslationListBox(m_panCurrent, &m_currentbone);
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbTranslations"), m_htlbTranslations, m_panCurrent);
    m_htlbRotations = new wxRotationListBox(m_panCurrent, &m_currentbone);
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbRotations"), m_htlbRotations, m_panCurrent);
*/

    m_htlbBones->Init(&m_animation);
    m_htlbTranslations->Init(&m_currentbone);
    m_htlbRotations->Init(&m_currentbone);

    Fit();
    Layout();
    GetSizer()->SetSizeHints(this);
    m_panCurrent->GetSizer()->Layout();

    m_btOk->SetId(wxID_OK);
    m_btCancel->SetId(wxID_CANCEL);
}

int dlgAnimation::UpdateControlState() {
    int bone = m_htlbBones->GetSelection();
    int count = m_animation.boneanimations.size();

    m_spinBone->Enable(count>=2);
    m_btBoneCopy->Enable(bone>=0);
    m_btBoneDel->Enable(bone>=0);
    m_btBoneClear->Enable(count>=1);

    m_htlbBones->UpdateContents();

    return bone;
}

void dlgAnimation::UpdateBone() {
    int bone = UpdateControlState();
    wxLogDebug(wxT("Trace, UpdateControlState %d"), bone);
    if (bone>=0) {
        m_currentbone = &m_animation.boneanimations[bone];
        m_htlbTranslations->UpdateContents();
        m_htlbRotations->UpdateContents();
        m_comboBoneName->SetValue(m_currentbone->name);
        m_panCurrent->Enable();
        m_sizRotations->GetStaticBox()->SetLabel(wxString(_("Rotations"))+(m_currentbone->axis?_(" (Axis/Angle)"):_(" (Euler)")));
    } else {
        m_currentbone = NULL;
        m_htlbTranslations->UpdateContents();
        m_htlbRotations->UpdateContents();
        m_comboBoneName->SetValue(wxT(""));
        m_sizRotations->GetStaticBox()->SetLabel(_("Rotations"));
/*
        m_textTimeTranslation->ChangeValue(wxT(""));
        m_textXTranslation->ChangeValue(wxT(""));
        m_textYTranslation->ChangeValue(wxT(""));
        m_textZTranslation->ChangeValue(wxT(""));
        m_textTimeRotation->ChangeValue(wxT(""));
        m_textXRotation->ChangeValue(wxT(""));
        m_textYRotation->ChangeValue(wxT(""));
        m_textZRotation->ChangeValue(wxT(""));
*/
        m_panCurrent->Disable();
    }
}

////////////////////////////////////////////////////////////////////////
//
//  dlgAnimation, Bones
//
////////////////////////////////////////////////////////////////////////

void dlgAnimation::OnBoneChange(wxCommandEvent& WXUNUSED(event)) {
    UpdateBone();
}

void dlgAnimation::OnBoneUp(wxSpinEvent& WXUNUSED(event)) {
    int sel = m_htlbBones->GetSelection();
    if ((sel < 1) || (sel >= m_animation.boneanimations.size()))
        return;
    cBoneAnimation ban = m_animation.boneanimations[sel];
    m_animation.boneanimations.erase(m_animation.boneanimations.begin() + sel);
    m_animation.boneanimations.insert(m_animation.boneanimations.begin() + sel - 1, ban);

    m_htlbBones->UpdateContents();
    m_htlbBones->SetSelection(sel-1);
    UpdateBone();
}

void dlgAnimation::OnBoneDown(wxSpinEvent& WXUNUSED(event)) {
    int count = m_animation.boneanimations.size();
    int sel = m_htlbBones->GetSelection();
    if (((count-sel) <= 1) || (sel >= m_animation.boneanimations.size()))
        return;
    cBoneAnimation ban = m_animation.boneanimations[sel];
    m_animation.boneanimations.erase(m_animation.boneanimations.begin() + sel);
    m_animation.boneanimations.insert(m_animation.boneanimations.begin() + sel + 1, ban);

    m_htlbBones->UpdateContents();
    m_htlbBones->SetSelection(sel+1);
    UpdateBone();
}

void dlgAnimation::OnBoneLoad(wxCommandEvent& WXUNUSED(event)) {
    cBoneAnimation ban;
    ban.name = _("New animated bone");
    m_animation.boneanimations.push_back(ban);

    m_htlbBones->UpdateContents();
    m_htlbBones->SetSelection(m_animation.boneanimations.size()-1);
    UpdateBone();
}

void dlgAnimation::OnBoneCopy(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbBones->GetSelection();
    if ((sel < 0) || (sel >= m_animation.boneanimations.size()))
        return;

    cBoneAnimation ban = m_animation.boneanimations[sel];
    ban.name += _(" Copy");
    m_animation.boneanimations.insert(m_animation.boneanimations.begin() + sel + 1, ban);

    m_htlbBones->UpdateContents();
    m_htlbBones->SetSelection(sel+1);
    UpdateBone();
}

void dlgAnimation::OnBoneDel(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbBones->GetSelection();
    if ((sel < 0) || (sel >= m_animation.boneanimations.size()))
        return;

    m_animation.boneanimations.erase(m_animation.boneanimations.begin() + sel);

    m_htlbBones->UpdateContents();
    m_htlbBones->SetSelection(sel-1);
    UpdateBone();
}

void dlgAnimation::OnBoneClear(wxCommandEvent& WXUNUSED(event)) {
    if (m_animation.boneanimations.size()) {
        if (::wxMessageBox(_("Do you really want to delete all animated bones?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_animation.boneanimations.clear();

    m_htlbBones->UpdateContents();
    m_htlbBones->SetSelection(-1);
    UpdateBone();
}

struct DecimInfo {
    wxString b;
    int tr;
    int ro;
    DecimInfo(const wxString& n): b(n), tr(0), ro(0) {}
};

/** @brief OnDecimate
  *
  * @todo: document this function
  */
void dlgAnimation::OnDecimate(wxCommandEvent& event) {
    boost::shared_ptr<dlgDecimate> dlg(new dlgDecimate(this), wxWindowDestroyer);
    int sel = m_htlbBones->GetSelection();
    if ((sel < 0) || (sel >= m_animation.boneanimations.size())) {
        dlg->m_choiceBones->Enable(false);
    }

    if (dlg->ShowModal() == wxID_OK) {
        vector<DecimInfo> decims;
        wxString info;
        {
            wxBusyCursor busy;
            wxBusyInfo binfo("Decimating, please wait...");
            float r = Deg2Rad(dlg->m_rotbail);
            if (dlg->m_bones) {
                DecimInfo d(m_animation.boneanimations[sel].name);
                if ((dlg->m_frames == 0) || (dlg->m_frames == 1)) {
                    d.tr = m_animation.boneanimations[sel].decimateTranslations(dlg->m_threshold);
                }
                if ((dlg->m_frames == 0) || (dlg->m_frames == 2)) {
                    d.ro = m_animation.boneanimations[sel].decimateRotations(dlg->m_threshold, r);
                }
                decims.push_back(d);
            } else {
                foreach(cBoneAnimation& b, m_animation.boneanimations) {
                    DecimInfo d(b.name);
                    if ((dlg->m_frames == 0) || (dlg->m_frames == 1)) {
                        d.tr = b.decimateTranslations(dlg->m_threshold);
                    }
                    if ((dlg->m_frames == 0) || (dlg->m_frames == 2)) {
                        d.ro = b.decimateRotations(dlg->m_threshold, r);
                    }
                    decims.push_back(d);
                }
            }
            foreach(DecimInfo& d, decims) {
                if (d.tr || d.ro)
                    info += wxString::Format(_("Bone %s: Removed %d translations and %d rotations.\n"), d.b.c_str(), d.tr, d.ro);
            }
        }
        wxMessageBox(info, _("Information"));
    }
}


////////////////////////////////////////////////////////////////////////
//
//  dlgAnimation, Current Bone
//
////////////////////////////////////////////////////////////////////////

void dlgAnimation::OnBoneName(wxCommandEvent& WXUNUSED(event)) {
    if (m_currentbone) {
        wxString sel = m_comboBoneName->GetValue();
        if (sel != wxT("")) {
            m_currentbone->name = sel;
            m_htlbBones->UpdateContents();
        }
    }
}

