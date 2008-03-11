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
#include "importerapp.h"

#include <wx/dir.h>
#include <wx/fileconf.h>
#include <wx/fs_arc.h>
#include <wx/fs_filter.h>
#include <wx/fs_mem.h>
#include <wx/link.h>
//#include <wx/xrc/xmlres.h>
//#include <wx/xrc/xh_all.h>

#include "bzipstream.h"
#include "counted_array_ptr.h"
#include "cXmlDoc.h"
#include "cXmlNode.h"
#include "wxXmlInputCallbackFileSystem.h"

#include "impRawovlFileManager.h"
#include "impElement.h"
#include "importerconfig.h"
#include "importermain.h"
#include "importerresources.h"

#include "version.h"

IMPLEMENT_APP(ImporterApp);

wxFORCE_LINK_MODULE(bzipstream)

bool ImporterApp::OnInit() {
	::wxInitAllImageHandlers();
    //wxXmlResource::Get()->InitAllHandlers();
    wxFileSystem::AddHandler(new wxArchiveFSHandler);
    wxFileSystem::AddHandler(new wxFilterFSHandler);
    wxFileSystem::AddHandler(new wxMemoryFSHandler);

    wxBZipClassFactory cf;

    xmlcpp::wxXmlInputCallbackFileSystem::Init();

    m_appdir = wxFileName(argv[0]).GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME);
    m_title = wxString::Format(_("%s v%ld.%ld%s (Build %ld)"),
        wxT("RCT3 Importer TNG"), AutoVersion::MAJOR, AutoVersion::MINOR,
        wxString(AutoVersion::STATUS_SHORT, wxConvLocal).c_str(), AutoVersion::BUILD);

    wxFileConfig::Set(new wxFileConfig(wxT("RCT3 Importer TNG"), wxT("Freeware"), m_appdir.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME)+wxT("RCT3 Manager.conf"), wxT(""), wxCONFIG_USE_LOCAL_FILE));

    // *&^$% GraphicsMagick
#ifdef __WXMSW__
    wxString appenv = wxT("MAGICK_CONFIGURE_PATH=") + m_appdir.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME);
    putenv(appenv.mb_str(wxConvLocal));
#endif

    InitElements();
    InitResources();

    impRawovlFileManager::getManager().newProject(true);

    frmMain* main = new frmMain(NULL);
    main->Show(true);
    SetTopWindow(main);

    return true;

}

int ImporterApp::OnRun() {
    #ifdef __WXDEBUG__
    wxLogWindow log(NULL, wxT("Log"));
    #endif

    return wxApp::OnRun();
}

int ImporterApp::OnExit() {
    wxcFlush();
    return 0;
}

