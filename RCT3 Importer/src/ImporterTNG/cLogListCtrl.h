///////////////////////////////////////////////////////////////////////////////
//
// RCT3 Importer TNG
// Next generation RCT3 Importer
// Copyright (C) 2008 Tobias Minch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, <http://www.gnu.org/licenses/>.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CLOGLISTCTRL_H
#define CLOGLISTCTRL_H

#include <wx/listctrl.h>
#include <wx/log.h>
#include <sigc++/trackable.h>
#include <vector>

struct cLogEntry {
    wxLogLevel level;
    wxString message;
    wxString source;
    time_t timestamp;

    cLogEntry(wxLogLevel _level, const wxChar *_msg, const wxChar* _source, time_t _timestamp):
        level(_level),
        message(_msg),
        source(_source),
        timestamp(_timestamp)
    {}
};

class cLogListCtrl : public wxListCtrl, public wxLog, public sigc::trackable {
public:
    cLogListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
    virtual ~cLogListCtrl();

    virtual void DoLog(wxLogLevel level, const wxChar *msg, time_t timestamp) { DoLogSpecial(level, msg, wxT(""), timestamp); }
    virtual void DoLogSpecial(wxLogLevel level, const wxChar *msg, const wxChar* source, time_t timestamp);
    virtual void DoLogString(const wxChar *msg, time_t timestamp) { /*DoLogSpecial(wxLOG_Info, msg, wxT(""), timestamp);*/ }

    void Clear() { m_log.clear(); Update(); }
protected:
private:
    wxLog* m_oldTarget;
    wxListItemAttr m_liaInfo;
    wxListItemAttr m_liaWarning;
    wxListItemAttr m_liaError;
    wxImageList m_images;
    std::vector<cLogEntry> m_log;

    void Update();

    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemColumnImage(long item, long column) const;
    virtual wxListItemAttr* OnGetItemAttr(long item) const;
};

#endif // CLOGLISTCTRL_H
