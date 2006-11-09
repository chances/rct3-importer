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

#ifndef WXDLGSTALLMAN_H_INCLUDED
#define WXDLGSTALLMAN_H_INCLUDED

#include "wx_pch.h"

#include <wx/xrc/xmlres.h>

#include "RCT3Structs.h"
class dlgStallMan : public wxDialog {
public:
    dlgStallMan(wxWindow *parent=NULL);

protected:
    wxListBox* m_StallList;
    wxButton* m_Add;
    wxButton* m_Edit;
    wxButton* m_Delete;
    wxButton* m_Close;

    void OnAdd(wxCommandEvent& event);
    void OnEdit(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnClose(wxCommandEvent& event);

private:
    void InitWidgetsFromXRC(wxWindow *parent) {
        wxXmlResource::Get()->LoadObject(this,parent,_T("dlgStallMan"), _T("wxDialog"));
        m_StallList = XRCCTRL(*this,"m_StallList",wxListBox);
        m_Add = XRCCTRL(*this,"m_Add",wxButton);
        m_Edit = XRCCTRL(*this,"m_Edit",wxButton);
        m_Delete = XRCCTRL(*this,"m_Delete",wxButton);
        m_Close = XRCCTRL(*this,"m_Close",wxButton);
    }

    DECLARE_EVENT_TABLE()

};

#endif
