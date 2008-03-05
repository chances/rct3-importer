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

#ifndef _IMPORTERAPP_H_
#define _IMPORTERAPP_H_

#include <wx/app.h>
#include <wx/filename.h>

#include "cXmlInitHandler.h"
#include "wxXmlInputCallbackFileSystem.h"
#include "importermain.h"

class ImporterApp : public wxApp, public xmlcpp::cXmlInitHandler {
private:
    wxFileName m_appdir;
    wxString m_title;

public:
    virtual bool OnInit();
    virtual int OnRun();
    virtual int OnExit();

    inline const wxFileName& getAppDir() const              { return m_appdir; }
    inline const wxString& getTitle() const                 { return m_title; }
};

DECLARE_APP(ImporterApp)

#endif
