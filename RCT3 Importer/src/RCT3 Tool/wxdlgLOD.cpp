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

#include "wxdlgLOD.h"
#include "wxdlgLOD_HTLB.h"

#include <wx/choicdlg.h>
#include <wx/valgen.h>

#include "colhtmllbox.h"
#include "htmlentities.h"
#include "valext.h"
#include "SCNFile.h"
#include "wxutilityfunctions.h"
#include "pretty.h"

using namespace std;

////////////////////////////////////////////////////////////////////////
//
//  dlgLOD
//
////////////////////////////////////////////////////////////////////////

/*
BEGIN_EVENT_TABLE(dlgLOD,wxDialog)
EVT_CHOICE(XRCID("m_choiceModel"), dlgLOD::OnModelChange)
EVT_CHECKBOX(XRCID("m_checkShowUnknown"), dlgLOD::OnShowUnknowns)
EVT_BUTTON(XRCID("m_btDist40"), dlgLOD::OnDist40)
EVT_BUTTON(XRCID("m_btDist100"), dlgLOD::OnDist100)
EVT_BUTTON(XRCID("m_btDist4000"), dlgLOD::OnDist4000)
END_EVENT_TABLE()
*/

dlgLOD::dlgLOD(const cSCNFile& scn, wxWindow *parent):rcdlgLOD(parent), m_scn(scn), m_warned(false) {
//    InitWidgetsFromXRC((wxWindow *)parent);
//    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

//    m_htlbAnimations = new wxLODAnimationListBox(this, &m_lod);
//    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbAnimations"), m_htlbAnimations, this);
    m_htlbAnimations->Init(&m_lod);

/*
    m_gxpictAni = new wxGXPicture(this, wxID_ANY, wxT(""), true, wxDefaultPosition, wxSize(16,16), wxNO_BORDER);
    //m_gxpictAni->SetBackgroundColour(*wxWHITE);
    m_gxpictAni->SetMinSize(wxSize(16, 16));
    m_gxpictAni->SetScale(wxGXSCALE_NONE);
    m_gxpictAni->SetAlignment(wxALIGN_CENTER);
    m_gxpictAni->UseAlpha();
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_gxpictAni"), m_gxpictAni, this);
*/

    if (m_scn.models.size()) {
        m_choiceType->SetSelection(0);
        m_lod.animated = false;
        m_lod.modelname = m_scn.models[0].name;
    } else {
        m_choiceType->SetSelection(1);
        m_lod.animated = true;
        m_lod.modelname = m_scn.animatedmodels[0].name;
    }

    UpdateAll();

    m_textDistance->SetValidator(wxExtendedValidator(&m_lod.distance, 1));
    m_textUnknown2->SetValidator(wxExtendedValidator(&m_lod.unk2));
    m_textUnknown4->SetValidator(wxExtendedValidator(&m_lod.unk4));
    m_textUnknown14->SetValidator(wxExtendedValidator(&m_lod.unk14));

    m_btOk->SetId(wxID_OK);
    m_btCancel->SetId(wxID_CANCEL);

    Fit();
    Layout();
    GetSizer()->SetSizeHints(this);
}

void dlgLOD::UpdateAll() {
    UpdateModels();
    m_htlbAnimations->UpdateContents();
    UpdateControlState();
}

/** @brief UpdateModels
  *
  * @todo: document this function
  */
void dlgLOD::UpdateModels() {
    int v = m_choiceType->GetSelection();
    m_choiceModel->Clear();
    wxString zeroname;
    if (v == 0) {
        for (cModel::const_iterator it = m_scn.models.begin(); it != m_scn.models.end(); ++it) {
            m_choiceModel->Append(it->name);
        }
        zeroname = m_scn.models[0].name;
    } else {
        for (cAnimatedModel::const_iterator it =  m_scn.animatedmodels.begin(); it != m_scn.animatedmodels.end(); ++it) {
            m_choiceModel->Append(it->name);
        }
        zeroname = m_scn.animatedmodels[0].name;
    }
    if (!m_choiceModel->SetStringSelection(m_lod.modelname)) {
        m_choiceModel->SetStringSelection(zeroname);
        wxCommandEvent ev;
        OnModelChange(ev);
    }
}


