///////////////////////////////////////////////////////////////////////////////
//
// Attraction Manager Dialog
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

#include "wxdlgAttractMan.h"
#include "wxdlgAttract.h"
#include "stall.h"

using namespace r3;

extern std::vector <AttractionStr *> Attractions;
extern bool save;
int CurrentAttraction;

BEGIN_EVENT_TABLE(dlgAttractMan,wxDialog)
EVT_BUTTON(XRCID("m_Add"), dlgAttractMan::OnAdd)
EVT_BUTTON(XRCID("m_Edit"), dlgAttractMan::OnEdit)
EVT_BUTTON(XRCID("m_Delete"), dlgAttractMan::OnDelete)
EVT_BUTTON(XRCID("m_Close"), dlgAttractMan::OnClose)
EVT_LISTBOX_DCLICK(XRCID("m_AttractionList"), dlgAttractMan::OnEdit)
END_EVENT_TABLE()

dlgAttractMan::dlgAttractMan(wxWindow *parent) {
    InitWidgetsFromXRC((wxWindow *)parent);
    m_AttractionList->Clear();
	for (unsigned int i = 0;i < Attractions.size();i++)
	{
	    m_AttractionList->Append(*(new wxString(Attractions[i]->Name)));
	}
}

void dlgAttractMan::OnAdd(wxCommandEvent& WXUNUSED(event))
{
    CurrentAttraction = -1;
    dlgAttract *dialog = new dlgAttract(NULL);
    if (dialog->ShowModal() == wxID_OK)
    {
        AttractionStr *s = new AttractionStr;
        s->Name = strdup(dialog->Name.c_str());
        s->AttractionType = dialog->AttractionType;
        s->NameString = strdup(dialog->NameString.c_str());
        s->DescriptionString = strdup(dialog->DescriptionString.c_str());
        s->Unk2 = dialog->Unk2;
        s->Unk3 = dialog->Unk3;
        s->SID = strdup(dialog->SID.c_str());
        Attractions.push_back(s);
        m_AttractionList->Clear();
        for (unsigned int i = 0;i < Attractions.size();i++)
        {
            m_AttractionList->Append(*(new wxString(Attractions[i]->Name)));
        }
        m_AttractionList->SetSelection(Attractions.size()-1);
    }
    dialog->Destroy();
}

void dlgAttractMan::OnEdit(wxCommandEvent& WXUNUSED(event))
{
    CurrentAttraction = m_AttractionList->GetSelection();
    dlgAttract *dialog = new dlgAttract(NULL);
    if (CurrentAttraction != -1)
    {
        AttractionStr *s = Attractions[CurrentAttraction];
        dialog->Name = *(new wxString(s->Name));
        dialog->AttractionType = s->AttractionType;
        dialog->NameString = *(new wxString(s->NameString));
        dialog->DescriptionString = *(new wxString(s->DescriptionString));
        dialog->Unk2 = s->Unk2;
        dialog->Unk3 = s->Unk3;
        dialog->SID = s->SID;
    }
    if (dialog->ShowModal() == wxID_OK)
    {
        AttractionStr *s;
        if (CurrentAttraction != -1)
        {
            s = Attractions[CurrentAttraction];
        }
        else
        {
            s = new AttractionStr;
        }
        s->Name = strdup(dialog->Name.c_str());
        s->AttractionType = dialog->AttractionType;
        s->NameString = strdup(dialog->NameString.c_str());
        s->DescriptionString = strdup(dialog->DescriptionString.c_str());
        s->Unk2 = dialog->Unk2;
        s->Unk3 = dialog->Unk3;
        s->SID = strdup(dialog->SID.c_str());
        if (CurrentAttraction == -1)
        {
            Attractions.push_back(s);
        }

        m_AttractionList->Clear();
        for (unsigned int i = 0;i < Attractions.size();i++)
        {
            m_AttractionList->Append(*(new wxString(Attractions[i]->Name)));
        }
        m_AttractionList->SetSelection(CurrentAttraction);
    }
    dialog->Destroy();
}

void dlgAttractMan::OnDelete(wxCommandEvent& WXUNUSED(event))
{
    if (::wxMessageBox(_("Are you sure you want to delete?"), _("Are You Sure"), wxYES_NO | wxICON_QUESTION, this) == wxYES)
    {
        CurrentAttraction = m_AttractionList->GetSelection();
        if (CurrentAttraction != -1)
        {
            delete Attractions[CurrentAttraction]->Name;
            delete Attractions[CurrentAttraction]->NameString;
            delete Attractions[CurrentAttraction]->DescriptionString;
            delete Attractions[CurrentAttraction]->SID;
            delete Attractions[CurrentAttraction];
            Attractions.erase(Attractions.begin() + CurrentAttraction);
            m_AttractionList->Clear();
            for (unsigned int i = 0;i < Attractions.size();i++)
            {
                m_AttractionList->Append(*(new wxString(Attractions[i]->Name)));
            }
            m_AttractionList->SetSelection(CurrentAttraction-1);
        }
    }
}

void dlgAttractMan::OnClose(wxCommandEvent& WXUNUSED(event))
{
    EndModal(0);
}
