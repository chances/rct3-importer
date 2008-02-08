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

#ifndef _MANAGERAPP_H_
#define _MANAGERAPP_H_

#include <wx/app.h>
#include <wx/filename.h>
#include <wx/snglinst.h>
#include <wx/wxsqlite3.h>

#include "cXmlInitHandler.h"
#include "wxXmlInputCallbackFileSystem.h"

class ManagerApp : public wxApp, public xmlcpp::cXmlInitHandler {
private:
    wxSingleInstanceChecker* m_checker;
    wxFileName m_appdir;
    wxFileName m_installdir;
    bool m_soaked;
    bool m_wild;
    wxSQLite3Database m_managerdb;
    wxSQLite3Database m_filesdb;

    bool TestInstallDir();
    bool CheckInstallDir();

    bool CheckManagerDB();
    bool CheckFilesDB();
public:
    virtual bool OnInit();
    virtual int OnRun();
    virtual int OnExit();

    bool ScanRCT3Directory();

    inline const wxFileName& GetAppDir() const              { return m_appdir; }
    inline const wxFileName& GetInstallDir() const          { return m_installdir; }
    inline const wxSQLite3Database& GetManagerDB() const    { return m_managerdb; }
    inline wxSQLite3Database& GetManagerDB()                { return m_managerdb; }
    inline const wxSQLite3Database& GetFilesDB() const      { return m_filesdb; }
    inline wxSQLite3Database& GetFilesDB()                  { return m_filesdb; }

    inline const bool HasSoaked() const                     { return m_soaked; }
    inline const bool HasWild() const                       { return m_wild; }
};

DECLARE_APP(ManagerApp)

#endif
