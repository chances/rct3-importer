/////////////////////////////////////////////////////////////////////////////
// Name:        wxutilityfunctions.cpp
// Purpose:     wx utility functions
// Author:      Tobias Minich
// Modified by:
// Created:     2008-02-22
// RCS-ID:
// Copyright:   (c) Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXHELP_BASE

#include "wxutilityfunctions.h"

void wxWindowDestroyer(wxWindow* window) {
    if (window)
        window->Destroy();
}

#endif
