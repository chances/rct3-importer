///////////////////////////////////////////////////////////////////////////////
//
// Check for save Dialog
// Copyright (C) 2007 Tobias Minch, Jonathan Wilson
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
// Note: ressource in xrc_misc
//
///////////////////////////////////////////////////////////////////////////////

#ifndef WXDLGSAVE_H_INCLUDED
#define WXDLGSAVE_H_INCLUDED

#include "wx_pch.h"

//#include <wx/xrc/xmlres.h>

#include "xrc\res_misc.h"

class dlgSave : public rcdlgSave {
protected:
/*
    wxStaticText* m_textText;
    wxButton* m_SAVE;
    wxButton* m_NO;
    wxButton* m_CANCEL;
*/
    void OnButton(wxCommandEvent& event);

//    DECLARE_EVENT_TABLE()
private:
/*
    void InitWidgetsFromXRC(wxWindow *parent){
        wxXmlResource::Get()->LoadObject(this,parent,_T("dlgSave"), _T("wxDialog"));
        m_textText = XRCCTRL(*this,"m_textText",wxStaticText);
        m_SAVE = XRCCTRL(*this,"wxID_SAVE",wxButton);
        m_NO = XRCCTRL(*this,"wxID_NO",wxButton);
        m_CANCEL = XRCCTRL(*this,"wxID_CANCEL",wxButton);
    }
*/
public:
    dlgSave(wxString message, wxString title = _("Unsaved Changes"), wxWindow *parent=NULL);
};

#endif
