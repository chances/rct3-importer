///////////////////////////////////////////////////////////////////////////////
//
// Config file implemetation
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

#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include "win_predefine.h"

#include "3DLoaderTypes.h"

#include <string>

#define READ_APP_WORKDIR()     wxcRead<wxString>(wxT("App/WorkDirectory"), g_appdir)
#define WRITE_APP_WORKDIR(v)   wxcWrite<wxString>(wxT("App/WorkDirectory"), v)

//#define READ_APP_MATRIX()     wxcRead<r3::MATRIX>(wxT("App/Matrix"), matrixGetUnity())
//#define WRITE_APP_MATRIX(v)   wxcWrite<r3::MATRIX>(wxT("App/Matrix"), v)

#define READ_APP_PREFIX()     wxcRead<wxString>(wxT("App/Prefix"), wxT(""))
#define WRITE_APP_PREFIX(v)   wxcWrite<wxString>(wxT("App/Prefix"), v)

#define READ_APP_TOOLTIPS()     wxcRead<bool>(wxT("App/Tooltips"), true)
#define WRITE_APP_TOOLTIPS(v)   wxcWrite<bool>(wxT("App/Tooltips"), v)

#define READ_APP_SIZE(w, d)     wxcRead<wxSize>(wxString(wxT("App/Size/"))+wxT(w), d)
#define WRITE_APP_SIZE(w, v)   wxcWrite<wxSize>(wxString(wxT("App/Size/"))+wxT(w), v)

/*
#define RCT3CONFIG_VERSION      4

class RCT3Config {
public:
    wxString m_configfile;
    wxString m_prefix;
    D3DMATRIX m_def_matrix;
    wxString m_workdir;
    c3DLoaderOrientation m_def_orient;
    bool m_tooltips;

    RCT3Config();
    RCT3Config(wxString appdir);

    void Load();
    bool Save();
};
*/
#endif // CONFIG_H_INCLUDED
