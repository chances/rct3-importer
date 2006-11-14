///////////////////////////////////////////////////////////////////////////////
//
// Stall Manager Dialog
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

#include "wxdlgStallMan.h"
#include "wxdlgStall.h"
#include "stall.h"

extern std::vector <StallStr *> Stalls;
extern bool save;
int CurrentStall;

BEGIN_EVENT_TABLE(dlgStallMan,wxDialog)
EVT_BUTTON(XRCID("m_Add"), dlgStallMan::OnAdd)
EVT_BUTTON(XRCID("m_Edit"), dlgStallMan::OnEdit)
EVT_BUTTON(XRCID("m_Delete"), dlgStallMan::OnDelete)
EVT_BUTTON(XRCID("m_Close"), dlgStallMan::OnClose)
END_EVENT_TABLE()

dlgStallMan::dlgStallMan(wxWindow *parent) {
    InitWidgetsFromXRC((wxWindow *)parent);
    m_StallList->Clear();
	for (unsigned int i = 0;i < Stalls.size();i++)
	{
	    m_StallList->Append(*(new wxString(Stalls[i]->Name)));
	}
}

void dlgStallMan::OnAdd(wxCommandEvent& WXUNUSED(event))
{
    CurrentStall = -1;
    dlgStall *dialog = new dlgStall(NULL);
    if (dialog->ShowModal() == wxID_OK)
    {
        StallStr *s = new StallStr;
        s->Name = strdup(dialog->Name.c_str());
        s->StallType = dialog->StallType;
        s->NameString = strdup(dialog->NameString.c_str());
        s->DescriptionString = strdup(dialog->DescriptionString.c_str());
        s->Unk2 = dialog->Unk2;
        s->Unk3 = dialog->Unk3;
        s->SID = strdup(dialog->SID.c_str());
        s->Unk11 = dialog->Unk11;
        s->Unk12 = dialog->Unk12;
        s->Unk13 = dialog->Unk13;
        s->Unk14 = dialog->Unk14;
        s->Unk15 = dialog->Unk15;
        s->Unk16 = dialog->Unk16;
        s->Items = 0;
        s->ItemCount = 0;
        Stalls.push_back(s);

        m_StallList->Clear();
        for (unsigned int i = 0;i < Stalls.size();i++)
        {
            m_StallList->Append(*(new wxString(Stalls[i]->Name)));
        }
        m_StallList->SetSelection(Stalls.size()-1);
    }
    dialog->Destroy();
}

void dlgStallMan::OnEdit(wxCommandEvent& WXUNUSED(event))
{
    CurrentStall = m_StallList->GetSelection();
    dlgStall *dialog = new dlgStall(NULL);
    if (CurrentStall != -1)
    {
        StallStr *s = Stalls[CurrentStall];
        dialog->Name = *(new wxString(s->Name));
        dialog->StallType = s->StallType;
        dialog->NameString = *(new wxString(s->NameString));
        dialog->DescriptionString = *(new wxString(s->DescriptionString));
        dialog->Unk2 = s->Unk2;
        dialog->Unk3 = s->Unk3;
        dialog->SID = s->SID;
        dialog->Unk11 = s->Unk11;
        dialog->Unk12 = s->Unk12;
        dialog->Unk13 = s->Unk13;
        dialog->Unk14 = s->Unk14;
        dialog->Unk15 = s->Unk15;
        dialog->Unk16 = s->Unk16;
    }
    if (dialog->ShowModal() == wxID_OK)
    {
        StallStr *s;
        if (CurrentStall != -1)
        {
            s = Stalls[CurrentStall];
        }
        else
        {
            s = new StallStr;
        }
        s->Name = strdup(dialog->Name.c_str());
        s->StallType = dialog->StallType;
        s->NameString = strdup(dialog->NameString.c_str());
        s->DescriptionString = strdup(dialog->DescriptionString.c_str());
        s->Unk2 = dialog->Unk2;
        s->Unk3 = dialog->Unk3;
        s->SID = strdup(dialog->SID.c_str());
        s->Unk11 = dialog->Unk11;
        s->Unk12 = dialog->Unk12;
        s->Unk13 = dialog->Unk13;
        s->Unk14 = dialog->Unk14;
        s->Unk15 = dialog->Unk15;
        s->Unk16 = dialog->Unk16;
        s->Items = 0;
        s->ItemCount = 0;
        if (CurrentStall == -1)
        {
            Stalls.push_back(s);
        }

        m_StallList->Clear();
        for (unsigned int i = 0;i < Stalls.size();i++)
        {
            m_StallList->Append(*(new wxString(Stalls[i]->Name)));
        }
        m_StallList->SetSelection(CurrentStall);
    }
    dialog->Destroy();
}

void dlgStallMan::OnDelete(wxCommandEvent& WXUNUSED(event))
{
    if (::wxMessageBox(_("Are you sure you want to delete?"), _("Are You Sure"), wxYES_NO | wxICON_QUESTION, this) == wxYES)
    {
        CurrentStall = m_StallList->GetSelection();
        if (CurrentStall != -1)
        {
            delete Stalls[CurrentStall]->Name;
            delete Stalls[CurrentStall]->NameString;
            delete Stalls[CurrentStall]->DescriptionString;
            delete Stalls[CurrentStall]->SID;
            if (Stalls[CurrentStall]->Items)
            {
                delete Stalls[CurrentStall]->Items;
            }
            delete Stalls[CurrentStall];
            Stalls.erase(Stalls.begin() + CurrentStall);
            m_StallList->Clear();
            for (unsigned int i = 0;i < Stalls.size();i++)
            {
                m_StallList->Append(*(new wxString(Stalls[i]->Name)));
            }
            m_StallList->SetSelection(CurrentStall-1);
        }
    }
}

void dlgStallMan::OnClose(wxCommandEvent& WXUNUSED(event))
{
    EndModal(0);
}
