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

#include "injector_main.h"

#include "injector_app.h"

#include <wx/filename.h>

#include "wxsha.h"

/*
 * gui\FrontScreen\ScenarioLogos\Base\FrontScreen.common.ovl
 * 1a7866fb54c576b8906fda2242f0407a50aee85516570438cffc446666ffdf0d
 *
 * gui\FrontScreen\ScenarioLogos\Base\FrontScreen.unique.ovl
 * bf74db02548e4d3696a77a8e2d8d713acd9dd9707dfebd124654f53a73d0781d
 *
 * WildAnimals\Items\Items.common.ovl
 * 023577f30516cea3a3cc857087f400545fb533ec9156839c5199920f65f9229f
 *
 * WildAnimals\Items\Items.unique.ovl
 * 64f09227190589381aff702976c3b3cc144b8634293fc3ae1a108e5d981ec4e9
 * 
 */ 

#define RCT3_FRONTSCREEN_C_FILE		"gui/FrontScreen/ScenarioLogos/Base/FrontScreen.common.ovl"
#define RCT3_FRONTSCREEN_C_FILE_TO	"gui/FrontScreen/ScenarioLogos/Base/OriginalFrontScreen.common.ovl"
#define RCT3_FRONTSCREEN_C_RES		"ovls/base/Rename-this-to-FrontScreen-and-original-to-OriginalFrontScreen.common.ovl"
#define RCT3_FRONTSCREEN_C_SHA_1	"1a7866fb54c576b8906fda2242f0407a50aee85516570438cffc446666ffdf0d"
#define RCT3_FRONTSCREEN_C_RES_SHA	"aa3b819ae9adff3f70837d0749e4e5dd1e4e80153313a31366c42210899f6f90"

#define RCT3_FRONTSCREEN_U_FILE		"gui/FrontScreen/ScenarioLogos/Base/FrontScreen.unique.ovl"
#define RCT3_FRONTSCREEN_U_FILE_TO	"gui/FrontScreen/ScenarioLogos/Base/OriginalFrontScreen.unique.ovl"
#define RCT3_FRONTSCREEN_U_RES		"ovls/base/Rename-this-to-FrontScreen-and-original-to-OriginalFrontScreen.unique.ovl"
#define RCT3_FRONTSCREEN_U_SHA_1	"bf74db02548e4d3696a77a8e2d8d713acd9dd9707dfebd124654f53a73d0781d"
#define RCT3_FRONTSCREEN_U_RES_SHA	"4aaf5e334936006e61440934e74b695fb8d3eaf0f43113072103d02560184f20"

#define RCT3_MAIN_PROXY_DIR			"gui/Custom/"
#define RCT3_MAIN_PROXY_FILE		"MainInjectionProxy"

#define RCT3_WILDITEMS_C_FILE		"WildAnimals/Items/Items.common.ovl"
#define RCT3_WILDITEMS_C_SHA_1		"023577f30516cea3a3cc857087f400545fb533ec9156839c5199920f65f9229f"

#define RCT3_WILDITEMS_U_FILE		"WildAnimals/Items/Items.unique.ovl"
#define RCT3_WILDITEMS_U_SHA_1		"64f09227190589381aff702976c3b3cc144b8634293fc3ae1a108e5d981ec4e9"

#define RCT3_WILD_PROXY_DIR			"WildAnimals/Items/Custom/"
#define RCT3_WILD_PROXY_FILE		"WildInjectionProxy"
#define RCT3_WILD_REF				"Custom\\" RCT3_WILD_PROXY_FILE

bool CanBeWrittenTo(const wxFileName& target) {
    if ((!target.IsDir()) && target.FileExists()) {
        return target.IsFileWritable();
    }
    if (target.DirExists()) {
        return target.IsDirWritable();
    }

    wxFileName test = target;
    test.RemoveLastDir();
    while (test.GetDirCount()) {
        if (test.DirExists()) {
            return test.IsDirWritable();
        }
        test.RemoveLastDir();
    }
    return false;
}

bool EnsureDir(wxFileName& target) {
    if (target.DirExists()) {
        return true;
    }
    return target.Mkdir(0777, wxPATH_MKDIR_FULL);
}

