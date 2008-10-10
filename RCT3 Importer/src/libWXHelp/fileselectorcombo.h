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

#include <wx/combo.h>
#include <wx/dcmemory.h>
#include <wx/dirdlg.h>
#include <wx/filename.h>

template <class T>
class wxFileSelectorCombo : public wxComboCtrl
{
//DECLARE_DYNAMIC_CLASS(wxFileSelectorCombo)
public:
    wxFileSelectorCombo() : wxComboCtrl() { Init(); }

    wxFileSelectorCombo(wxWindow *parent,
                        T* dialog,
                        wxFileName* default_dir = NULL,
                        wxWindowID id = wxID_ANY,
                        const wxString& value = wxEmptyString,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxComboBoxNameStr): wxComboCtrl()  {
        Create(parent, dialog, default_dir, id, value, pos, size, style, validator, name);
    }

    wxFileSelectorCombo(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
                        const wxString& value = wxEmptyString,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxComboBoxNameStr): wxComboCtrl() {
        Create(parent, NULL, NULL, id, value, pos, size, style, validator, name);
    }

    void Create(wxWindow *parent,
                        T* dialog,
                        wxFileName* default_dir = NULL,
                        wxWindowID id = wxID_ANY,
                        const wxString& value = wxEmptyString,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxComboBoxNameStr);

    void Assign(T* dialog, wxFileName* default_dir = NULL) {
        m_dialog = dialog;
        m_defdir = default_dir;
    }

    virtual ~wxFileSelectorCombo() {
        /*
        if (m_dialog)
            m_dialog->Destroy();
        */
    }

    virtual void OnButtonClick();
    virtual void ShowPopup() { OnButtonClick(); }

    // Implement empty DoSetPopupControl to prevent assertion failure.
    virtual void DoSetPopupControl(wxComboPopup* WXUNUSED(popup)) {};

protected:
    T* m_dialog;
    wxFileName* m_defdir;

private:
    void Init() {
        m_dialog = NULL;
    };
};

template <class T>
void wxFileSelectorCombo<T>::Create(wxWindow *parent,
                    T* dialog,
                    wxFileName* default_dir,
                    wxWindowID id,
                    const wxString& value,
                    const wxPoint& pos,
                    const wxSize& size,
                    long style,
                    const wxValidator& validator,
                    const wxString& name)
        {
    Init();
    wxComboCtrl::Create(parent,id,value,
           pos,size,
           // Style flag wxCC_STD_BUTTON makes the button
           // behave more like a standard push button.
           style | wxCC_STD_BUTTON,
           validator,name);

    m_dialog = dialog;
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

template <class T>
void wxFileSelectorCombo<T>::OnButtonClick() {
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

template <>
void wxFileSelectorCombo<wxDirDialog>::OnButtonClick();

#endif // FILESELECTORCOMBO_H_INCLUDED
