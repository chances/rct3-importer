///////////////////////////////////////////////////////////////////////////////
//
// RCT3 Importer TNG
// Next generation RCT3 Importer
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


#include "wx_pch.h"

#include "importersave.h"

#include <wx/artprov.h>

#include "gxpict.h"

BEGIN_EVENT_TABLE(dlgSave,rcdlgSave)
EVT_BUTTON(wxID_SAVE, dlgSave::OnButton)
EVT_BUTTON(wxID_NO, dlgSave::OnButton)
END_EVENT_TABLE()

dlgSave::dlgSave(wxString message, bool cancancel, wxString title, wxWindow *parent):
        rcdlgSave(parent, wxID_ANY, title) {

    m_textText->SetLabel(message);

    m_sdbNo->SetLabel(_("Do&n't Save"));
    m_sdbCancel->Enable(cancancel);
    m_imgIcon->SetBitmap(wxArtProvider::GetBitmap(wxART_QUESTION, wxART_MESSAGE_BOX));
    m_imgIcon->SetAlignment(wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL);

    Fit();
    Layout();
    GetSizer()->SetSizeHints(this);
    Center();
}

void dlgSave::OnButton(wxCommandEvent& event) {
    EndModal(event.GetId());
}
