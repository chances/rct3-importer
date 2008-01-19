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

#include "managermain.h"

#include <wx/aboutdlg.h>
#include <wx/html/m_templ.h>
#include <wx/propgrid/propgrid.h>

#include "managerabout.h"
#include "managerapp.h"
#include "managerconfig.h"
#include "managerresourcefiles.h"
#include "version.h"

TAG_HANDLER_BEGIN(WXINFOTAG, "INFO")

TAG_HANDLER_PROC(tag)
{
    wxString ret = wxT("INFO TAG PARSING ERROR");
    if (tag.HasParam(wxT("VERSION"))) {
        wxString version = tag.GetParam(wxT("VERSION"));
        if (!version.CmpNoCase(wxT("date")))
            ret = wxString(AutoVersion::DATE, wxConvLocal);
        else if (!version.CmpNoCase(wxT("month")))
            ret = wxString(AutoVersion::MONTH, wxConvLocal);
        else if (!version.CmpNoCase(wxT("year")))
            ret = wxString(AutoVersion::YEAR, wxConvLocal);
        else if (!version.CmpNoCase(wxT("status")))
            ret = wxString(AutoVersion::STATUS, wxConvLocal);
        else if (!version.CmpNoCase(wxT("status_short")))
            ret = wxString(AutoVersion::STATUS_SHORT, wxConvLocal);
        else if (!version.CmpNoCase(wxT("major")))
            ret = wxString::Format(wxT("%ld"), AutoVersion::MAJOR);
        else if (!version.CmpNoCase(wxT("minor")))
            ret = wxString::Format(wxT("%ld"), AutoVersion::MINOR);
        else if (!version.CmpNoCase(wxT("build")))
            ret = wxString::Format(wxT("%ld"), AutoVersion::BUILD);
        else if (!version.CmpNoCase(wxT("revision")))
            ret = wxString::Format(wxT("%ld"), AutoVersion::REVISION);
    }
    if (tag.HasParam(wxT("WXWIDGETS"))) {
        wxString version = tag.GetParam(wxT("WXWIDGETS"));
        if (!version.CmpNoCase(wxT("version")))
            ret = wxVERSION_STRING;
        else if (!version.CmpNoCase(wxT("build"))) {
#ifdef wxUSE_UNICODE
            ret = wxT("Unicode");
#else
            ret = wxT("Ansi");
#endif
        }
        else if (!version.CmpNoCase(wxT("debug"))) {
#ifdef __WXDEBUG__
            ret = wxT("Debug");
#else
            ret = wxT("Release");
#endif
        }
        else if (!version.CmpNoCase(wxT("platform"))) {
#if defined(__WXMSW__)
            ret = wxT("Windows");
#elif defined(__WXMOTIF__)
            ret = wxT("Motif");
#elif defined(__WXPALMOS__)
            ret = wxT("PalmOS");
#elif defined(__WXGTK__)
            ret = wxT("GTK");
#elif defined(__WXPM__)
            ret = wxT("PM");
#elif defined(__WXMAC__)
            ret = wxT("Mac");
#elif defined(__WXCOCOA__)
            ret = wxT("Cocoa");
#elif defined(__X__)
            ret = wxT("X");
#elif defined(__WXMGL__)
            ret = wxT("MGL");
#elif defined(__WXDFB__)
            ret = wxT("DFB");
#elif defined(__WXX11__)
            ret = wxT("X11");
#endif
        }
    }

    if (tag.HasParam(wxT("SP"))) {
        ret += wxT(" ");
    }

    m_WParser->GetContainer()->InsertCell(new wxHtmlWordCell(ret, *m_WParser->GetDC()));

    return false;
}

TAG_HANDLER_END(WXINFOTAG)



TAGS_MODULE_BEGIN(wxInfoTag)

    TAGS_MODULE_ADD(WXINFOTAG)

TAGS_MODULE_END(wxInfoTag)


