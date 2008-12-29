///////////////////////////////////////////////////////////////////////////////
//
// RCT3 Injector
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


#include "injector_app.h"

#include <wx/cmdline.h>
#include <wx/fs_arc.h>
#include <wx/fs_filter.h>
#include <wx/fs_mem.h>
#include <wx/image.h>
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>

#ifdef _WIN32
#include <wx/msw/registry.h>
#endif

#include "injector_main.h"

#include "version_wrap.h"

IMPLEMENT_APP(InjectorApp);

bool InjectorApp::OnInit() {
	::wxInitAllImageHandlers();
    //wxXmlResource::Get()->InitAllHandlers();
    wxFileSystem::AddHandler(new wxArchiveFSHandler);
    wxFileSystem::AddHandler(new wxFilterFSHandler);
    wxFileSystem::AddHandler(new wxMemoryFSHandler);

    m_appdir = wxStandardPaths::Get().GetExecutablePath();
    if (!m_appdir.IsAbsolute())
        m_appdir.MakeAbsolute();

    m_title = GetAppVersion();
	
	m_installdir = "";
	if (!wxApp::OnInit())
		return false;
		
#ifdef _WIN32
	if (m_installdir.IsEmpty()) {
		wxRegKey key("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{907B4640-266B-4A21-92FB-CD1A86CD0F63}");
		if (key.Exists())
			key.QueryValue("InstallLocation", m_installdir);
	}
#endif

	if (m_installdir.IsEmpty() || (!CheckInstallDir())) {
		::wxMessageBox("Could not determine RCT3 installation folder. Please give the correct folder with the command line option '--installdir'.", "Error",wxICON_ERROR|wxOK);
		return false;
	}

    frmMain* main = new frmMain(NULL);
    main->Show(true);
    SetTopWindow(main);

    return true;

}

int InjectorApp::OnRun() {
    #ifdef __WXDEBUG__
    wxLogWindow log(NULL, wxT("Log"));
    #endif

    return wxApp::OnRun();
}

int InjectorApp::OnExit() {
    return 0;
}

bool InjectorApp::CheckInstallDir() {
	wxFileName cf(m_installdir, "Main.common", "ovl");
	return cf.FileExists();
}

bool InjectorApp::OnCmdLineError(wxCmdLineParser& parser) {
	//::wxMessageBox(parser.Usage(), "Command Line Error",wxICON_ERROR|wxOK);
	parser.Usage();
	return false;
}

bool InjectorApp::OnCmdLineHelp(wxCmdLineParser& parser) {
	//::wxMessageBox(parser.Usage(), "Command Line Options",wxICON_INFORMATION|wxOK);
	parser.Usage();
	return false;
}

bool InjectorApp::OnCmdLineParsed(wxCmdLineParser& parser) {
	parser.Found("installdir", &m_installdir);
	return true;
}

void InjectorApp::OnInitCmdLine(wxCmdLineParser& parser) {
	parser.SetSwitchChars(wxT("-"));
	parser.AddSwitch("h", "help", "show this help message", wxCMD_LINE_OPTION_HELP);
	parser.AddOption("", "installdir", "name RCT3 installation directory");
}

