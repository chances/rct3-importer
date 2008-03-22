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

#include "wx_pch.h"
#include "validatorapp.h"

#include <wx/dir.h>
#include <wx/fileconf.h>
#include <wx/fs_arc.h>
#include <wx/fs_filter.h>
#include <wx/fs_mem.h>
#include <wx/link.h>
//#include <wx/xrc/xmlres.h>
//#include <wx/xrc/xh_all.h>

#include "bzipstream.h"
#include "cXmlDoc.h"
#include "cXmlNode.h"
#include "wxXmlInputCallbackFileSystem.h"

#include "validatormain.h"

#include "version.h"

IMPLEMENT_APP(ValidatorApp);

wxFORCE_LINK_MODULE(bzipstream)

bool ValidatorApp::OnInit() {
	::wxInitAllImageHandlers();
    //wxXmlResource::Get()->InitAllHandlers();
    wxFileSystem::AddHandler(new wxArchiveFSHandler);
    wxFileSystem::AddHandler(new wxFilterFSHandler);
    wxFileSystem::AddHandler(new wxMemoryFSHandler);

    wxBZipClassFactory cf;

    xmlcpp::wxXmlInputCallbackFileSystem::Init();

    m_appdir = wxFileName(argv[0]).GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME);
    m_title = wxString::Format(_("%s v%ld.%ld%s (Build %ld)"),
        wxT("Schema Validator"), AutoVersion::MAJOR, AutoVersion::MINOR,
        wxString(AutoVersion::STATUS_SHORT, wxConvLocal).c_str(), AutoVersion::BUILD);

    frmMain* main = new frmMain(NULL);
    main->Show(true);
    SetTopWindow(main);

    return true;

}

int ValidatorApp::OnRun() {
    #ifdef __WXDEBUG__
    wxLogWindow log(NULL, wxT("Log"));
    #endif

    return wxApp::OnRun();
}

int ValidatorApp::OnExit() {
    return 0;
}

