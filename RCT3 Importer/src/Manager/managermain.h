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

#ifndef MANAGERMAIN_H_INCLUDED
#define MANAGERMAIN_H_INCLUDED

#include "wx_pch.h"
#include "xrc\xrc_main.h"

#include <wx/aui/aui.h>
//#include <ifm/ifm.h>

#include "packagelistbox.h"

class frmMain: public rcfrmMain {
private:
    wxAuiManager m_mgr;
    wxPackageListBox* m_lbPackages;
//    std::auto_ptr<ifm::InterfaceManager> m_mgr;
//    enum {
//        PAN_MAIN = 10,
//        PAN_INFO,
//        PAN_OPTIONS
//    };
public:
    frmMain(wxWindow* parent);
    virtual ~frmMain();

    virtual void OnMenuExit( wxCommandEvent& event );
    virtual void OnMenuAbout( wxCommandEvent& event );
    virtual void OnMenuSettingsRescan( wxCommandEvent& event );

    virtual void OnMenuOpen( wxMenuEvent& event );

    virtual void OnMenuViewPackageInfo( wxCommandEvent& event );
    virtual void OnMenuViewPackageOptions( wxCommandEvent& event );
    virtual void OnMenuChangeView( wxCommandEvent& event );
};

#endif // MANAGERMAIN_H_INCLUDED
