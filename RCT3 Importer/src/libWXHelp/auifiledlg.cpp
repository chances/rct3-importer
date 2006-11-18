/////////////////////////////////////////////////////////////////////////////
// Name:        auifiledlg.cpp
// Purpose:     AUI generic file dialog
// Author:      Tobias Minich
// Modified by:
// Created:     Oct 28 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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

