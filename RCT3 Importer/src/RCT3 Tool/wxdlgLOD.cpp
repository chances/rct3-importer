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

#include "valext.h"

////////////////////////////////////////////////////////////////////////
//
//  dlgLOD
//
////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(dlgLOD,wxDialog)
EVT_CHECKBOX(XRCID("m_checkShowUnknown"), dlgLOD::OnShowUnknowns)
EVT_BUTTON(XRCID("m_btDist40"), dlgLOD::OnDist40)
EVT_BUTTON(XRCID("m_btDist100"), dlgLOD::OnDist100)
EVT_BUTTON(XRCID("m_btDist4000"), dlgLOD::OnDist4000)
END_EVENT_TABLE()

dlgLOD::dlgLOD(const std::vector<cModel>& mods, wxWindow *parent) {
    InitWidgetsFromXRC((wxWindow *)parent);

    for(int i = 0; i < mods.size(); i++) {
        m_choiceModel->Append(mods[i].name);
    }
    m_choiceModel->SetSelection(0);

    m_choiceModel->SetValidator(wxGenericValidator(&m_lod.modelname));
    m_textDistance->SetValidator(wxExtendedValidator(&m_lod.distance, 1));
    m_textUnknown2->SetValidator(wxExtendedValidator(&m_lod.unk2));
    m_textUnknown4->SetValidator(wxExtendedValidator(&m_lod.unk4));
    m_textUnknown14->SetValidator(wxExtendedValidator(&m_lod.unk14));

    m_btOk->SetId(wxID_OK);
    m_btCancel->SetId(wxID_CANCEL);
}

void dlgLOD::OnShowUnknowns(wxCommandEvent& WXUNUSED(event)) {
    if (m_checkShowUnknown->IsChecked()) {
        if (::wxMessageBox(_("You should better leave these settings alone.\nDo you want to continue?"), _("Warning"), wxYES_NO | wxNO_DEFAULT | wxICON_WARNING, this)==wxNO) {
            m_checkShowUnknown->SetValue(false);
            return;
        }
    }
    TransferDataFromWindow();
    m_panelUnknown->Show( m_checkShowUnknown->IsChecked());
    Fit();
    Layout();
    TransferDataToWindow();
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

