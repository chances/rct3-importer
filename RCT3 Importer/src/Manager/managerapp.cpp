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

#include "wx_pch.h"
#include "managerapp.h"

#include <wx/dir.h>
#include <wx/fileconf.h>
#include <wx/fs_arc.h>
#include <wx/fs_filter.h>
#include <wx/fs_mem.h>
#include <wx/link.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_all.h>

#include "bzipstream.h"
#include "counted_array_ptr.h"
#include "managerconfig.h"
#include "managermain.h"
#include "managerresourcefiles.h"
#include "managerresources.h"
#include "OVLDump.h"
#include "wxsha1.h"
#include "xrc\xrc_progress.h"

IMPLEMENT_APP(ManagerApp);

wxFORCE_LINK_MODULE(bzipstream)

class dlgProgress: public rcdlgProgress {
private:
    wxWindowDisabler m_disable;
    int m_counter;
    wxDateTime m_start;
public:
    dlgProgress(): rcdlgProgress(NULL, wxID_ANY), m_disable(this), m_counter(0), m_start(wxDateTime::Now()) {
        Fit();
        Layout();
        Show(true);
    };
    void Pulse(const wxString& file) {
        if (!(m_counter % 100)) {
            Refresh(true);
            Update();
        }
        m_counter++;
        m_textFiles->SetLabel(wxString::Format(wxT("%d"), m_counter));
        m_textCurrent->SetLabel(file);
        m_textTime->SetLabel(wxDateTime::Now().Subtract(m_start).Format(wxT("%H:%M:%S")));
        ::wxGetApp().Yield();
    }
};

class wxRCT3Filler: public wxDirTraverser {
private:
    wxSQLite3Database* m_db;
    wxSQLite3StatementBuffer m_buf;
    dlgProgress m_pd;
    wxFileName m_root;
    wxSHA1 m_sha;
public:
    wxRCT3Filler(wxSQLite3Database* db, const wxDir& root): m_db(db),
//        m_pd(_("Scanning RCT3 Directory"), wxT("Scanning"), 1000, NULL, wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_ELAPSED_TIME),
        m_pd(),
        m_root(root.GetName(), wxT("")) {};
    virtual wxDirTraverseResult OnFile(const wxString& filename) {
        wxFileName temp = filename;
        wxString shastr = wxT("0");
        unsigned long ovlversion = 0;
        int modtime = temp.GetModificationTime().GetTicks();
        wxFileOffset size = 0;

        if (wxFile::Access(filename.c_str(), wxFile::read)) {
            m_sha.Reset();
            /*
            wxFile f(filename, wxFile::read);
            counted_array_ptr<unsigned char> buf(new unsigned char[f.Length()]);
            size = f.Length();
            f.Read(buf.get(), f.Length());
            m_sha.Input(buf.get(), f.Length());
            */
            size = m_sha.Input(filename);
            shastr = m_sha.Result();

            if (filename.Lower().EndsWith(wxT(".common.ovl"))) {
                cOVLDump dovl;
                dovl.Load(filename.mb_str(wxConvFile));
                ovlversion = dovl.GetVersion();
            }
        }

        temp.MakeRelativeTo(m_root.GetPath(wxPATH_GET_SEPARATOR));
        m_buf.Format("insert into vanilla values ('%q', '%q', %d, %lld, %ld);", temp.GetFullPath().mb_str(wxConvUTF8).data(), shastr.mb_str(wxConvUTF8).data(), modtime, size, ovlversion);
        m_db->ExecuteUpdate(m_buf);
        m_pd.Pulse(temp.GetFullPath());
        return wxDIR_CONTINUE;
    }

    virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname)) {
        return wxDIR_CONTINUE;
    }
};

