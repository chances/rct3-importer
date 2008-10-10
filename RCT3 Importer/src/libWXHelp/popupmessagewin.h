/////////////////////////////////////////////////////////////////////////////
// Name:        popupmessagewin.h
// Purpose:     Basic transient version of a wxMessageBox
// Author:      Tobias Minich
// Modified by:
// Created:     Nov 18 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef POPUPMESSAGEWIN_H_INCLUDED
#define POPUPMESSAGEWIN_H_INCLUDED

#include <wx/popupwin.h>

class wxPopupTransientMessageWindow : public wxPopupTransientWindow {
public:
    wxPopupTransientMessageWindow():wxPopupTransientWindow() {};
    wxPopupTransientMessageWindow(const wxString& message, const wxString& title = wxT(""), wxWindow *parent = NULL, int style = wxRAISED_BORDER);

    virtual void Dismiss();

protected:
    void OnOk(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxPopupTransientMessageWindow)
    DECLARE_NO_COPY_CLASS(wxPopupTransientMessageWindow)
};

#endif // POPUPMESSAGEWIN_H_INCLUDED
