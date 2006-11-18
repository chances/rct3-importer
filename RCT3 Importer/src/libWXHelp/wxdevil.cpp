/////////////////////////////////////////////////////////////////////////////
// Name:        wxdevil.cpp
// Purpose:     Mutex to make devIL thread safe
// Author:      Tobias Minich
// Modified by:
// Created:     Oct 28 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx_pch.h"

#include "wxdevil.h"
wxMutex wxILMutex(wxMUTEX_DEFAULT);
