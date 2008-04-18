#ifndef FILESELECTORCOMBOFILE_H_INCLUDED
#define FILESELECTORCOMBOFILE_H_INCLUDED

#include <wx/filedlg.h>
#include "fileselectorcombo.h"

//typedef wxFileSelectorCombo<wxFileDialog> wxFileSelectorComboFile;


class wxFileSelectorComboFile: public wxFileSelectorCombo<wxFileDialog> {
public:
    wxFileSelectorComboFile(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
                        const wxString& value = wxEmptyString,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxComboBoxNameStr):
        wxFileSelectorCombo<wxFileDialog>(parent, id, value, pos, size, style, validator, name) {}
    wxFileSelectorComboFile(wxWindow* parent,
                        wxWindowID id, const wxString& value = "",
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        int n = 0,
                        const wxString choices[] = NULL,
                        long style = 0,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxComboBoxNameStr):
        wxFileSelectorCombo<wxFileDialog>(parent, id, value, pos, size, style, validator, name) {}
};

class wxFileSelectorComboDir: public wxFileSelectorCombo<wxDirDialog> {
public:
    wxFileSelectorComboDir(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
                        const wxString& value = wxEmptyString,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxComboBoxNameStr):
        wxFileSelectorCombo<wxDirDialog>(parent, id, value, pos, size, style, validator, name) {}
    wxFileSelectorComboDir(wxWindow* parent,
                        wxWindowID id, const wxString& value = "",
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        int n = 0,
                        const wxString choices[] = NULL,
                        long style = 0,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxComboBoxNameStr):
        wxFileSelectorCombo<wxDirDialog>(parent, id, value, pos, size, style, validator, name) {}
};


#endif // FILESELECTORCOMBOFILE_H_INCLUDED
