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

#ifndef WXSHA1_H_INCLUDED
#define WXSHA1_H_INCLUDED

#include "sha1.h"
#include <wx/filename.h>
#include <wx/string.h>

class wxSHA1: public SHA1 {
public:
    using SHA1::Result;
    using SHA1::Input;

    wxSHA1(): SHA1() {};

    wxString Result();
    wxFileOffset Input(const wxString& file);
    wxFileOffset Input(const wxFileName& file) {
        return Input(file.GetFullPath());
    }
};

#endif // WXSHA1_H_INCLUDED
