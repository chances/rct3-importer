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

#include "wx_pch.h"

#include "wxdlgLOD.h"

#include <wx/valgen.h>

#include "colhtmllbox.h"
#include "htmlentities.h"
#include "valext.h"

////////////////////////////////////////////////////////////////////////
//
//  wxReferenceListBox
//
////////////////////////////////////////////////////////////////////////

class wxLODAnimationListBox : public wxColourHtmlListBox {
public:
    wxLODAnimationListBox(wxWindow *parent, cLOD *contents):
        wxColourHtmlListBox(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxSUNKEN_BORDER) {
        m_contents = contents;
        UpdateContents();
        SetSelection(0);
    }
    virtual void UpdateContents() {
        SetItemCount(m_contents->animations.size());
        if (GetSelection() >= m_contents->animations.size())
            SetSelection(wxNOT_FOUND);
        RefreshAll();
    }
protected:
    cLOD* m_contents;
    virtual wxString OnGetItem(size_t n) const {
        return wxT("<font size='2'>")+wxEncodeHtmlEntities(m_contents->animations[n])+wxT("</font>");
    }
};

////////////////////////////////////////////////////////////////////////
//
//  dlgLOD
//
////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(dlgLOD,wxDialog)
EVT_CHOICE(XRCID("m_choiceModel"), dlgLOD::OnModelChange)
EVT_CHECKBOX(XRCID("m_checkShowUnknown"), dlgLOD::OnShowUnknowns)
EVT_BUTTON(XRCID("m_btDist40"), dlgLOD::OnDist40)
EVT_BUTTON(XRCID("m_btDist100"), dlgLOD::OnDist100)
EVT_BUTTON(XRCID("m_btDist4000"), dlgLOD::OnDist4000)
END_EVENT_TABLE()

dlgLOD::dlgLOD(cModel::vec* mods, cAnimatedModel::vec* amods, wxWindow *parent) {
    InitWidgetsFromXRC((wxWindow *)parent);
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

    m_models = mods;
    m_animatedmodels = amods;

    m_htlbAnimations = new wxLODAnimationListBox(this, &m_lod);
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbAnimations"), m_htlbAnimations, this);
    m_gxpictAni = new wxGXPicture(this, wxID_ANY, wxT(""), true, wxDefaultPosition, wxSize(16,16), wxNO_BORDER);
    //m_gxpictAni->SetBackgroundColour(*wxWHITE);
    m_gxpictAni->SetMinSize(wxSize(16, 16));
    m_gxpictAni->SetScale(wxGXSCALE_NONE);
    m_gxpictAni->SetAlignment(wxALIGN_CENTER);
    m_gxpictAni->UseAlpha();
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_gxpictAni"), m_gxpictAni, this);

    if (m_models) {
        for (cModel::iterator it = m_models->begin(); it != m_models->end(); ++it) {
            m_choiceModel->Append(it->name);
        }
    }
    if (m_animatedmodels) {
        for (cAnimatedModel::iterator it = m_animatedmodels->begin(); it != m_animatedmodels->end(); ++it) {
            m_choiceModel->Append(it->name);
        }
    }
    m_choiceModel->SetSelection(0);

    m_choiceModel->SetValidator(wxGenericValidator(&m_lod.modelname));
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
    m_htlbAnimations->UpdateContents();
    UpdateControlState();
}

void dlgLOD::UpdateControlState() {
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
}

void dlgLOD::OnModelChange(wxCommandEvent& WXUNUSED(event)) {
    UpdateControlState();
}

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

void dlgLOD::OnDist40(wxCommandEvent& WXUNUSED(event)) {
    m_textDistance->ChangeValue(wxT("40.0"));
}

void dlgLOD::OnDist100(wxCommandEvent& WXUNUSED(event)) {
    m_textDistance->ChangeValue(wxT("100.0"));
}

void dlgLOD::OnDist4000(wxCommandEvent& WXUNUSED(event)) {
    m_textDistance->ChangeValue(wxT("4000.0"));
}

