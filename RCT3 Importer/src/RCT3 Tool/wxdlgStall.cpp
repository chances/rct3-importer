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
#include "stall.h"

extern std::vector <StallStr *> Stalls;
extern std::vector<cText> cTextStrings;
extern std::vector <Scenery *> SceneryItems;
extern bool save;
extern int CurrentStall;

BEGIN_EVENT_TABLE(dlgStall,wxDialog)
EVT_BUTTON(XRCID("m_btDefault"), dlgStall::OnDefault)
END_EVENT_TABLE()

dlgStall::dlgStall(wxWindow *parent) {
    InitWidgetsFromXRC((wxWindow *)parent);
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
    m_Name->SetValidator(wxGenericValidator(&Name));
    m_Type->SetValidator(wxGenericValidator((int *)&StallType));
    m_NameString->SetValidator(wxGenericValidator(&NameString));
    m_Description->SetValidator(wxGenericValidator(&DescriptionString));
    m_Unk2->SetValidator(wxGenericValidator((int *)&Unk2));
    m_Unk3->SetValidator(wxGenericValidator((int *)&Unk3));
    m_SceneryItem->SetValidator(wxGenericValidator(&SID));
    m_Unk11->SetValidator(wxGenericValidator((int *)&Unk11));
    m_Unk12->SetValidator(wxGenericValidator((int *)&Unk12));
    m_Unk13->SetValidator(wxGenericValidator((int *)&Unk13));
    m_Unk14->SetValidator(wxGenericValidator((int *)&Unk14));
    m_Unk15->SetValidator(wxGenericValidator((int *)&Unk15));
    m_Unk16->SetValidator(wxGenericValidator((int *)&Unk16));
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
