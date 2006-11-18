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

class wxColourHtmlListBox : public wxHtmlListBox {
public:
    wxColourHtmlListBox(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxVListBoxNameStr):
        wxHtmlListBox(parent, id, pos, size, style, name) {};
protected:
    virtual wxColour GetSelectedTextColour(const wxColour& colFg) const;
};

#endif // COLHTMLLBOX_H_INCLUDED
