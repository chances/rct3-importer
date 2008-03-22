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
};


#endif // FILESELECTORCOMBOFILE_H_INCLUDED
