/////////////////////////////////////////////////////////////////////////////
// Name:        confhelp.cpp
// Purpose:     Function templates to help wonking with wxConfig
// Author:      Tobias Minich
// Modified by:
// Created:     2007-11-13
// RCS-ID:
// Copyright:   (c) Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "confhelp.h"

template <>
r3::MATRIX wxcRead(const wxString& path, const r3::MATRIX& def) {
    wxConfigBase* conf = wxConfigBase::Get(false);
    if (!conf)
        return def;
    r3::MATRIX val;
    double v, d;
    for (int x = 0; x < 4; ++x) {
        for (int y = 0; y < 4; ++y) {
            d = def.m[x][y];
            if (conf->Read(path + wxString::Format(wxT("_%d_%d"), x, y), &v, d))
                val.m[x][y] = v;
            else
                return def;
        }
    }
    return val;
}

template <>
bool wxcWrite(const wxString& path, const r3::MATRIX& val) {
    wxConfigBase* conf = wxConfigBase::Get(false);
    if (!conf)
        return false;
    for (int x = 0; x < 4; ++x) {
        for (int y = 0; y < 4; ++y) {
            if (!conf->Write(path + wxString::Format(wxT("_%d_%d"), x, y), val.m[x][y]))
                return false;
        }
    }
    return true;
}

#if wxUSE_IMAGE
template <>
wxSize wxcRead(const wxString& path, const wxSize& def) {
    wxConfigBase* conf = wxConfigBase::Get(false);
    if (!conf)
        return def;
    wxSize val = def;
    int i;
    if (conf->Read(path + wxT("_w"), &i, def.GetWidth()))
        val.SetWidth(i);
    if (conf->Read(path + wxT("_h"), &i, def.GetHeight()))
        val.SetHeight(i);
    return val;
}

template <>
bool wxcWrite(const wxString& path, const wxSize& val) {
    wxConfigBase* conf = wxConfigBase::Get(false);
    if (!conf)
        return false;
    if (!conf->Write(path + wxT("_w"), val.GetWidth()))
        return false;
    if (!conf->Write(path + wxT("_h"), val.GetHeight()))
        return false;
    return true;
}

bool wxcFlush() {
    wxConfigBase* conf = wxConfigBase::Get(false);
    if (conf)
        return conf->Flush();
    else
        return false;
}
#endif
