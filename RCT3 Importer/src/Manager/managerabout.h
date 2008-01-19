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

#ifndef MANAGERABOUT_H_INCLUDED
#define MANAGERABOUT_H_INCLUDED

#include "xrc\xrc_about.h"

class dlgAbout: public rcdlgAbout {
protected:
    virtual void OnFilesListBox( wxCommandEvent& event );
public:
    dlgAbout(wxWindow* parent);
};

#endif // MANAGERABOUT_H_INCLUDED
