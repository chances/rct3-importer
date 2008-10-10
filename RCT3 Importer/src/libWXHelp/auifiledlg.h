/////////////////////////////////////////////////////////////////////////////
// Name:        auifiledlg.h
// Purpose:     AUI generic file dialog
// Author:      Tobias Minich
// Modified by:
// Created:     Oct 28 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _AUIFILEDLG_H_
#define _AUIFILEDLG_H_

#include "filedlgg.h"
#include <wx/aui/aui.h>

class wxAUIFileDialog: public wxGenericFileDialog
{
public:
    wxAUIFileDialog() : wxGenericFileDialog() {m_mgr.SetManagedWindow(this); }

    wxAUIFileDialog(wxWindow *parent,
                        const wxString& message = wxFileSelectorPromptStr,
                        const wxString& defaultDir = wxEmptyString,
                        const wxString& defaultFile = wxEmptyString,
                        const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
                        long style = wxFD_DEFAULT_STYLE,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& sz = wxDefaultSize,
                        const wxString& name = wxFileDialogNameStr);
    bool Create( wxWindow *parent,
                        const wxString& message = wxFileSelectorPromptStr,
                        const wxString& defaultDir = wxEmptyString,
                        const wxString& defaultFile = wxEmptyString,
                        const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
                        long style = wxFD_DEFAULT_STYLE,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& sz = wxDefaultSize,
                        const wxString& name = wxFileDialogNameStr);

    virtual ~wxAUIFileDialog();

protected:
    wxAuiManager m_mgr;
    wxPanel *m_panelMain;

private:
    DECLARE_DYNAMIC_CLASS(wxAUIFileDialog)
//    DECLARE_EVENT_TABLE()

};

#endif
