/////////////////////////////////////////////////////////////////////////////
// Name:        popupmessagewin.cpp
// Purpose:     Basic transient version of a wxMessageBox
// Author:      Tobias Minich
// Modified by:
// Created:     Nov 18 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXHELP_BASE

#include "popupmessagewin.h"
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

BEGIN_EVENT_TABLE(wxPopupTransientMessageWindow, wxPopupTransientWindow)
    EVT_BUTTON(wxID_OK, wxPopupTransientMessageWindow::OnOk)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxPopupTransientMessageWindow, wxPopupTransientWindow)

wxPopupTransientMessageWindow::wxPopupTransientMessageWindow(const wxString& message, const wxString& title, wxWindow *parent, int style):wxPopupTransientWindow(parent, style) {
    if (title != wxT("")) {
        style |= wxCAPTION;
        SetWindowStyleFlag(style);
        SetLabel(title);
        Refresh();
    }

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText *text = new wxStaticText(this, wxID_ANY, message);
    sizer->Add(text, wxSizerFlags().Center().Border());
    wxButton *button = new wxButton(this, wxID_OK);
    sizer->Add(button, wxSizerFlags().Center().Border());

    SetSizer(sizer);

    Fit();
    Layout();
    sizer->SetSizeHints(this);

    if (parent) {
        wxSize p = parent->GetSize();
        wxSize c = GetSize();
        int x = (p.GetWidth() - c.GetWidth()) / 2;
        int y = (p.GetHeight() - c.GetHeight()) / 2;
        parent->ClientToScreen(&x, &y);
        Move(x, y);
    } else
        Centre(wxCENTRE_ON_SCREEN);
}

void wxPopupTransientMessageWindow::OnOk(wxCommandEvent& WXUNUSED(event)) {
    Dismiss();
}

void wxPopupTransientMessageWindow::Dismiss() {
    wxPopupTransientWindow::Dismiss();
    Close(true);
}

#endif
