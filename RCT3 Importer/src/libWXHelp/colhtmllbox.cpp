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
    wxColour bg = GetSelectionBackground();
    if ((((int) bg.Red() + (int) bg.Green() + (int) bg.Blue()) / 3) > 128) {
        // Light Bg
        return wxColour(colFg.Red() / 2, colFg.Green() / 2, colFg.Blue() / 2);
    } else {
        // Dark Bg
        return wxColour(((int) colFg.Red() + 255) / 2, ((int) colFg.Green() + 255) / 2, ((int) colFg.Blue() + 255) / 2);
    }
}

//wxColour wxColourHtmlListBox::GetSelectedTextBgColour(const wxColour& colFg) const {
//    return GetSelectionBackground();
//}
