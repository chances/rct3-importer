///////////////////////////////////////////////////////////////////////////////
//
// Attraction Dialog
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

#include "wxdlgAttract.h"
#include "stall.h"

#include "valsimplelist.h"
#include "valext.h"

extern std::vector <AttractionStr *> Attractions;
extern std::vector<cText> cTextStrings;
extern std::vector <Scenery *> SceneryItems;
extern bool save;
extern int CurrentAttraction;

dlgAttract::dlgAttract(wxWindow *parent) {
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
    m_Name->SetValidator(wxExtendedValidator(&Name, false));
    m_Type->SetValidator(wxGenericValidator((int *)&AttractionType));
    m_NameString->SetValidator(wxSimpleListValidator(&NameString));
    m_Description->SetValidator(wxSimpleListValidator(&DescriptionString));
    m_Unk2->SetValidator(wxGenericValidator((int *)&Unk2));
    m_Unk3->SetValidator(wxGenericValidator((int *)&Unk3));
    m_SceneryItem->SetValidator(wxSimpleListValidator(&SID));
    m_OK->SetId(wxID_OK);
    m_Cancel->SetId(wxID_CANCEL);
}
