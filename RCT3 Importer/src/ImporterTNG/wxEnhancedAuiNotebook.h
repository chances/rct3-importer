#ifndef WXENHANCEDAUINOTEBOOK_H
#define WXENHANCEDAUINOTEBOOK_H

#include <wx/aui/auibook.h>

class wxEnhancedAuiNotebook : public wxAuiNotebook {
public:
    wxEnhancedAuiNotebook(): wxAuiNotebook() {}
    wxEnhancedAuiNotebook(wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxAUI_NB_DEFAULT_STYLE):
        wxAuiNotebook(parent, id, pos, size, style) {}

    wxString SavePerspective();
    bool LoadPerspective(const wxString& layout);
protected:
private:
};

#endif // WXENHANCEDAUINOTEBOOK_H
