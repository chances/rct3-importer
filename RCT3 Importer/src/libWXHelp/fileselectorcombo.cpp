/////////////////////////////////////////////////////////////////////////////
// Name:        fileselectorcombo.cpp
// Purpose:     File selector combo control from the wxComboCtrl sample
// Author:      Jaakko Salli
// Modified by: Tobias Minich
// Created:     Apr-30-2006
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "fileselectorcombo.h"

#include <wx/filename.h>

IMPLEMENT_DYNAMIC_CLASS(wxFileSelectorCombo, wxComboCtrl)

wxFileSelectorCombo::wxFileSelectorCombo(wxWindow *parent,
                    wxFileDialogBase* dialog,
                    wxFileName* default_dir,
                    wxWindowID id,
                    const wxString& value,
                    const wxPoint& pos,
                    const wxSize& size,
                    long style,
                    const wxValidator& validator,
                    const wxString& name)
        : wxComboCtrl() {
    Init();
    Create(parent,id,value,
           pos,size,
           // Style flag wxCC_STD_BUTTON makes the button
           // behave more like a standard push button.
           style | wxCC_STD_BUTTON,
           validator,name);

    if (dialog) {
        m_dialog = dialog;
//        m_dialog->Reparent(this);
    } else {
        m_dialog = new wxFileDialog(this,
                         _("Choose File"),
                         wxEmptyString,
                         GetValue(),
                         _("All files (*.*)|*.*"),
                         wxFD_OPEN);
    }
    m_defdir = default_dir;

    //
    // Prepare custom button bitmap (just '...' text)
    wxMemoryDC dc;
    wxBitmap bmp(12,16);
    dc.SelectObject(bmp);

    // Draw transparent background
    wxColour magic(255,0,255);
    wxBrush magicBrush(magic);
    dc.SetBrush( magicBrush );
    dc.SetPen( *wxTRANSPARENT_PEN );
    dc.DrawRectangle(0,0,bmp.GetWidth(),bmp.GetHeight());

    // Draw text
    wxString str = wxT("...");
    int w,h;
    dc.GetTextExtent(str, &w, &h, 0, 0);
    dc.DrawText(str, (bmp.GetWidth()-w)/2, (bmp.GetHeight()-h)/2);

    dc.SelectObject( wxNullBitmap );

    // Finalize transparency with a mask
    wxMask *mask = new wxMask( bmp, magic );
    bmp.SetMask( mask );

    SetButtonBitmaps(bmp,true);
}

void wxFileSelectorCombo::OnButtonClick() {
    // Show standard wxFileDialog on button click
    if (m_dialog) {
        wxFileName name = GetValue();
        if (name!=wxT("")) {
            m_dialog->SetFilename(name.GetFullName());
            m_dialog->SetDirectory(name.GetPath());
        } else if (m_defdir) {
            m_dialog->SetDirectory(m_defdir->GetPath());
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
