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

#ifndef WXDLGSTALL_H_INCLUDED
#define WXDLGSTALL_H_INCLUDED

#include <wx/button.h>
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/xrc/xmlres.h>

#include "RCT3Structs.h"
#include "wxInputBox.h"

class dlgStall : public wxDialog {
protected:
    wxTextCtrl* m_Name;
    wxChoice* m_Type;
    wxChoice* m_NameString;
    wxChoice* m_Description;
    wxTextCtrl* m_Unk2;
    wxTextCtrl* m_Unk3;
    wxChoice* m_SceneryItem;
    wxTextCtrl* m_Unk11;
    wxTextCtrl* m_Unk12;
    wxTextCtrl* m_Unk13;
    wxTextCtrl* m_Unk14;
    wxTextCtrl* m_Unk15;
    wxTextCtrl* m_Unk16;
    wxButton* m_btDefault;
    wxButton* m_OK;
    wxButton* m_Cancel;

    wxInputBox* m_ibNameString;
    wxInputBox* m_ibDescription;
    wxInputBox* m_ibSceneryItem;

    void OnDefault(wxCommandEvent& event);
    void OnNameString(wxCommandEvent& event);

private:
    void InitWidgetsFromXRC(wxWindow *parent) {
        wxXmlResource::Get()->LoadObject(this,parent,_T("dlgStall"), _T("wxDialog"));
        m_Name = XRCCTRL(*this,"m_Name",wxTextCtrl);
        m_Type = XRCCTRL(*this,"m_Type",wxChoice);
        m_NameString = XRCCTRL(*this,"m_NameString",wxChoice);
        m_Description = XRCCTRL(*this,"m_Description",wxChoice);
        m_Unk2 = XRCCTRL(*this,"m_Unk2",wxTextCtrl);
        m_Unk3 = XRCCTRL(*this,"m_Unk3",wxTextCtrl);
        m_SceneryItem = XRCCTRL(*this,"m_SceneryItem",wxChoice);
        m_Unk11 = XRCCTRL(*this,"m_Unk11",wxTextCtrl);
        m_Unk12 = XRCCTRL(*this,"m_Unk12",wxTextCtrl);
        m_Unk13 = XRCCTRL(*this,"m_Unk13",wxTextCtrl);
        m_Unk14 = XRCCTRL(*this,"m_Unk14",wxTextCtrl);
        m_Unk15 = XRCCTRL(*this,"m_Unk15",wxTextCtrl);
        m_Unk16 = XRCCTRL(*this,"m_Unk16",wxTextCtrl);
        m_btDefault = XRCCTRL(*this,"m_btDefault",wxButton);
        m_OK = XRCCTRL(*this,"m_OK",wxButton);
        m_Cancel = XRCCTRL(*this,"m_Cancel",wxButton);
    }

    DECLARE_EVENT_TABLE()
public:
    dlgStall(wxWindow *parent);
    wxString Name;
    unsigned long StallType;
    wxString NameString;
    wxString DescriptionString;
    unsigned long Unk2;
    long Unk3;
    wxString SID;
    unsigned long Unk11;
    unsigned long Unk12;
    unsigned long Unk13;
    unsigned long Unk14;
    unsigned long Unk15;
    unsigned long Unk16;

    virtual bool TransferDataToWindow();
};

#endif
