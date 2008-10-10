/////////////////////////////////////////////////////////////////////////////
// Name:        fsfilename.h
// Purpose:     wxFSFileName class
// Author:      Tobias Minich
// Modified by:
// Created:     2008-01-09
// RCS-ID:
// Copyright:   (c) Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef FSFILENAME_H_INCLUDED
#define FSFILENAME_H_INCLUDED

#include <wx/filename.h>
#include <vector>

class wxFSFileNameComponent {
private:
    wxString m_protocol;
    wxFileName m_filename;

    wxString Make(const wxString& right) const {
        return m_protocol + wxT(":") + right;
    }
    bool SameAs(const wxFSFileNameComponent& query) const {
        return ((m_protocol == query.m_protocol) && (m_filename == query.m_filename));
    }
public:
    wxFSFileNameComponent() {
        Assign(wxT("file"), wxT(""));
    }
    wxFSFileNameComponent(const wxString& filename, bool isdir = false) {
        Assign(filename, isdir);
    }
    wxFSFileNameComponent(const wxString& protocol, const wxString& filename) {
        Assign(protocol, filename);
    }
    wxFSFileNameComponent(const wxString& protocol, const wxFileName& filename) {
        Assign(protocol, filename);
    }

    void Assign(const wxString& filename, bool isdir = false);
    void Assign(const wxString& protocol, const wxString& filename) {
        m_protocol = protocol;
        m_filename = filename;
    }
    void Assign(const wxString& protocol, const wxFileName& filename) {
        m_protocol = protocol;
        m_filename = filename;
    }

    bool operator==(const wxFSFileNameComponent& filename) const
        { return SameAs(filename); }
    bool operator!=(const wxFSFileNameComponent& filename) const
        { return !SameAs(filename); }

    wxString& GetProtocol() {
        return m_protocol;
    }
    const wxString& GetProtocol() const {
        return m_protocol;
    }
    wxFileName& GetFileName() {
        return m_filename;
    }
    const wxFileName& GetFileName() const {
        return m_filename;
    }
    wxString GetPath(int flags = wxPATH_GET_VOLUME, wxPathFormat format = wxPATH_NATIVE) const {
        return Make(m_filename.GetPath(flags, format));
    }
    wxString GetFullPath( wxPathFormat format = wxPATH_NATIVE ) const {
        return Make(m_filename.GetFullPath(format));
    }
    wxString GetExt() const                     { return m_filename.GetExt(); }
    wxString GetName() const                    { return m_filename.GetName(); }
    wxString GetFullName() const                { return m_filename.GetFullName(); }
};

class wxFSFileName {
private:
    std::vector<wxFSFileNameComponent> m_components;

    bool SameAs(const wxFSFileName& query) const;
    bool SameRootAs(const wxFSFileName& query) const; // Compares components before last and last protocol
public:
    wxFSFileName() {
        Assign(wxT(""));
    }
    wxFSFileName(const wxString& filename, bool isdir = false) {
        Assign(filename, isdir);
    }

    void Assign(const wxString& filename, bool isdir = false);

    bool operator==(const wxFSFileName& filename) const
        { return SameAs(filename); }
    bool operator!=(const wxFSFileName& filename) const
        { return !SameAs(filename); }
    bool operator==(const wxString& filename) const
        { return SameAs(wxFSFileName(filename)); }
    bool operator!=(const wxString& filename) const
        { return !SameAs(wxFSFileName(filename)); }

    int GetComponentCount() const               { return m_components.size(); }
    const wxFSFileNameComponent& GetComponent(int n) const {
        return m_components[n];
    }
    wxFSFileNameComponent& GetComponent(int n)  { return m_components[n]; }
    const wxFSFileNameComponent& GetLastComponent() const {
        return m_components[m_components.size()-1];
    }
    wxFSFileNameComponent& GetLastComponent()   { return m_components[m_components.size()-1]; }
    wxString GetBasePath(int flags = wxPATH_GET_VOLUME, wxPathFormat format = wxPATH_NATIVE) const;
    wxString GetPath(int flags = wxPATH_GET_VOLUME, wxPathFormat format = wxPATH_NATIVE) const;
    wxString GetFullPath( wxPathFormat format = wxPATH_NATIVE ) const;

    bool IsAbsolute(wxPathFormat format = wxPATH_NATIVE) const;
    bool IsRelative(wxPathFormat format = wxPATH_NATIVE) const
        { return !IsAbsolute(format); }
    bool MakeAbsolute(const wxString& cwd = wxEmptyString);
    bool MakeRelativeTo(const wxString& pathBase);

    wxString GetExt() const                     { return GetLastComponent().GetExt(); }
    wxString GetName() const                    { return GetLastComponent().GetName(); }
    wxString GetFullName() const                { return GetLastComponent().GetFullName(); }
};

#endif