frmMain::frmMain(wxWindow* parent):rcfrmMain(parent) {

    m_mgr.SetManagedWindow(this);

    m_lbPackages = new wxPackageListBox(this);
    m_lbPackages->RefreshList();
    m_mgr.AddPane(m_lbPackages, wxAuiPaneInfo().Name(wxT("main")).
                                          Caption(_("Package List")).
                                          CenterPane().
                                          MaximizeButton().
                                          CaptionVisible(true));
    wxAuiNotebook* c_infonb = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB);
    m_mgr.AddPane(c_infonb, wxAuiPaneInfo().Name(wxT("info")).
                                          Caption(_("Package Info")).
                                          Layer(0).
                                          Right().
                                          CaptionVisible(true).
                                          PaneBorder(true));
    wxPropertyGrid* c_options = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_AUTO_SORT | wxPG_SPLITTER_AUTO_CENTER | wxPG_DEFAULT_STYLE);
    m_mgr.AddPane(c_options, wxAuiPaneInfo().Name(wxT("options")).
                                          Caption(_("Package Options")).
                                          Layer(0).
                                          Right().
                                          CaptionVisible(true).
                                          PaneBorder(true));

    SetSize(READ_APP_SIZE("Main", GetSize()));
    wxString view = READ_APP_VIEW();
    if (!view.IsEmpty())
        m_mgr.LoadPerspective(view, false);
    m_mgr.Update();

/*
    wxPanel* c_main = new wxPanel(this, frmMain::PAN_MAIN);
    wxPanel* c_info = new rcpanInfo(this, frmMain::PAN_INFO);
    wxPanel* c_options = new rcpanOptions(this, frmMain::PAN_OPTIONS);

    m_mgr = std::auto_ptr<ifm::InterfaceManager>(new ifm::InterfaceManager(this, c_main));

    ifm::layout::Layout layout;
    ifm::layout::Container container;
    container.add(frmMain::PAN_INFO, "Packet Info");
    container.add(frmMain::PAN_OPTIONS, "Packet Options");
    layout.add(container.copy(), ifm::RIGHT);

    SetSize(READ_APP_SIZE("Main", GetSize()));

    m_mgr->set_layout(layout);
*/
    this->Connect(wxEVT_MENU_OPEN, wxMenuEventHandler(frmMain::OnMenuOpen));
}

frmMain::~frmMain() {
    this->Disconnect(wxEVT_MENU_OPEN, wxMenuEventHandler(frmMain::OnMenuOpen));
    WRITE_APP_SIZE("Main", GetSize());
//    m_mgr->destroy_layout();
    WRITE_APP_VIEW(m_mgr.SavePerspective());
    m_mgr.UnInit();
}

void frmMain::OnMenuAbout(wxCommandEvent& WXUNUSED(event)) {
/*
    wxAboutDialogInfo info;
    info.SetName(_("RCT3 Customization Manager"));
    info.SetVersion(wxString::Format(_("%ld.%ld %s (Build %ld)"), AutoVersion::MAJOR, AutoVersion::MINOR, wxString(AutoVersion::STATUS, wxConvLocal).c_str(), AutoVersion::BUILD));
    info.AddDeveloper(wxT("Belgabor"));
    info.SetWebSite(wxT("http://rct3.sourceforge.net"), _("RCT3 Importer and Customization Manager web site"));
    info.SetCopyright(_T("(C) 2008 Belgabor <belgabor@gmx.de>"));

    wxAboutBox(info);
*/
    dlgAbout info(this);
    info.ShowModal();
}

void frmMain::OnMenuExit(wxCommandEvent& WXUNUSED(event)) {
    Close();
}

void frmMain::OnMenuSettingsRescan(wxCommandEvent& WXUNUSED(event)) {
    if (::wxMessageBox(_("Are you sure you want to rescan RCT3's installation directory?\n\n"
                         "BEFORE PRESSING YES please make sure your RCT3 installation directory does not contain any files that do not belong to the official RCT3 installation. "
                         "If there are any (for example custom scenery and/or particle effects), please move them somewhere else first."), _("Question"),
                         wxYES_NO|wxNO_DEFAULT|wxICON_QUESTION, this) == wxYES) {
        if (!::wxGetApp().ScanRCT3Directory())
            ::wxMessageBox(_("Scanning failed. Make sure the correct installation directory is scanned and you have sufficient rights to access the directory."),
                           _("Error"), wxICON_ERROR, this);
    }
}

