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

#include <wx/valgen.h>

#include "colhtmllbox.h"
#include "htmlentities.h"
#include "matrix.h"
#include "valext.h"
#include "wxInputBox.h"

#include "wxdlgCreateScenery.h"

#include "wxdlgAnimation.h"

////////////////////////////////////////////////////////////////////////
//
//  wxBonesListBox
//
////////////////////////////////////////////////////////////////////////

class wxBonesListBox: public wxColourHtmlListBox {
private:
    cAnimation* m_contents;
public:
    wxBonesListBox(wxWindow *parent, cAnimation *content):
            wxColourHtmlListBox(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxSUNKEN_BORDER) {
        wxLogDebug(wxT("Trace, wxBonesListBox::Create"));
        m_contents = content;
        UpdateContents();
    };
    void UpdateContents() {
        wxLogDebug(wxT("wxBonesListBox::UpdateContents Size:%d"), m_contents->boneanimations.size());
        SetItemCount(m_contents->boneanimations.size());
        if (GetSelection() >= m_contents->boneanimations.size())
            SetSelection(wxNOT_FOUND);
        RefreshAll();
    };
    virtual wxString OnGetItem(size_t n) const {
        wxLogDebug(wxT("wxBonesListBox::OnGetItem(%d) '%d'"), n, m_contents->boneanimations.size());
        if (n < m_contents->boneanimations.size()) {
            wxString addon = wxString::Format(wxT("%d&nbsp;translations<br>%d&nbsp;rotations"), m_contents->boneanimations[n].translations.size(), m_contents->boneanimations[n].rotations.size());
            wxString ret = wxT("<font size='2'>")+wxEncodeHtmlEntities(m_contents->boneanimations[n].name)
                +HTML_INSET_START+addon+HTML_INSET_END
                +wxT("</font>");
            wxLogDebug(wxT("wxBonesListBox::OnGetItem(%d) '%s'"), n, ret.c_str());
            return ret;
        } else {
                return wxT("Internal Error");
        }
    }
};


////////////////////////////////////////////////////////////////////////
//
//  wxTranslationListBox
//
////////////////////////////////////////////////////////////////////////

class wxTranslationListBox: public wxColourHtmlListBox {
private:
    cBoneAnimation** m_contents;
public:
    wxTranslationListBox(wxWindow *parent, cBoneAnimation **content):
            wxColourHtmlListBox(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxSUNKEN_BORDER) {
        m_contents = content;
        UpdateContents();
    };
    void UpdateContents() {
        if (*m_contents) {
            SetItemCount((*m_contents)->translations.size());
            if (GetSelection() >= (*m_contents)->translations.size())
                SetSelection(wxNOT_FOUND);
        } else {
            SetItemCount(0);
            SetSelection(wxNOT_FOUND);
        }
        RefreshAll();
    };
    virtual wxString OnGetItem(size_t n) const {
        if (*m_contents) {
            if (n < (*m_contents)->translations.size()) {
                return wxT("<font size='2'>")+wxString::Format(wxT("%.2f s"), (*m_contents)->translations[n].v.Time)
                    +HTML_INSET_START+wxT("&lt;")+wxString::Format(wxT("%.6f,%.6f,%.6f"), (*m_contents)->translations[n].v.X, (*m_contents)->translations[n].v.Y, (*m_contents)->translations[n].v.Z)+wxT("&gt;")
                    +HTML_INSET_END+wxT("</font>");
            } else {
                    return wxT("Internal Error");
            }
        } else
            return wxT("");
    }
};


////////////////////////////////////////////////////////////////////////
//
//  wxRotationListBox
//
////////////////////////////////////////////////////////////////////////

class wxRotationListBox: public wxColourHtmlListBox {
private:
    cBoneAnimation** m_contents;
public:
    wxRotationListBox(wxWindow *parent, cBoneAnimation **content):
            wxColourHtmlListBox(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxSUNKEN_BORDER) {
        m_contents = content;
        UpdateContents();
    };
    void UpdateContents() {
        if (*m_contents) {
            SetItemCount((*m_contents)->rotations.size());
            if (GetSelection() >= (*m_contents)->rotations.size())
                SetSelection(wxNOT_FOUND);
        } else {
            SetItemCount(0);
            SetSelection(wxNOT_FOUND);
        }
        RefreshAll();
    };
    virtual wxString OnGetItem(size_t n) const {
        if (*m_contents) {
            if (n < (*m_contents)->rotations.size()) {
                return wxT("<font size='2'>")+wxString::Format(wxT("%.2f s"), (*m_contents)->rotations[n].v.Time)
                    +HTML_INSET_START+wxT("&lt;")+wxString::Format(wxT("%.2f°,%.2f°,%.2f°"), Rad2Deg((*m_contents)->rotations[n].v.X), Rad2Deg((*m_contents)->rotations[n].v.Y), Rad2Deg((*m_contents)->rotations[n].v.Z))+wxT("&gt;")
                    +HTML_INSET_END+wxT("</font>");
            } else {
                    return wxT("Internal Error");
            }
        } else
            return wxT("");
    }
};


////////////////////////////////////////////////////////////////////////
//
//  dlgAnimation
//
////////////////////////////////////////////////////////////////////////
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

dlgAnimation::dlgAnimation(wxWindow *parent): m_currentbone(NULL) {
    wxLogDebug(wxT("Trace, dlgAnimation::Create"));
    InitWidgetsFromXRC((wxWindow *)parent);

    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

    m_staticXRot->SetLabel(wxT("X (°)"));
    m_staticYRot->SetLabel(wxT("Y (°)"));
    m_staticZRot->SetLabel(wxT("Z (°)"));

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

    m_htlbBones = new wxBonesListBox(this, &m_animation);
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbBones"), m_htlbBones, this);
    m_htlbTranslations = new wxTranslationListBox(m_panCurrent, &m_currentbone);
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbTranslations"), m_htlbTranslations, m_panCurrent);
    m_htlbRotations = new wxRotationListBox(m_panCurrent, &m_currentbone);
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbRotations"), m_htlbRotations, m_panCurrent);

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
    } else {
        m_currentbone = NULL;
        m_htlbTranslations->UpdateContents();
        m_htlbRotations->UpdateContents();
        m_comboBoneName->SetValue(wxT(""));
        m_textTimeTranslation->ChangeValue(wxT(""));
        m_textXTranslation->ChangeValue(wxT(""));
        m_textYTranslation->ChangeValue(wxT(""));
        m_textZTranslation->ChangeValue(wxT(""));
        m_textTimeRotation->ChangeValue(wxT(""));
        m_textXRotation->ChangeValue(wxT(""));
        m_textYRotation->ChangeValue(wxT(""));
        m_textZRotation->ChangeValue(wxT(""));
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

void dlgAnimation::OnBoneAdd(wxCommandEvent& WXUNUSED(event)) {
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

