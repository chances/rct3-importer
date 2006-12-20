/////////////////////////////////////////////////////////////////////////////
// Name:        wrappereditevent.h
// Purpose:     Event to tell the dialog that a wrapper starts or stops editing
// Author:      Tobias Minich
// Modified by:
// Created:     Dec 07 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WRAPPEREDITEVENT_H_INCLUDED
#define WRAPPEREDITEVENT_H_INCLUDED

#include "wx_pch.h"

#define wxWRAPPER_START_EDITING 1
#define wxWRAPPER_STOP_EDITING 2

DECLARE_EVENT_TYPE(wxEVT_WRAPPER, -1)

#endif // WRAPPEREDITEVENT_H_INCLUDED
