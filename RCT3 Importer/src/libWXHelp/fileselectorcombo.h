/////////////////////////////////////////////////////////////////////////////
// Name:        fileselectorcombo.h
// Purpose:     File selector combo control from the wxComboCtrl sample
// Author:      Jaakko Salli
// Modified by: Tobias Minich
// Created:     Apr-30-2006
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef FILESELECTORCOMBO_H_INCLUDED
#define FILESELECTORCOMBO_H_INCLUDED

#include "wx_pch.h"

#include <wx/combo.h>
#include <wx/filename.h>

class wxFileSelectorCombo : public wxComboCtrl
{
DECLARE_DYNAMIC_CLASS(wxFileSelectorCombo)
public:
    wxFileSelectorCombo() : wxComboCtrl() { Init(); }

    wxFileSelectorCombo(wxWindow *parent,
                        wxFileDialogBase* dialog,
                        wxFileName* default_dir = NULL,
                        wxWindowID id = wxID_ANY,
                        const wxString& value = wxEmptyString,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxComboBoxNameStr);

    virtual ~wxFileSelectorCombo() {
        /*
        if (m_dialog)
            m_dialog->Destroy();
        */
    }

    virtual void OnButtonClick();

    // Implement empty DoSetPopupControl to prevent assertion failure.
    virtual void DoSetPopupControl(wxComboPopup* WXUNUSED(popup)) {};

protected:
    wxFileDialogBase* m_dialog;
    wxFileName* m_defdir;

private:
    void Init() {
        m_dialog = NULL;
    };
};


#endif // FILESELECTORCOMBO_H_INCLUDED
