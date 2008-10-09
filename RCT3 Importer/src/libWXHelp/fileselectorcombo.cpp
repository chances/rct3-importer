/////////////////////////////////////////////////////////////////////////////
// Name:        fileselectorcombo.cpp
// Purpose:     File selector combo control from the wxComboCtrl sample
// Author:      Jaakko Salli
// Modified by: Tobias Minich
// Created:     Apr-30-2006
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXHELP_BASE

#include "fileselectorcombo.h"

#include <wx/filename.h>

//IMPLEMENT_DYNAMIC_CLASS(wxFileSelectorCombo, wxComboCtrl)

template <>
void wxFileSelectorCombo<wxDirDialog>::OnButtonClick() {
    // Show standard wxFileDialog on button click
    if (m_dialog) {
        wxFileName name = GetValue();
        if (name!=wxT("")) {
            m_dialog->SetPath(name.GetFullPath());
        } else if (m_defdir) {
            m_dialog->SetPath(m_defdir->GetFullPath());
        }
        if ( m_dialog->ShowModal() == wxID_OK ) {
            SetValue(m_dialog->GetPath());
            wxCommandEvent event(wxEVT_COMMAND_COMBOBOX_SELECTED, GetId());
            event.SetEventObject(this);
            event.SetString(m_dialog->GetPath());
            GetEventHandler()->AddPendingEvent(event);
        }
    } else {
        wxMessageBox(_("No file dialog set"), _("Internal Error"), wxOK|wxICON_ERROR, this);
    }
}

#endif
