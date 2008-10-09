/////////////////////////////////////////////////////////////////////////////
// Name:        silence.h
// Purpose:     Support classes/interface to turn off need for user interaction
// Author:      Tobias Minich
// Modified by:
// Created:     Nov 29 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXHELP_BASE

#include "silence.h"

//IMPLEMENT_CLASS(wxSilent,wxObject)

wxSilent::wxSilent(const wxSilent& copyFrom) {
    Copy(copyFrom);
}

bool wxSilent::Copy(const wxSilent& val) {
    m_silent = val.m_silent;
    return true;
}

wxSilencer::wxSilencer(wxSilent* subject) {
    m_subject = subject;
    if (m_subject)
        m_subject->Silence();
}

wxSilencer::wxSilencer(void* WXUNUSED(subject)) {
    m_subject = NULL;
}

wxSilencer::~wxSilencer() {
    if (m_subject)
        m_subject->Silence(false);
}

#endif
