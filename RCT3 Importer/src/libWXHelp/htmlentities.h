/////////////////////////////////////////////////////////////////////////////
// Name:        htmlentities.h
// Purpose:     Encode basic html entities for wxHtmlListBoxes
// Author:      Tobias Minich
// Modified by:
// Created:     Oct 28 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef HTMLENTITIES_H_INCLUDED
#define HTMLENTITIES_H_INCLUDED

#include "wx_pch.h"

inline wxString wxEncodeHtmlEntities(const wxString src) {
    wxString res = src;
    res.Replace(_T("&"), _T("&amp;"), true);
    res.Replace(_T("<"), _T("&lt;"), true);
    res.Replace(_T(">"), _T("&gt;"), true);
    return res;
}

#endif // HTMLENTITIES_H_INCLUDED
