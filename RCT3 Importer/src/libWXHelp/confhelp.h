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

#include "wx_pch.h"

#include <wx/confbase.h>
#include <d3d9types.h>

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
D3DMATRIX wxcRead(const wxString& path, const D3DMATRIX& def);

template <class T>
bool wxcWrite(const wxString& path, const T& val) {
    wxConfigBase* conf = wxConfigBase::Get(false);
    if (!conf)
        return false;
    return conf->Write(path, val);
}

template <>
bool wxcWrite(const wxString& path, const D3DMATRIX& val);

bool wxcFlush();

#endif
