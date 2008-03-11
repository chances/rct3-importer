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

#include "importermain.h"

#include <wx/aboutdlg.h>
//#include <wx/propgrid/propgrid.h>

#include "cLogListCtrl.h"

#include "impRawovlFileManager.h"
#include "impViewTree.h"
#include "importerabout.h"
#include "importerapp.h"
#include "importerconfig.h"
#include "importerresourcefiles.h"
#include "version.h"


frmMain::frmMain(wxWindow* parent):rcfrmMain(parent), m_nb(NULL) {

    m_mgr.SetManagedWindow(this);

    m_nb = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB);
    m_mgr.AddPane(m_nb, wxAuiPaneInfo().Name(wxT("main")).
                                          CenterPane());
    impViewTree* pan = new impViewTree(this);
    impRawovlFileManager::getManager().registerView(*pan);
    m_mgr.AddPane(pan, wxAuiPaneInfo().Name(wxT("tree")).
                                          Caption(_("Project Tree")).
                                          Layer(0).
                                          Left().
                                          CaptionVisible(true).
                                          CloseButton(false).
                                          PaneBorder(true));

    cLogListCtrl* log = new cLogListCtrl(this, wxID_ANY);
    m_mgr.AddPane(log, wxAuiPaneInfo().Name(wxT("log")).
                                          Caption(_("Messages")).
                                          Layer(0).
                                          Bottom().
                                          CaptionVisible(true).
                                          CloseButton(false).
                                          PaneBorder(true));

    SetSize(READ_APP_SIZE("Main", GetSize()));
    wxString view = READ_APP_VIEW();
    if (!view.IsEmpty())
        m_mgr.LoadPerspective(view, false);
    m_mgr.Update();

    this->Connect(wxEVT_MENU_OPEN, wxMenuEventHandler(frmMain::OnMenuOpen));

    impRawovlFileManager::getManager().dirtyChange.connect(sigc::mem_fun(*this, &frmMain::dirtyUpdate));
    impRawovlFileManager::getManager().getNotebook.connect(sigc::mem_fun(*this, &frmMain::getNotebook));
    getProjectFileName.connect(sigc::mem_fun(impRawovlFileManager::getManager(), &impRawovlFileManager::getProjectFileName));
    prepareClose.connect(sigc::mem_fun(impRawovlFileManager::getManager(), &impRawovlFileManager::prepareReset));
    undo.connect(sigc::mem_fun(impRawovlFileManager::getManager(), &impRawovlFileManager::undo));
    canUndo.connect(sigc::mem_fun(impRawovlFileManager::getManager(), &impRawovlFileManager::canUndo));
    redo.connect(sigc::mem_fun(impRawovlFileManager::getManager(), &impRawovlFileManager::redo));
    canRedo.connect(sigc::mem_fun(impRawovlFileManager::getManager(), &impRawovlFileManager::canRedo));
}

void frmMain::dirtyUpdate(bool ndirty) {
    wxString fname = getProjectFileName();
    if (fname.IsEmpty())
        fname = _("Unsaved");
    wxString newtitle = (ndirty?wxT("*"):wxT(""));
    newtitle += wxFileName(fname).GetName();
    newtitle += wxT(" - ") + wxGetApp().getTitle();
    SetLabel(newtitle);
}


frmMain::~frmMain() {
    this->Disconnect(wxEVT_MENU_OPEN, wxMenuEventHandler(frmMain::OnMenuOpen));
    WRITE_APP_SIZE("Main", GetSize());
//    m_mgr->destroy_layout();
    WRITE_APP_VIEW(m_mgr.SavePerspective());
    m_mgr.UnInit();
}

void frmMain::OnClose(wxCloseEvent& event) {
    if (prepareClose(event.CanVeto())) {
        Destroy();
    } else {
        event.Veto();
    }
}

void frmMain::OnMenuOpen( wxMenuEvent& event ) {
    m_menuItemUndo->Enable(canUndo());
    m_menuItemRedo->Enable(canRedo());
    event.Skip();
}

void frmMain::OnMenuAbout(wxCommandEvent& WXUNUSED(event)) {
    dlgAbout info(this);
    info.ShowModal();
}

void frmMain::OnMenuExit(wxCommandEvent& WXUNUSED(event)) {
    Close();
}

void frmMain::OnMenuNewProject( wxCommandEvent& event ) {
    impRawovlFileManager::getManager().newProject();
}

void frmMain::OnMenuOpenProject( wxCommandEvent& event ) {
    impRawovlFileManager::getManager().loadProject();
}

void frmMain::OnMenuSaveProject( wxCommandEvent& event ) {
    impRawovlFileManager::getManager().saveProject();
}

void frmMain::OnMenuSaveAsProject( wxCommandEvent& event ) {
    impRawovlFileManager::getManager().saveasProject();
}

void frmMain::OnMenuUndo(wxCommandEvent& WXUNUSED(event)) {
    undo();
}

void frmMain::OnMenuRedo(wxCommandEvent& WXUNUSED(event)) {
    redo();
}

void frmMain::OnMenuViewPackageInfo( wxCommandEvent& WXUNUSED(event) ) {
//    if (m_mgr.GetPane(wxT("info")).IsShown()) {
//        m_mgr.GetPane(wxT("info")).Show(false);
//    } else {
//        m_mgr.GetPane(wxT("info")).Show(true);
//    }
//    m_mgr.Update();
}

void frmMain::OnMenuViewPackageOptions( wxCommandEvent& WXUNUSED(event) ) {
//    if (m_mgr.GetPane(wxT("options")).IsShown()) {
//        m_mgr.GetPane(wxT("options")).Show(false);
//    } else {
//        m_mgr.GetPane(wxT("options")).Show(true);
//    }
//    m_mgr.Update();
}

void frmMain::OnMenuViewRefresh( wxCommandEvent& event ) {
    impRawovlFileManager::getManager().update(wxT("/"));
}


wxAuiNotebook* frmMain::getNotebook() {
    return m_nb;
}
