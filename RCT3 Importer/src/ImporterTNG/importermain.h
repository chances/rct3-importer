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

#ifndef IMPORTERMAIN_H_INCLUDED
#define IMPORTERMAIN_H_INCLUDED

#include "wx_pch.h"
#include "resources\res_main.h"

#include <wx/aui/aui.h>
#include <sigc++/signal.h>
#include <sigc++/trackable.h>

class frmMain: public rcfrmMain, public sigc::trackable {
private:
    wxAuiManager m_mgr;
    wxAuiNotebook* m_nb;
private: // Signals
    sigc::signal<wxString> getProjectFileName;
    sigc::signal<bool, bool> prepareClose;

    sigc::signal<void> undo;
    sigc::signal<size_t> canUndo;
    sigc::signal<void> redo;
    sigc::signal<size_t> canRedo;
public:
    frmMain(wxWindow* parent);
    virtual ~frmMain();

    void dirtyUpdate(bool ndirty);

    virtual void OnClose(wxCloseEvent& event);
    virtual void OnMenuOpen( wxMenuEvent& event );

    virtual void OnMenuExit( wxCommandEvent& event );
    virtual void OnMenuAbout( wxCommandEvent& event );

    virtual void OnMenuNewProject( wxCommandEvent& event );
    virtual void OnMenuOpenProject( wxCommandEvent& event );
    virtual void OnMenuSaveProject( wxCommandEvent& event );
    virtual void OnMenuSaveAsProject( wxCommandEvent& event );

    virtual void OnMenuUndo( wxCommandEvent& event );
    virtual void OnMenuRedo( wxCommandEvent& event );

    virtual void OnMenuViewPackageInfo( wxCommandEvent& event );
    virtual void OnMenuViewPackageOptions( wxCommandEvent& event );

    virtual void OnMenuViewRefresh( wxCommandEvent& event );

    wxAuiNotebook* getNotebook();
};

#endif // MANAGERMAIN_H_INCLUDED
