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

#include "cLogListCtrl.h"

#include "gximage.h"
#include "importerresourcefiles.h"
#include "impRawovlFileManager.h"
#include "importerapp.h"

cLogListCtrl::cLogListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size):
        wxListCtrl(parent, id, pos, size, wxLC_REPORT|wxLC_VIRTUAL|wxLC_NO_HEADER),
        m_images(16, 16, true),
        m_liaInfo(*wxBLACK, wxNullColour, wxNullFont),
        m_liaWarning(*wxBLACK, wxColour(255, 255, 0), wxNullFont),
        m_liaError(*wxBLACK, *wxRED, wxNullFont) {
    InsertColumn(0, wxT(""));
    InsertColumn(1, wxT(""));
    m_images.Add(wxGXImage(wxT(MEMORY_PREFIX RES_IMAGE_INFO), false).GetImage());
    m_images.Add(wxGXImage(wxT(MEMORY_PREFIX RES_IMAGE_WARNING), false).GetImage());
    m_images.Add(wxGXImage(wxT(MEMORY_PREFIX RES_IMAGE_ERROR), false).GetImage());
    AssignImageList(&m_images, wxIMAGE_LIST_SMALL);
    m_oldTarget = wxLog::SetActiveTarget(this);
    impRawovlFileManager::getManager().log.connect(sigc::mem_fun(*this, &cLogListCtrl::DoLogSpecial));
}

cLogListCtrl::~cLogListCtrl() {
    wxLog::SetActiveTarget(m_oldTarget);
}

void cLogListCtrl::DoLogSpecial(wxLogLevel level, const wxChar *msg, const wxChar* source, time_t timestamp) {
    if (level == wxLOG_Status) {
        wxFrame* fr = dynamic_cast<wxFrame*>(wxGetApp().GetTopWindow());
        if (fr) {
            wxStatusBar* b = fr->GetStatusBar();
            if (b) {
                b->SetStatusText(msg);
            }
        }
    } else
        m_log.push_back(cLogEntry(level, msg, source, timestamp));
    Update();
}

void cLogListCtrl::Update() {
    SetItemCount(m_log.size());
    SetColumnWidth(1, -1);
    if (m_log.size()) {
        wxRect r;
        GetItemRect(0, r);
        ScrollList(0, r.GetHeight()*m_log.size());
    }
}

wxString cLogListCtrl::OnGetItemText(long item, long column) const {
    if (column) {
        return m_log[item].message;
    } else {
        switch (m_log[item].level) {
            case wxLOG_Warning: return _("Warning");
            case wxLOG_Error: return _("Error");
            default: return _("Info");
        }
    }
}

int cLogListCtrl::OnGetItemColumnImage(long item, long column) const {
    if (column)
        return -1;

    switch (m_log[item].level) {
        case wxLOG_Warning: return 1;
        case wxLOG_Error: return 2;
        default: return 0;
    }

}

wxListItemAttr* cLogListCtrl::OnGetItemAttr(long item) const {
    switch (m_log[item].level) {
        case wxLOG_Warning: return const_cast<wxListItemAttr*>(&m_liaWarning);
        case wxLOG_Error: return const_cast<wxListItemAttr*>(&m_liaError);
        default: return const_cast<wxListItemAttr*>(&m_liaInfo);
    }
}
