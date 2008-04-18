/////////////////////////////////////////////////////////////////////////////
// Name:        colhtmllbox.h
// Purpose:     Simple wxHtmlListBox subclass with foreground colour
// Author:      Tobias Minich
// Modified by:
// Created:     Oct 28 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef COLHTMLLBOX_H_INCLUDED
#define COLHTMLLBOX_H_INCLUDED

#include <wx/htmllbox.h>

#define HTML_INSET_START wxT("<table><tr><td width=5></td><td width=100%>")
#define HTML_INSET_END wxT("</td></tr></table>")

class wxColourHtmlListBox : public wxHtmlListBox {
public:
    wxColourHtmlListBox(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxVListBoxNameStr):
        wxHtmlListBox(parent, id, pos, size, style, name) {
        SetSelectionBackground(wxColour(0x00, 0x33, 0x66));
        //SetSelectionBackground(wxColour(0xEE, 0xCC, 0xAA));
    };
    virtual void UpdateContents() {};
protected:
    virtual wxColour GetSelectedTextColour(const wxColour& colFg) const;
    //virtual wxColour GetSelectedTextBgColour(const wxColour& colFg) const;
};

#endif // COLHTMLLBOX_H_INCLUDED
