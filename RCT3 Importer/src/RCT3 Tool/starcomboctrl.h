#ifndef STARCOMBOCTRL_H_INCLUDED
#define STARCOMBOCTRL_H_INCLUDED

class wxStarComboCtrl : public wxComboCtrl
{
public:
    wxStarComboCtrl() : wxComboCtrl() {}
    virtual ~wxStarComboCtrl() {}

    wxStarComboCtrl(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
                        const wxString& value = wxEmptyString,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxComboBoxNameStr)
        : wxComboCtrl()
    {
        Create(parent,id,value,
               pos,size,
               // Style flag wxCC_STD_BUTTON makes the button
               // behave more like a standard push button.
               style | wxCC_STD_BUTTON,
               validator,name);

        SetButtonBitmaps(wxXmlResource::Get()->LoadBitmap("bookmark_16x16"),true);
    }

    virtual void OnButtonClick() {
        wxCommandEvent event(wxEVT_COMMAND_COMBOBOX_SELECTED, GetId());
        event.SetEventObject(this);
        GetEventHandler()->AddPendingEvent(event);
    }

    // Implement empty DoSetPopupControl to prevent assertion failure.
    virtual void DoSetPopupControl(wxComboPopup* WXUNUSED(popup)) {}

};


#endif // STARCOMBOCTRL_H_INCLUDED