frmMain::frmMain( wxWindow* parent ):rcfrmMain( parent ), m_installed(false), m_initialinstall(false) {
	wxSHA sha;
	wxFileName f;
	f = wxFileName::DirName(wxGetApp().getInstallDir()).GetPathWithSep()+RCT3_FRONTSCREEN_C_FILE;
	
	if (!f.FileExists()) {
		wxMessageBox(_(
			"Original file for Level 1 not found. This is usually the case if you do not have any Expansions and/or"
			"don't have the latest patch. Please install the latest patch for RCT3."), _("Error"), wxICON_ERROR|wxOK);
		Close();
		return;
	}
	
	sha.Input(f);
	
	if (sha.getDigestString() == RCT3_FRONTSCREEN_C_RES_SHA) {
		m_textReport->AppendText("Injection sytem appears to be installed.\n");
		m_installed = true;
	} else {
		m_textReport->AppendText("Injection sytem not installed.\n\n");
	
		m_textReport->AppendText("FrontScreen.common.ovl\n");
		if (sha.getDigestString() == RCT3_FRONTSCREEN_C_SHA_1)
			m_textReport->AppendText("Match");
		else
			m_textReport->AppendText(sha.getDigestString());
		m_textReport->AppendText("\n");
		
		m_textReport->AppendText("FrontScreen.unique.ovl\n");
		f = wxFileName::DirName(wxGetApp().getInstallDir()).GetPathWithSep()+RCT3_FRONTSCREEN_U_FILE;
		sha.Input(f);
		if (sha.getDigestString() == RCT3_FRONTSCREEN_U_SHA_1)
			m_textReport->AppendText("Match");
		else
			m_textReport->AppendText(sha.getDigestString());
		m_textReport->AppendText("\n");
		
		m_textReport->AppendText("Items.common.ovl\n");
		f = wxFileName::DirName(wxGetApp().getInstallDir()).GetPathWithSep()+RCT3_WILDITEMS_C_FILE;
		sha.Input(f);
		if (sha.getDigestString() == RCT3_WILDITEMS_C_SHA_1)
			m_textReport->AppendText("Match");
		else
			m_textReport->AppendText(sha.getDigestString());
		m_textReport->AppendText("\n");
		
		m_textReport->AppendText("Items.unique.ovl\n");
		f = wxFileName::DirName(wxGetApp().getInstallDir()).GetPathWithSep()+RCT3_WILDITEMS_U_FILE;
		sha.Input(f);
		if (sha.getDigestString() == RCT3_WILDITEMS_U_SHA_1)
			m_textReport->AppendText("Match");
		else
			m_textReport->AppendText(sha.getDigestString());
		m_textReport->AppendText("\n");
		
		if (
			(!wxFileName::FileExists(wxFileName::DirName(wxGetApp().getInstallDir()).GetPathWithSep()+RCT3_MAIN_PROXY_DIR RCT3_MAIN_PROXY_FILE ".common.ovl")) &&
			(!wxFileName::FileExists(wxGetApp().getAppDir().GetPathWithSep()+"backup/" RCT3_FRONTSCREEN_C_FILE))
			)
			m_initialinstall = true;
		
	}
	UpdateButtons();
	Center();
}

void frmMain::OnExitClick(wxCommandEvent& event) {
	Close();
}

void frmMain::DoInstall() {
	// establish the status quo
	enum FILESTATUS {
		ABSENT,
		ORIGINAL,
		PROXY,
		PATCHED,
		MATCH,
		UNKNOWN
	};
	wxFileName f, s;
	wxSHA sha;
	wxString shadig;
	FILESTATUS fs_common = UNKNOWN;
	FILESTATUS fs_unique = UNKNOWN;
	FILESTATUS fs_common_ref = UNKNOWN;
	FILESTATUS fs_unique_ref = UNKNOWN;
	FILESTATUS fs_common_bak = UNKNOWN;
	FILESTATUS fs_unique_bak = UNKNOWN;
	FILESTATUS wild_common = UNKNOWN;
	FILESTATUS wild_unique = UNKNOWN;
	FILESTATUS wild_common_bak = UNKNOWN;
	FILESTATUS wild_unique_bak = UNKNOWN;
	
	// Check backup
	f = wxGetApp().getAppDir().GetPathWithSep()+"backup/" RCT3_FRONTSCREEN_C_FILE;
	s = wxGetApp().getAppDir().GetPathWithSep()+"backup/" RCT3_FRONTSCREEN_C_FILE ".sha";
	if (f.FileExists()) {
		sha.Input(f);
		shadig = sha.getDigestString();
		if (shadig == RCT3_FRONTSCREEN_C_SHA_1) {
			fs_common_bak = ORIGINAL;
		} else {
			
		}
	} else {
		fs_common_bak = ABSENT;
	}
}

void frmMain::DoReexamine() {
}

void frmMain::DoUninstall() {
}

void frmMain::UpdateButtons() {
	m_btReexamine->Enable(m_installed);
	m_btUninstall->Enable(m_installed);
}

