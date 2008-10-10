///////////////////////////////////////////////////////////////////////////////
//
// Info dialog
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
///////////////////////////////////////////////////////////////////////////////

#ifndef WXDLGINFO_H_INCLUDED
#define WXDLGINFO_H_INCLUDED

#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/xrc/xmlres.h>

class dlgInfo : public wxDialog {
protected:
    wxTextCtrl* m_textInfo;
    wxButton* m_btOk;

private:
    void InitWidgetsFromXRC(wxWindow *parent) {
        wxXmlResource::Get()->LoadObject(this,parent,_T("dlgInfo"), _T("wxDialog"));
        m_textInfo = XRCCTRL(*this,"m_textInfo",wxTextCtrl);
        m_btOk = XRCCTRL(*this,"m_btOk",wxButton);
    }
public:
    dlgInfo(wxWindow *parent=NULL);
    void SetText(const wxString& val) {m_textInfo->SetValue(val);};
};

#endif // WXDLGINFO_H_INCLUDED