bool ManagerApp::OnInit() {
    const wxString name = wxString::Format(wxT("RCT3Manager-%s"), wxGetUserId().c_str());
    m_checker = new wxSingleInstanceChecker(name);
    if ( m_checker->IsAnotherRunning() ) {
        wxLogError(_("Another program instance is already running, aborting."));
        return false;
    }

	::wxInitAllImageHandlers();
    //wxXmlResource::Get()->InitAllHandlers();
    wxFileSystem::AddHandler(new wxArchiveFSHandler);
    wxFileSystem::AddHandler(new wxFilterFSHandler);
    wxFileSystem::AddHandler(new wxMemoryFSHandler);

    wxBZipClassFactory cf;

    m_appdir = wxFileName(argv[0]).GetPath(wxPATH_GET_SEPARATOR);

    wxFileConfig::Set(new wxFileConfig(wxT("RCT3 Manager"), wxT("Freeware"), m_appdir.GetPath(wxPATH_GET_SEPARATOR)+wxT("RCT3 Manager.conf"), wxT(""), wxCONFIG_USE_LOCAL_FILE));

    // *&^$% GraphicsMagick
    wxString appenv = wxT("MAGICK_CONFIGURE_PATH=") + m_appdir.GetPath(wxPATH_GET_SEPARATOR);
    putenv(appenv.mb_str(wxConvLocal));

    if (!CheckInstallDir())
        return false;

    InitResources();

    if (!CheckManagerDB())
        return false;
    if (!CheckFilesDB())
        return false;

    frmMain* main = new frmMain(NULL);
    main->Show(true);
    SetTopWindow(main);

    return true;

}

int ManagerApp::OnRun() {
    #ifdef __WXDEBUG__
    wxLogWindow log(NULL, wxT("Log"));
    #endif

    return wxApp::OnRun();
}

int ManagerApp::OnExit() {
    delete m_checker;
    wxcFlush();
    return 0;
}

bool ManagerApp::TestInstallDir() {
    wxFileName test(m_installdir.GetPath(), wxT("Main.common.ovl"));
    if (!test.FileExists()) {
        return false;
    }
    if (!test.IsFileReadable()) {
        wxLogError(_("You chose the correct installation directory, but you and/or this application doesn't have sufficient rights to read files in it."));
        return false;
    }
    if (!test.IsDirWritable()) {
        wxLogError(_("You chose the correct installation directory, but you and/or this application doesn't have sufficient rights to write files into it. The manager will start, but you won't be able to install anything."));
    }
    return true;
}

bool ManagerApp::CheckInstallDir() {
    m_soaked = false;
    m_wild = false;

    if (m_installdir == wxFileName())
        m_installdir = READ_APP_INSTALLDIR();

#ifdef __WXMSW__
    if (m_installdir == wxFileName()) {
        HKEY key;
        wxChar *temp = new wxChar[MAX_PATH+1];
        LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                wxT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{907B4640-266B-4A21-92FB-CD1A86CD0F63}"),
                                0, KEY_QUERY_VALUE, &key);
        unsigned long length = MAX_PATH;
        res = RegQueryValueEx(key, wxT("InstallLocation"), 0, NULL, (LPBYTE) temp, &length);
        if (res == ERROR_SUCCESS)
            m_installdir = wxFileName(wxString(temp), wxT(""));
        delete[] temp;
        RegCloseKey(key);
    }