void dlgLOD::UpdateControlState() {
    int count = m_lod.animations.size();
    int sel = m_htlbAnimations->GetSelection();
    m_spinAnim->Enable(count>=2);
    m_btAnimAdd->Enable(count < m_scn.animations.size());
    m_btAnimDel->Enable((sel>=0) && (sel<count));
    m_btAnimClear->Enable(count>=1);
    if (m_lod.animated) {
        m_nbLOD->SetPageText(1, wxString::Format(_("Animations (%d)"), count));
    } else {
        m_nbLOD->SetPageText(1,_("(Animations)"));
    }

/*
    if (m_animatedmodels) {
        m_lod.animated = false;
        wxString sel = m_choiceModel->GetStringSelection();
        for (cAnimatedModel::iterator it = m_animatedmodels->begin(); it != m_animatedmodels->end(); ++it) {
            if (it->name == sel) {
                m_lod.animated = true;
                break;
            }
        }

        if (!m_lod.animated) {
            m_gxpictAni->LoadFromFileSystem(wxT("memory:XRC_resource/xrc_bitmap_crystal.cpp$icons_folder_home_16x16.png"));
        } else {
            m_gxpictAni->LoadFromFileSystem(wxT("memory:XRC_resource/xrc_bitmap_crystal.cpp$icons_aim_online_16x16.png"));
        }

    } else {
        m_gxpictAni->LoadFromFileSystem(wxT("memory:XRC_resource/xrc_bitmap_crystal.cpp$icons_no_16x16.png"));
    }
*/
}

/** @brief OnNBChanging
  *
  * @todo: document this function
  */
void dlgLOD::OnNBChanging(wxNotebookEvent& event) {
    if ((event.GetSelection() == 1) && (!m_lod.animated))
        m_nbLOD->SetSelection(event.GetOldSelection());
    if ((event.GetSelection() == 2) && (!m_warned)) {
        if (::wxMessageBox(_("You should better leave these settings alone.\nDo you want to continue?"), _("Warning"), wxYES_NO | wxNO_DEFAULT | wxICON_WARNING, this)==wxNO) {
            m_nbLOD->SetSelection(event.GetOldSelection());
        }
        m_warned = true;
    }
    event.Skip();
}

void dlgLOD::OnModelChange(wxCommandEvent& WXUNUSED(event)) {
    m_lod.modelname = m_choiceModel->GetStringSelection();
    UpdateControlState();
}

/*
void dlgLOD::OnShowUnknowns(wxCommandEvent& WXUNUSED(event)) {
//    if (m_checkShowUnknown->IsChecked()) {
//        if (::wxMessageBox(_("You should better leave these settings alone.\nDo you want to continue?"), _("Warning"), wxYES_NO | wxNO_DEFAULT | wxICON_WARNING, this)==wxNO) {
//            m_checkShowUnknown->SetValue(false);
//            return;
//        }
//    }
//    TransferDataFromWindow();
    m_panelUnknown->Show( m_checkShowUnknown->IsChecked());
    Fit();
    Layout();
//    TransferDataToWindow();
}
*/

/** @brief OnTypeChange
  *
  * @todo: document this function
  */
void dlgLOD::OnTypeChange(wxCommandEvent& event) {
    int v = m_choiceType->GetSelection();
    if (v) {
        if (m_lod.animated)
            return;
        if (!m_scn.animatedmodels.size()) {
            ::wxMessageBox(_("No animated models defined!"), _("Error"), wxICON_ERROR, this);
            m_choiceType->SetSelection(0);
            return;
        }
        m_lod.animated = true;
        UpdateAll();
    } else {
        if (!m_lod.animated)
            return;
        if (!m_scn.models.size()) {
            ::wxMessageBox(_("No static models defined!"), _("Error"), wxICON_ERROR, this);
            m_choiceType->SetSelection(1);
            return;
        }
        if (m_lod.animations.size()) {
            if (::wxMessageBox(_("This will delete all assigned animations. Do you want to continue?"), _("Are you sure?"), wxICON_QUESTION|wxYES_NO|wxNO_DEFAULT, this) != wxYES) {
                m_choiceType->SetSelection(1);
                return;
            }
        }
        m_lod.animations.clear();
        m_lod.animated = false;
        UpdateAll();
    }
}

void dlgLOD::OnDist40(wxCommandEvent& WXUNUSED(event)) {
    m_textDistance->ChangeValue(wxT("40.0"));
}

