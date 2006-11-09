///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// AUI generic file dialog
// Copyright (C) 2006 Tobias Minch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////

#include "wx_pch.h"

#include <wx/config.h>
#include <wx/artprov.h>
#include "auifiledlg.h"

IMPLEMENT_DYNAMIC_CLASS(wxAUIFileDialog, wxGenericFileDialog)

wxAUIFileDialog::wxAUIFileDialog(wxWindow *parent,
                                 const wxString& message,
                                 const wxString& defaultDir,
                                 const wxString& defaultFile,
                                 const wxString& wildCard,
                                 long  style,
                                 const wxPoint& pos,
                                 const wxSize& sz,
                                 const wxString& name) : wxGenericFileDialog() {
    m_mgr.SetManagedWindow(this);
    Create( parent, message, defaultDir, defaultFile, wildCard, style, pos, sz, name);
}

bool wxAUIFileDialog::Create( wxWindow *parent,
                              const wxString& message,
                              const wxString& defaultDir,
                              const wxString& defaultFile,
                              const wxString& wildCard,
                              long  style,
                              const wxPoint& pos,
                              const wxSize& sz,
                              const wxString& name) {

    if (!CreatePrepare(parent, message, defaultDir, defaultFile,
                       wildCard, style, pos, sz, name, false))
    {
        return false;
    }

    m_panelMain = new wxPanel(this);
    CreateDefaultControls(m_panelMain, wildCard, false);
    m_mgr.AddPane(m_panelMain, wxAuiPaneInfo().Name(wxT("fileselect")).CenterPane().BestSize(wxSize(600,400)));
    m_mgr.Update();
    CreateFinish(false);

    return true;
}

wxAUIFileDialog::~wxAUIFileDialog() {
    m_mgr.UnInit();
}

