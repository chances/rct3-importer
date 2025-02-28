/////////////////////////////////////////////////////////////////////////////
// Name:        confhelp.h
// Purpose:     Function templates to help wonking with wxConfig
// Author:      Tobias Minich
// Modified by:
// Created:     2007-11-13
// RCS-ID:
// Copyright:   (c) Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef CONFHELP_H_INCLUDED
#define CONFHELP_H_INCLUDED

#include <wx/confbase.h>
#if wxUSE_IMAGE
#include <wx/gdicmn.h>
#endif
//#ifndef DWORD
//#include <windows.h>
//#endif
//#include <d3d9types.h>
#include "vertex.h"

#include "basicsize.h"

template <class T>
T wxcRead(const wxString& path, const T& def) {
    wxConfigBase* conf = wxConfigBase::Get(false);
    if (!conf)
        return def;
    T val;
    if (conf->Read(path, &val, def))
        return val;
    else
        return def;
}

template <>
r3::MATRIX wxcRead(const wxString& path, const r3::MATRIX& def);
#if wxUSE_IMAGE
template <>
wxSize wxcRead(const wxString& path, const wxSize& def);
#endif

template <class T>
bool wxcWrite(const wxString& path, const T& val) {
    wxConfigBase* conf = wxConfigBase::Get(false);
    if (!conf)
        return false;
    return conf->Write(path, val);
}

template <>
bool wxcWrite(const wxString& path, const r3::MATRIX& val);
#if wxUSE_IMAGE
template <>
bool wxcWrite(const wxString& path, const wxSize& val);
#endif

bool wxcFlush();

#endif
