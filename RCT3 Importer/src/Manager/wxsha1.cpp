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

#include "wxsha1.h"

#include <wx/file.h>

#include "counted_array_ptr.h"

wxString wxSHA1::Result() {
    unsigned sh[5];
    wxString res;
    if (Result(sh)) {
        res = wxString::Format(wxT("%08X%08X%08X%08X%08X"), sh[0], sh[1], sh[2], sh[3], sh[4]);
    }
    return res;
}

wxFileOffset wxSHA1::Input(const wxString& file) {
    wxFileOffset res = 0;
    wxFile f(file, wxFile::read);
    res = f.Length();

    if (res) {
        counted_array_ptr<unsigned char> buf(new unsigned char[res]);
        f.Read(buf.get(), res);
        Input(buf.get(), res);
    }

    return res;
}
