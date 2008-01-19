///////////////////////////////////////////////////////////////////////////////
//
// RCT3 Customization Manager
// Installer for RCT3 customizations
// Copyright (C) 2008 Tobias Minch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, <http://www.gnu.org/licenses/>.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////

#include "managerabout.h"

#include <wx/xrc/xmlres.h>

#include "managerapp.h"
#include "managerresourcefiles.h"
#include "wxsha1.h"

dlgAbout::dlgAbout(wxWindow* parent): rcdlgAbout(parent) {
    m_htmlAbout->LoadPage(MEMORY_PREFIX RES_HTML_ABOUT);
    m_htmlGPL->LoadPage(MEMORY_PREFIX RES_HTML_GPL3);

    if (::wxGetApp().HasSoaked() && ::wxGetApp().HasWild()) {
        m_textExpansion->SetLabel(wxT("Soaked! && Wild!"));
    } else if (::wxGetApp().HasSoaked()) {
        m_textExpansion->SetLabel(wxT("Soaked!"));
    } else if (::wxGetApp().HasWild()) {
        m_textExpansion->SetLabel(wxT("Wild!"));
    } else {
        m_textExpansion->SetLabel(_("None"));
    }

    wxSQLite3ResultSet fs = ::wxGetApp().GetFilesDB().ExecuteQuery(wxT("select name from vanilla;"));
    while (fs.NextRow()) {
        m_lbFiles->Append(fs.GetString(0));
    }

    m_pgFile->SetSplitterPosition(60);

    m_pgFile->DisableProperty(m_pgFile->Append(wxStringProperty(wxT("Path"), wxPG_LABEL, wxT(""))));
    m_pgFile->DisableProperty(m_pgFile->Append(wxStringProperty(wxT("Name"), wxPG_LABEL, wxT(""))));
    m_pgFile->DisableProperty(m_pgFile->Append(wxStringProperty(wxT("Size"), wxPG_LABEL, wxT(""))));
    m_pgFile->DisableProperty(m_pgFile->Append(wxStringProperty(wxT("File Time"), wxPG_LABEL, wxT(""))));
    m_pgFile->DisableProperty(m_pgFile->Append(wxStringProperty(wxT("SHA1 Hash"), wxPG_LABEL, wxT(""))));

}

void dlgAbout::OnFilesListBox( wxCommandEvent& WXUNUSED(event) ) {
    wxString sel = m_lbFiles->GetStringSelection();
    wxFileName selfn = sel;
    m_pgFile->SetPropertyValue(wxT("Path"), selfn.GetPath());
    m_pgFile->SetPropertyValue(wxT("Name"), selfn.GetFullName());

    selfn.MakeAbsolute(::wxGetApp().GetInstallDir().GetPath(wxPATH_GET_SEPARATOR|wxPATH_GET_VOLUME));

    wxSQLite3StatementBuffer buf;
    buf.Format("select sha1, size, time from vanilla where name = '%q';", sel.mb_str(wxConvUTF8).data());
    wxSQLite3ResultSet fs = ::wxGetApp().GetFilesDB().ExecuteQuery(buf);
    wxULongLong s;
    s = fs.GetInt64(1);
    m_pgFile->SetPropertyValue(wxT("Size"), wxFileName::GetHumanReadableSize(s));
    if (selfn.GetSize() != s) {
        m_pgFile->SetPropertyBackgroundColour(wxT("Size"), wxColour(0xff, 0x33, 0x00));
    } else {
        m_pgFile->SetPropertyBackgroundColour(wxT("Size"), wxColour(0xcc, 0xff, 0x99));
    }
    time_t ti = fs.GetInt(2);
    wxDateTime t(ti);
    m_pgFile->SetPropertyValue(wxT("File Time"), t.Format(wxT("%d.%m.%Y %H:%M:%S")));
    if (selfn.GetModificationTime().GetTicks() != ti) {
        m_pgFile->SetPropertyBackgroundColour(wxT("File Time"), wxColour(0xff, 0x33, 0x00));
    } else {
        m_pgFile->SetPropertyBackgroundColour(wxT("File Time"), wxColour(0xcc, 0xff, 0x99));
    }
    m_pgFile->SetPropertyValue(wxT("SHA1 Hash"), fs.GetString(0));
    {
        wxSHA1 sha;
        sha.Input(selfn);
        if (sha.Result() != fs.GetString(0)) {
            m_pgFile->SetPropertyBackgroundColour(wxT("SHA1 Hash"), wxColour(0xff, 0x33, 0x00));
        } else {
            m_pgFile->SetPropertyBackgroundColour(wxT("SHA1 Hash"), wxColour(0xcc, 0xff, 0x99));
        }
    }
}