void dlgLOD::OnDist100(wxCommandEvent& WXUNUSED(event)) {
    m_textDistance->ChangeValue(wxT("100.0"));
}

void dlgLOD::OnDist4000(wxCommandEvent& WXUNUSED(event)) {
    m_textDistance->ChangeValue(wxT("4000.0"));
}

/** @brief OnUpdateControlState
  *
  * @todo: document this function
  */
void dlgLOD::OnUpdateControlState(wxCommandEvent& event) {
    UpdateControlState();
}

/** @brief OnAnimEdit
  *
  * @todo: document this function
  */
void dlgLOD::OnAnimEdit(wxCommandEvent& event) {
    int sel = m_htlbAnimations->GetSelection();
    if (sel<0)
        return;

    wxString newname = ::wxGetTextFromUser(_("Enter new name for the animation"), _("Change animation name"), m_lod.animations[sel], this);
    if (!newname.IsEmpty()) {
        m_lod.animations[sel] = newname;

        m_htlbAnimations->UpdateContents();
        m_htlbAnimations->SetSelection(sel);
        UpdateControlState();
    }
}

/** @brief OnAnimationClear
  *
  * @todo: document this function
  */
void dlgLOD::OnAnimationClear(wxCommandEvent& event) {
    if (m_lod.animations.size()) {
        if (::wxMessageBox(_("Do you really want to delete all animations?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_lod.animations.clear();

    m_htlbAnimations->UpdateContents();
    m_htlbAnimations->SetSelection(-1);
    UpdateControlState();
}

/** @brief OnAnimationDel
  *
  * @todo: document this function
  */
void dlgLOD::OnAnimationDel(wxCommandEvent& event) {
    int sel = m_htlbAnimations->GetSelection();
    if (sel<0)
        return;

    m_lod.animations.erase(m_lod.animations.begin() + sel);

    m_htlbAnimations->UpdateContents();
    m_htlbAnimations->SetSelection(sel-1);
    UpdateControlState();
}

/** @brief OnAnimationAdd
  *
  * @todo: document this function
  */
void dlgLOD::OnAnimationAdd(wxCommandEvent& event) {
    vector<wxString> ani(m_scn.animations.size());
    transform(m_scn.animations.begin(), m_scn.animations.end(), ani.begin(), NameExtractorC<cAnimation>);

    vector<wxString>::iterator end = ani.end();
    foreach(const wxString& an, m_lod.animations)
        end = remove(ani.begin(), end, an);
    ani.resize(end - ani.begin());

    if (ani.size() > 1) {
        boost::shared_ptr<wxMultiChoiceDialog> dlg(new wxMultiChoiceDialog(this, _("Select animations to add:"), _("Add animations"), ani.size(), &ani[0]), wxWindowDestroyer);
        dlg->ShowModal();

        wxArrayInt sel = dlg->GetSelections();
        foreach(int i, sel)
            m_lod.animations.push_back(ani[i]);
    } else if (ani.size()) {
        m_lod.animations.push_back(ani[0]);
    }

    m_htlbAnimations->UpdateContents();
    UpdateControlState();
}

/** @brief OnAnimationUp
  *
  * @todo: document this function
  */
void dlgLOD::OnAnimationUp(wxSpinEvent& event) {
    int sel = m_htlbAnimations->GetSelection();
    if (sel < 1)
        return;
    wxString ft = m_lod.animations[sel];
    m_lod.animations.erase(m_lod.animations.begin() + sel);
    m_lod.animations.insert(m_lod.animations.begin() + sel - 1, ft);

    m_htlbAnimations->UpdateContents();
    m_htlbAnimations->SetSelection(sel-1);
    UpdateControlState();
}

/** @brief OnAnimationDown
  *
  * @todo: document this function
  */
void dlgLOD::OnAnimationDown(wxSpinEvent& event) {
    int count = m_lod.animations.size();
    int sel = m_htlbAnimations->GetSelection();
    if ((count-sel) <= 1)
        return;
    wxString ft = m_lod.animations[sel];
    m_lod.animations.erase(m_lod.animations.begin() + sel);
    m_lod.animations.insert(m_lod.animations.begin() + sel + 1, ft);

    m_htlbAnimations->UpdateContents();
    m_htlbAnimations->SetSelection(sel+1);
    UpdateControlState();
}