#endif
    if ((m_installdir == wxFileName()) || (!TestInstallDir())) {
        ::wxMessageBox(_("Could not determine your RCT3 installation directory or the one you set is wrong/not readable.\n"
                         "Please manually select the Main.common.ovl file in your RCT3 installation directory"),
                         _("Error"), wxICON_ERROR);
        wxFileDialog fd(NULL, _("Select Main.common.ovl in your RCT3 installation directory"), wxT(""),
                        wxT("Main.common.ovl"), wxT("Main.common.ovl|Main.common.ovl"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
chooseagain:
        if (fd.ShowModal() == wxID_OK) {
            m_installdir = wxFileName(fd.GetDirectory(), wxT(""));
            if (!TestInstallDir()) {
                ::wxMessageBox(_("The directory you selected is not the RCT3 installation directory or you/this applicatoin doesn't have sufficient rights to read it!"), _("Error"), wxICON_ERROR);
                goto chooseagain;
            }
            WRITE_APP_INSTALLDIR(m_installdir.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
            wxcFlush();
        }
    }

    if (m_installdir == wxFileName())
        return false;

    m_soaked = wxFileName(m_installdir.GetPath(), wxT("Soaked_Readme.txt")).FileExists();
    m_wild = wxFileName(m_installdir.GetPath(), wxT("Wild_Readme.txt")).FileExists();

    return true;
}

#define DB_MANAGER_VERSION_GROUPS 1
#define DB_MANAGER_VERSION_PACKAGES 1
#define DB_MANAGER_VERSION_AUTHORLIST 1
#define DB_MANAGER_VERSION_PACKAGELIST 1
#define DB_MANAGER_VERSION_PACKAGELIST_AUTHORS 1
#define DB_MANAGER_VERSION_PACKAGELIST_WARNINGS 1
bool ManagerApp::CheckManagerDB() {
    int version_groups = DB_MANAGER_VERSION_GROUPS;
    int version_packages = DB_MANAGER_VERSION_PACKAGES;
    int version_authorlist = DB_MANAGER_VERSION_AUTHORLIST;
    int version_packagelist = DB_MANAGER_VERSION_PACKAGELIST;
    int version_packagelist_authors = DB_MANAGER_VERSION_PACKAGELIST_AUTHORS;
    int version_packagelist_warnings = DB_MANAGER_VERSION_PACKAGELIST_WARNINGS;
    wxSQLite3StatementBuffer temp;

    m_managerdb.Open(m_appdir.GetPath(wxPATH_GET_SEPARATOR) + wxT("manager.db"));
    if (!m_managerdb.TableExists(wxT("versions"))) {
        m_managerdb.ExecuteUpdate(wxT("create table versions(tablename char[30], version int);"));
        temp.Format("insert into versions values ('groups', %d);", version_groups);
        m_managerdb.ExecuteUpdate(temp);
        temp.Format("insert into versions values ('packages', %d);", version_packages);
        m_managerdb.ExecuteUpdate(temp);
        temp.Format("insert into versions values ('authorlist', %d);", version_authorlist);
        m_managerdb.ExecuteUpdate(temp);
        temp.Format("insert into versions values ('packagelist', %d);", version_packagelist);
        m_managerdb.ExecuteUpdate(temp);
        temp.Format("insert into versions values ('packagelist_authors', %d);", version_packagelist_authors);
        m_managerdb.ExecuteUpdate(temp);
        temp.Format("insert into versions values ('packagelist_warnings', %d);", version_packagelist_warnings);
        m_managerdb.ExecuteUpdate(temp);
    } else {
        version_groups = m_managerdb.ExecuteScalar(wxT("select version from versions where tablename = 'groups';"));
        version_packages = m_managerdb.ExecuteScalar(wxT("select version from versions where tablename = 'packages';"));
        version_authorlist = m_managerdb.ExecuteScalar(wxT("select version from versions where tablename = 'authorlist';"));
        version_packagelist = m_managerdb.ExecuteScalar(wxT("select version from versions where tablename = 'packagelist';"));
        version_packagelist_authors = m_managerdb.ExecuteScalar(wxT("select version from versions where tablename = 'packagelist_authors';"));
        version_packagelist_warnings = m_managerdb.ExecuteScalar(wxT("select version from versions where tablename = 'packagelist_warnings';"));
    }
    if ((version_groups > DB_MANAGER_VERSION_GROUPS) ||
        (version_packages > DB_MANAGER_VERSION_PACKAGES) ||
        (version_authorlist > DB_MANAGER_VERSION_AUTHORLIST) ||
        (version_packagelist > DB_MANAGER_VERSION_PACKAGELIST) ||
        (version_packagelist_authors > DB_MANAGER_VERSION_PACKAGELIST_AUTHORS) ||
        (version_packagelist_warnings > DB_MANAGER_VERSION_PACKAGELIST_WARNINGS)) {
        wxLogError(_("Manager database modified by a newer version of this application."));
        return false;
    }
    if (!m_managerdb.TableExists(wxT("groups"))) {
        m_managerdb.ExecuteUpdate(wxT("create table groups(name text, guid text);"));
    }
    if (!m_managerdb.TableExists(wxT("packages"))) {
        m_managerdb.ExecuteUpdate(wxT("create table packages(name text, guid text, file text, version text);"));
    }
    if (!m_managerdb.TableExists(wxT("authorlist"))) {
        m_managerdb.ExecuteUpdate(wxT("create table authorlist(name text, guid text, official int);"));
        wxFileSystem fs;

        m_managerdb.Begin();

        std::auto_ptr<wxFSFile> inputfsfile(fs.OpenFile(MEMORY_PREFIX RES_XML_AUTHORS, wxFS_READ));

        wxXmlDocument doc(*inputfsfile->GetStream());

        wxXmlNode* child = doc.GetRoot()->GetChildren();
        while (child) {
            if (child->GetName() == wxT("author")) {
                wxString name = child->GetPropVal(wxT("name"), wxT(""));
                name.Trim();
                name.Trim(true);
                wxString guid = child->GetPropVal(wxT("guid"), wxT(""));
                guid.Trim();
                guid.Trim(true);
                if (name.IsEmpty() || guid.IsEmpty()) {
                    m_managerdb.Rollback();
                    wxLogError(_("Error in author list xml."));
                    return false;
                }
                temp.Format("insert into authorlist values ('%q', '%q', 1);", name.mb_str(wxConvUTF8).data(), guid.mb_str(wxConvUTF8).data());
                m_managerdb.ExecuteUpdate(temp);
            }
            child = child->GetNext();
        }
        m_managerdb.Commit();
    }
    if (!m_managerdb.TableExists(wxT("packagelist"))) {
        m_managerdb.ExecuteUpdate(wxT("create table packagelist(name text, shortname text, guid text, version text, official int);"));
    }
    if (!m_managerdb.TableExists(wxT("packagelist_authors"))) {
        m_managerdb.ExecuteUpdate(wxT("create table packagelist_authors(guid text, authorguid text);"));
    }
    if (!m_managerdb.TableExists(wxT("packagelist_warnings"))) {
        m_managerdb.ExecuteUpdate(wxT("create table packagelist_warnings(guid text, type int, descrition text);"));
    }
    if (!m_managerdb.ExecuteScalar(wxT("select count(*) from packagelist;"))) {
        wxFileSystem fs;

        m_managerdb.Begin();

        std::auto_ptr<wxFSFile> inputfsfile(fs.OpenFile(MEMORY_PREFIX RES_XML_PACKAGES, wxFS_READ));

        wxXmlDocument doc(*inputfsfile->GetStream());

        wxXmlNode* child = doc.GetRoot()->GetChildren();
        while (child) {
            if (child->GetName() == wxT("package")) {
                wxString name = child->GetPropVal(wxT("name"), wxT(""));
                name.Trim();
                name.Trim(true);
                wxString shortname = child->GetPropVal(wxT("shortname"), wxT(""));
                shortname.Trim();
                shortname.Trim(true);
                wxString guid = child->GetPropVal(wxT("guid"), wxT(""));
                guid.Trim();
                guid.Trim(true);
                wxString version = child->GetPropVal(wxT("version"), wxT("0"));
                version.Trim();
                version.Trim(true);
                if (name.IsEmpty() || guid.IsEmpty()) {
                    m_managerdb.Rollback();
                    wxLogError(_("Error in package list xml (name and/or guid missing)."));
                    return false;
                }
                temp.Format("insert into packagelist values ('%q', '%q', '%q', '%q', 1);", name.mb_str(wxConvUTF8).data(),
                            shortname.mb_str(wxConvUTF8).data(), guid.mb_str(wxConvUTF8).data(), version.mb_str(wxConvUTF8).data());
                m_managerdb.ExecuteUpdate(temp);

                bool haveauthor = false;
                if (child->HasProp(wxT("authorguid"))) {
                    wxString authorguid = child->GetPropVal(wxT("authorguid"), wxT(""));
                    authorguid.Trim();
                    authorguid.Trim(true);
                    if (authorguid.IsEmpty()) {
                        m_managerdb.Rollback();
                        wxLogError(_("Error in package list xml (authorguid empty)."));
                        return false;
                    }
                    temp.Format("insert into packagelist_authors values ('%q', '%q');", guid.mb_str(wxConvUTF8).data(), authorguid.mb_str(wxConvUTF8).data());
                    m_managerdb.ExecuteUpdate(temp);
                    haveauthor  = true;
                }

                wxXmlNode* subchild = child->GetChildren();
                while(subchild) {
                    if (subchild->GetName() == wxT("author")) {
                        wxString authorguid = subchild->GetPropVal(wxT("guid"), wxT(""));
                        authorguid.Trim();
                        authorguid.Trim(true);
                        if (authorguid.IsEmpty()) {
                            m_managerdb.Rollback();
                            wxLogError(_("Error in package list xml (author[guid] empty)."));
                            return false;
                        }
                        temp.Format("insert into packagelist_authors values ('%q', '%q');", guid.mb_str(wxConvUTF8).data(), authorguid.mb_str(wxConvUTF8).data());
                        m_managerdb.ExecuteUpdate(temp);
                        haveauthor  = true;
                    } else if (subchild->GetName() == wxT("warning")) {
                        wxString wtype = subchild->GetPropVal(wxT("type"), wxT(""));
                        wtype.Trim();
                        wtype.Trim(true);
                        if (wtype.IsEmpty()) {
                            m_managerdb.Rollback();
                            wxLogError(_("Error in package list xml (warning[type] empty)."));
                            return false;
                        }
                        wxString desc = subchild->GetNodeContent();
                        desc.Trim();
                        desc.Trim(true);
                        temp.Format("insert into packagelist_warnings values ('%q', '%q', '%q');", guid.mb_str(wxConvUTF8).data(), wtype.mb_str(wxConvUTF8).data(), desc.mb_str(wxConvUTF8).data());
                        m_managerdb.ExecuteUpdate(temp);
                    }
                    subchild = subchild->GetNext();
                }
                if (!haveauthor) {
                    m_managerdb.Rollback();
                    wxLogError(_("Error in package list xml (author missing)."));
                    return false;
                }
            }
            child = child->GetNext();
        }
        m_managerdb.Commit();
    }
    return true;
}

#define DB_FILES_VERSION_VANILLA 1
#define DB_FILES_VERSION_SETTINGS 1
bool ManagerApp::CheckFilesDB() {
    int version_vanilla = DB_FILES_VERSION_VANILLA;
    int version_settings = DB_FILES_VERSION_SETTINGS;
    wxSQLite3StatementBuffer temp;

    m_filesdb.Open(m_appdir.GetPath(wxPATH_GET_SEPARATOR) + wxT("files.db"));
    if (!m_filesdb.TableExists(wxT("versions"))) {
        m_filesdb.ExecuteUpdate(wxT("create table versions(tablename char[20], version int);"));
        temp.Format("insert into versions values ('vanilla', %d);", version_vanilla);
        m_filesdb.ExecuteUpdate(temp);
        temp.Format("insert into versions values ('settings', %d);", version_settings);
        m_filesdb.ExecuteUpdate(temp);
    } else {
        version_vanilla = m_filesdb.ExecuteScalar(wxT("select version from versions where tablename = 'vanilla';"));
        version_settings = m_filesdb.ExecuteScalar(wxT("select version from versions where tablename = 'settings';"));
    }
    if ((version_vanilla > DB_FILES_VERSION_VANILLA) ||
        (version_settings > DB_FILES_VERSION_SETTINGS)) {
        wxLogError(_("Files database modified by a newer version of this application."));
        return false;
    }
    if (!m_filesdb.TableExists(wxT("settings"))) {
        m_filesdb.ExecuteUpdate(wxT("create table settings(setting text, value text);"));
    }
    if (!m_filesdb.TableExists(wxT("vanilla"))) {
        m_filesdb.ExecuteUpdate(wxT("create table vanilla(name text, sha1 text, time int, size int, version int);"));
        wxMessageBox(_("It seems like you are running the RCT3 Customization Manager for the first time, so welcome on board!\n"
                       "To make as sure as possible no package irrecoverably damages your RCT3 installation, the manager will now scan your installation directory and memorize all files. "
                       "Depending on your hardware this process can take some time, usually up to 15 or 20 minutes, so please be patient.\n\n"
                       "BEFORE PRESSING OK please make sure your RCT3 installation directory does not contain any files that do not belong to the official RCT3 installation. "
                       "If there are any (for example custom scenery and/or particle effects), please move them somewhere else now."));
        if (!ScanRCT3Directory())
            return false;
    }
    return true;
}

bool ManagerApp::ScanRCT3Directory() {
    if (!CheckInstallDir())
        return false;

    m_filesdb.Begin();

    m_filesdb.ExecuteUpdate(wxT("delete from vanilla;"));

    wxDateTime start = wxDateTime::Now();
    wxDir rct3dir(m_installdir.GetPath(wxPATH_GET_SEPARATOR));
    wxRCT3Filler rct3fill(&m_filesdb, rct3dir);
    rct3dir.Traverse(rct3fill);
    wxTimeSpan took = wxDateTime::Now().Subtract(start);
    int files = m_filesdb.ExecuteScalar(wxT("select count(*) from vanilla;"));
    wxMessageBox(wxString::Format(_("Took %s to scan %d files."), took.Format(wxT("%H:%M:%S")).c_str(), files));

    m_filesdb.Commit();
    return true;
}

