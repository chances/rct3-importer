/////////////////////////////////////////////////////////////////////////////
// Name:        colhtmllbox.cpp
// Purpose:     Simple wxHtmlListBox subclass with foreground colour
// Author:      Tobias Minich
// Modified by:
// Created:     Oct 28 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx_pch.h"

#include "colhtmllbox.h"

wxColour wxColourHtmlListBox::GetSelectedTextColour(const wxColour& colFg) const {
    if (colFg == *wxBLACK)
        return *wxWHITE;
    return wxColour(((int) colFg.Red() + 255) / 2, ((int) colFg.Green() + 255) / 2, ((int) colFg.Blue() + 255) / 2);
}