void frmMain::OnMenuOpen( wxMenuEvent& WXUNUSED(event) ) {
    wxMenuItemList li = m_menuGroupInstall->GetMenuItems();
    for (wxMenuItemList::iterator it = li.begin(); it != li.end(); ++it) {
        m_menuGroupInstall->Destroy(*it);
    }
    li = m_menuGroupUninstall->GetMenuItems();
    for (wxMenuItemList::iterator it = li.begin(); it != li.end(); ++it) {
        m_menuGroupUninstall->Destroy(*it);
    }
    wxSQLite3ResultSet groups = ::wxGetApp().GetManagerDB().ExecuteQuery(wxT("select distinct name from groups;"));
    if (groups.Eof()) {
        wxMenuItem* i = new wxMenuItem(m_menuGroupInstall, wxID_ANY, _("No groups defined"));
        m_menuGroupInstall->Append(i);
        i->Enable(false);
        i = new wxMenuItem(m_menuGroupUninstall, wxID_ANY, _("No groups defined"));
        m_menuGroupUninstall->Append(i);
        i->Enable(false);
    } else {
        while (groups.NextRow()) {
            m_menuGroupInstall->Append(wxID_ANY, groups.GetString(0));
            m_menuGroupUninstall->Append(wxID_ANY, groups.GetString(0));
        }
    }

    if (m_mgr.GetPane(wxT("info")).IsShown()) {
        m_menuItemViewInfo->SetItemLabel(_("Hide Package &Info"));
    } else {
        m_menuItemViewInfo->SetItemLabel(_("Show Package &Info"));
    }
    if (m_mgr.GetPane(wxT("options")).IsShown()) {
        m_menuItemViewOptions->SetItemLabel(_("Hide Package &Options"));
    } else {
        m_menuItemViewOptions->SetItemLabel(_("Show Package &Options"));
    }
}

void frmMain::OnMenuViewPackageInfo( wxCommandEvent& WXUNUSED(event) ) {
    if (m_mgr.GetPane(wxT("info")).IsShown()) {
        m_mgr.GetPane(wxT("info")).Show(false);
    } else {
        m_mgr.GetPane(wxT("info")).Show(true);
    }
    m_mgr.Update();
}

void frmMain::OnMenuViewPackageOptions( wxCommandEvent& WXUNUSED(event) ) {
    if (m_mgr.GetPane(wxT("options")).IsShown()) {
        m_mgr.GetPane(wxT("options")).Show(false);
    } else {
        m_mgr.GetPane(wxT("options")).Show(true);
    }
    m_mgr.Update();
}

void frmMain::OnMenuChangeView( wxCommandEvent& event ) {
    bool all = m_menuItemViewShowAll->IsChecked();
    if (event.GetEventObject() == m_menuItemViewShowAll) {
        all = !all;
        m_menuItemViewShowAll->Check(all);
    }
    bool packages = m_menuItemViewByPackage->IsChecked();
    if (event.GetEventObject() == m_menuItemViewByPackage) {
        if (packages)
            return;
        packages = true;
        m_menuItemViewByPackage->Check(true);
    }
    if (event.GetEventObject() == m_menuItemViewByAuthor) {
        if (!packages)
            return;
        packages = false;
        m_menuItemViewByAuthor->Check(true);
    }
    m_lbPackages->SetSettings(
        ((all)?wxPackageListBox::TYPE_ALL:0) |
        ((packages)?(
            (READ_APP_VIEW_PACK_SHORT())?wxPackageListBox::TYPE_SHORTNAME:0
        ):(
            wxPackageListBox::TYPE_AUTHORLIST |
            ((READ_APP_VIEW_AUTH_SHORT())?wxPackageListBox::TYPE_SHORTNAME:0) |
            ((READ_APP_VIEW_AUTH_OPEN())?wxPackageListBox::TYPE_INITSHOW:0)
        ))
    );
    m_lbPackages->RefreshList();
}
