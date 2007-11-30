/////////////////////////////////////////////////////////////////////////////
// Name:        wxgmagick.h
// Purpose:     Mutex to make devIL thread safe & related helper functions
// Author:      Tobias Minich
// Modified by:
// Created:     Nov 13 2007
// RCS-ID:
// Copyright:   (c) 2007 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXGMAGICK_H_INCLUDED
#define WXGMAGICK_H_INCLUDED

#include "wx_pch.h"

#include <IL/il.h>
#include <IL/ilu.h>

extern wxMutex wxILMutex;

#if !wxUSE_IMAGE
class wxSize {
private:
    int w, h;
public:
    wxSize() {
        w = 0;
        h = 0;
    }
    wxSize(int nw, int nh) {
        w = nw;
        h = nh;
    }
    int GetWidth() {return w;}
    int GetHeight() {return h;}
};
#endif

wxSize getBitmapSize(const char *filename);

//bool getBitmapInfo(const char *filename, ILinfo& info);
//bool ilInfoHasAlpha(const ILinfo& info);

#endif // WXDEVIL_H_INCLUDED
