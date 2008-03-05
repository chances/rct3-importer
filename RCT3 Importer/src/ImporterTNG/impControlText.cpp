///////////////////////////////////////////////////////////////////////////////
//
// RCT3 Importer TNG
// Custom object importer for Ataris Roller Coaster Tycoon 3
// Copyright (C) 2008 Belgabor (Tobias Minich)
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
//   Belgabor (Tobias Minich) - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////

#include "impControlText.h"

using namespace xmlcpp;

impControlText::impControlText(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name):
        wxTextCtrl(parent, id, value, pos, size, style, validator, name) {
	this->Connect( GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( impControlText::OnTextChange ) );
}

impControlText::~impControlText() {
	this->Disconnect( GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( impControlText::OnTextChange ) );
}

void impControlText::update(const wxString& value) {
    cXmlXPath p = getXPath();
    if (p) {
        cXmlXPathResult res = p(m_path.utf8_str());
        if (res.size()) {
            if ((value == wxT("/")) || (value == res[0].wxpath()))
                ChangeValue(res[0].wxcontent());
        } else {
            ChangeValue(wxT(""));
        }
    } else {
        wxLogError(wxT("PANIC! XPath slot unconnected"));
    }
}

wxString impControlText::value() const {
    return GetValue();
}

void impControlText::OnTextChange(wxCommandEvent& event) {
    change();
}

