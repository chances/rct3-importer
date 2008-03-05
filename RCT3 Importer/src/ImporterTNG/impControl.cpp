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

#include "impControl.h"

#include "impView.h"

impControl::impControl(): m_valid(new impValidator()) {
    m_valid->setControl(this);
}

impControl::~impControl() {
    //dtor
}

void impControl::init(impView& view, const wxString& path, const wxString& parentpath) {
    setPath(path);
    m_parentpath = parentpath;

    //getXPath.connect(view.getXPath.make_slot());
    attachConnection(&view);
    changeValue.connect(view.changeValue.make_slot());

    view.update_controls.connect(sigc::mem_fun(*this, &impControl::update));
}

wxString impControl::value() const {
    wxLogError(wxT("PANIC! value called for non-value control."));
    return wxT("");
}

void impControl::change() {
    impValidator::STATUS status = valid();
    switch (status) {
    case impValidator::STATUS_ERROR:
        wxLogStatus(_("Error: ") + validator().reason());
        break;
    case impValidator::STATUS_WARNING:
        wxLogStatus(_("Warning: ") + validator().reason());
        changeValue(m_path, value());
        break;
    default:
        wxLogStatus(wxT(""));
        changeValue(m_path, value());
    }
}

