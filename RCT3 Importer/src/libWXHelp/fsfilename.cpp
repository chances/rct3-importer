/////////////////////////////////////////////////////////////////////////////
// Name:        fsfilename.cpp
// Purpose:     wxFSFileName class
// Author:      Tobias Minich
// Modified by:
// Created:     2008-01-09
// RCS-ID:
// Copyright:   (c) Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "fsfilename.h"

#include <wx/tokenzr.h>

void wxFSFileNameComponent::Assign(const wxString& filename, bool isdir) {
    if ((filename.Find(wxT(':')) == wxNOT_FOUND) || (filename.BeforeFirst(wxT(':')).Length() == 1)){
        m_protocol = wxT("file");
        if (isdir)
            m_filename = wxFileName::DirName(filename);
        else
            m_filename = filename;
    } else {
        m_protocol = filename.BeforeFirst(wxT(':'));
        if (isdir)
            m_filename = wxFileName::DirName(filename.AfterFirst(wxT(':')));
        else
            m_filename = filename.AfterFirst(wxT(':'));
    }
}

bool wxFSFileName::SameAs(const wxFSFileName& query) const {
    if (m_components.size() != query.m_components.size())
        return false;
    for (int i = 0; i < static_cast<int>(m_components.size()); ++i) {
        if (m_components[i] != query.m_components[i])
            return false;
    }
    return true;
}

bool wxFSFileName::SameRootAs(const wxFSFileName& query) const {
    if (m_components.size() != query.m_components.size())
        return false;
    for (int i = 0; i < static_cast<int>(m_components.size()) - 1; ++i) {
        if (m_components[i] != query.m_components[i])
            return false;
    }
    if (m_components[m_components.size()-1].GetProtocol() != query.m_components[m_components.size()-1].GetProtocol())
        return false;

    return true;
}

void wxFSFileName::Assign(const wxString& filename, bool isdir) {
    m_components.clear();
    if (filename.Find(wxT('#')) == wxNOT_FOUND) {
        m_components.push_back(wxFSFileNameComponent(filename, isdir));
    } else {
        wxArrayString temp;
        wxStringTokenizer tok(filename, wxT("#"));
        while (tok.HasMoreTokens()) {
            wxString t = tok.GetNextToken();
            temp.push_back(t);
        }
        for (int i = 0; i < static_cast<int>(temp.size()) - 1; ++i) {
            m_components.push_back(wxFSFileNameComponent(temp[i]));
        }
        m_components.push_back(wxFSFileNameComponent(temp[temp.size()-1], isdir));
    }
}

wxString wxFSFileName::GetBasePath(int flags, wxPathFormat format) const {
    return m_components[0].GetPath(flags, format);
}

wxString wxFSFileName::GetPath(int flags, wxPathFormat format) const {
    wxString ret;
    for (int i = 0; i < static_cast<int>(m_components.size()) - 1; ++i) {
        ret += m_components[i].GetFullPath(format) + wxT("#");
    }
    ret += m_components[m_components.size() - 1].GetPath(flags, format);
    return ret;
}

wxString wxFSFileName::GetFullPath(wxPathFormat format) const {
    if (m_components.size() > 1) {
        wxString ret;
        for (int i = 0; i < static_cast<int>(m_components.size()) - 1; ++i) {
            ret += m_components[i].GetFullPath(format) + wxT("#");
        }
        ret += m_components[m_components.size() - 1].GetFullPath(format);
        return ret;
    } else {
        if (m_components[0].GetProtocol() == wxT("file"))
            return m_components[0].GetFileName().GetFullPath(format);
        else
            return m_components[0].GetFullPath(format);
    }
}

bool wxFSFileName::IsAbsolute(wxPathFormat format) const {
    // Absoluteness is kinda debatable in a FS environment.
    if (m_components.size() > 1)
        return true;
    return m_components[0].GetFileName().IsAbsolute(format);
}

bool wxFSFileName::MakeAbsolute(const wxString& cwd) {
    wxFSFileName temp(cwd);
    if (IsRelative()) {
        if ((m_components.size() == 1) && cwd.IsEmpty()) {
            return GetLastComponent().GetFileName().MakeAbsolute();
        }
        wxFSFileNameComponent me = m_components[0];
        wxString tempstr = temp.GetLastComponent().GetFileName().GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) + me.GetFileName().GetFullPath();
//        bool ret = me.GetFileName().MakeAbsolute(temp.GetLastComponent().GetFileName().GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
        m_components = temp.m_components;
        GetLastComponent().GetFileName() = tempstr;
        return GetLastComponent().GetFileName().Normalize(wxPATH_NORM_DOTS);
//        return ret;
    } else {
        if (!SameRootAs(cwd))
            return false;
        return GetLastComponent().GetFileName().MakeAbsolute(temp.GetLastComponent().GetFileName().GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
    }
}

bool wxFSFileName::MakeRelativeTo(const wxString& pathBase) {
    if (IsRelative()) {
        if (!MakeAbsolute(pathBase))
            return false;
    }
    wxFSFileName temp(pathBase);
    if (!SameRootAs(temp))
        return false;

    wxFSFileNameComponent t = GetLastComponent();
    bool ret = t.GetFileName().MakeRelativeTo(temp.GetLastComponent().GetFileName().GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
    m_components.clear();
    m_components.push_back(t);
    return ret;
}

