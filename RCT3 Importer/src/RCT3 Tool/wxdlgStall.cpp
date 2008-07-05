///////////////////////////////////////////////////////////////////////////////
//
// Stall Dialog
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

#include <wx/valtext.h>
#include <wx/valgen.h>

#include "wxdlgStall.h"
#include "scenery.h"
#include "stall.h"

#include "valsimplelist.h"
#include "valext.h"

using namespace r3;

extern std::vector <r3old::StallStr *> Stalls;
extern std::vector<cText> cTextStrings;
extern std::vector <r3old::Scenery *> SceneryItems;
extern bool save;
extern int CurrentStall;

BEGIN_EVENT_TABLE(dlgStall,wxDialog)
EVT_BUTTON(XRCID("m_btDefault"), dlgStall::OnDefault)
EVT_CHOICE(XRCID("m_SceneryItem"), dlgStall::OnNameString)
END_EVENT_TABLE()

dlgStall::dlgStall(wxWindow *parent) {
    InitWidgetsFromXRC((wxWindow *)parent);
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

    for (unsigned int i = 0;i < cTextStrings.size();i++)
    {
        m_NameString->Append(cTextStrings[i].name);
        m_Description->Append(cTextStrings[i].name);
    }
    for (unsigned int i = 0;i < SceneryItems.size();i++)
    {
        char name[MAX_PATH];
        _splitpath(SceneryItems[i]->ovl, NULL, NULL, name, NULL);
        strchr(name, '.')[0] = 0;
        m_SceneryItem->Append(*(new wxString(name)));
    }
    Unk2 = 0;
    Unk3 = 0;
    Unk11 = 0;
    Unk12 = 0;
    Unk13 = 0;
    Unk14 = 0;
    Unk15 = 0;
    Unk16 = 0;
    m_Name->SetValidator(wxExtendedValidator(&Name, false));
    m_Type->SetValidator(wxGenericValidator((int *)&StallType));
    m_NameString->SetValidator(wxSimpleListValidator(&NameString));
    m_Description->SetValidator(wxSimpleListValidator(&DescriptionString));
    m_Unk2->SetValidator(wxGenericValidator((int *)&Unk2));
    m_Unk3->SetValidator(wxGenericValidator((int *)&Unk3));
    m_SceneryItem->SetValidator(wxSimpleListValidator(&SID));
    m_Unk11->SetValidator(wxGenericValidator((int *)&Unk11));
    m_Unk12->SetValidator(wxGenericValidator((int *)&Unk12));
    m_Unk13->SetValidator(wxGenericValidator((int *)&Unk13));
    m_Unk14->SetValidator(wxGenericValidator((int *)&Unk14));
    m_Unk15->SetValidator(wxGenericValidator((int *)&Unk15));
    m_Unk16->SetValidator(wxGenericValidator((int *)&Unk16));

    wxInputBox *t_ibName = new wxInputBox(this, wxID_ANY);
    t_ibName->SetEditor(m_Name);
    m_ibNameString = new wxInputBox(this, wxID_ANY);
    m_ibNameString->SetEditor(m_NameString);
    m_ibDescription = new wxInputBox(this, wxID_ANY);
    m_ibDescription->SetEditor(m_Description);
    m_ibSceneryItem = new wxInputBox(this, wxID_ANY);
    m_ibSceneryItem->SetEditor(m_SceneryItem);

    Fit();
    Layout();
    GetSizer()->SetSizeHints(this);

    m_OK->SetId(wxID_OK);
    m_Cancel->SetId(wxID_CANCEL);
}

void dlgStall::OnDefault(wxCommandEvent& WXUNUSED(event)) {
    m_Unk2->SetValue(wxT("10000"));
    m_Unk3->SetValue(wxT("-7500"));
    m_Unk11->SetValue(wxT("6"));
    m_Unk12->SetValue(wxT("7"));
    m_Unk13->SetValue(wxT("5"));
    m_Unk14->SetValue(wxT("7"));
    m_Unk15->SetValue(wxT("8"));
    m_Unk16->SetValue(wxT("9"));
}

void dlgStall::OnNameString(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_SceneryItem->GetSelection();
    if ((sel != wxNOT_FOUND) && (m_Name->GetValue() == wxT(""))) {
        m_Name->SetValue(m_SceneryItem->GetStringSelection());
    }
}

bool dlgStall::TransferDataToWindow() {
    bool ret = wxDialog::TransferDataToWindow();
    m_ibDescription->Update();
    m_ibNameString->Update();
    m_ibSceneryItem->Update();
    return ret;
}
