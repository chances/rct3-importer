/////////////////////////////////////////////////////////////////////////////
// Name:        wxdevil.h
// Purpose:     Mutex to make devIL thread safe & related helper functions
// Author:      Tobias Minich
// Modified by:
// Created:     Oct 28 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXDEVIL_H_INCLUDED
#define WXDEVIL_H_INCLUDED

#include "wx_pch.h"

extern wxMutex wxILMutex;

wxSize getBitmapSize(const char *filename);

#endif // WXDEVIL_H_INCLUDED
