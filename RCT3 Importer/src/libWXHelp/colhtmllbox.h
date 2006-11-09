///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// Simple wxHtmlListBox subclass with foreground colour
// Copyright (C) 2006 Tobias Minch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////

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
