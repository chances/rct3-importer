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

#ifndef MANAGERCONFIG_H_INCLUDED
#define MANAGERCONFIG_H_INCLUDED

#include "confhelp.h"

#define READ_APP_SIZE(w, d)            wxcRead<wxSize>(wxString(wxT("App/Size/"))+wxT(w), d)
#define WRITE_APP_SIZE(w, v)          wxcWrite<wxSize>(wxString(wxT("App/Size/"))+wxT(w), v)

#define READ_APP_INSTALLDIR()          wxcRead<wxString>(wxT("App/InstallDir"), wxT(""))
#define WRITE_APP_INSTALLDIR(v)       wxcWrite<wxString>(wxT("App/InstallDir"), v)

#define READ_APP_VIEW()                wxcRead<wxString>(wxT("App/View"), wxT(""))
#define WRITE_APP_VIEW(v)             wxcWrite<wxString>(wxT("App/View"), v)

#define READ_APP_VIEW_PACK_SHORT()     wxcRead<bool>(wxT("App/Package/Short"), false)
#define WRITE_APP_VIEW_PACK_SHORT(v)  wxcWrite<bool>(wxT("App/Package/Short"), v)
#define READ_APP_VIEW_AUTH_SHORT()     wxcRead<bool>(wxT("App/Author/Short"), true)
#define WRITE_APP_VIEW_AUTH_SHORT(v)  wxcWrite<bool>(wxT("App/Author/Short"), v)
#define READ_APP_VIEW_AUTH_OPEN()      wxcRead<bool>(wxT("App/Author/Open"), false)
#define WRITE_APP_VIEW_AUTH_OPEN(v)   wxcWrite<bool>(wxT("App/Author/Open"), v)

#endif // MANAGERCONFIG_H_INCLUDED
