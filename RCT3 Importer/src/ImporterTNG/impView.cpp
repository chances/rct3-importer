///////////////////////////////////////////////////////////////////////////////
//
// RCT3 Importer TNG
// Custom object importer for Ataris Roller Coaster Tycoon 3
// Copyright (C) 2008 Belgabor (Tobias Minich)
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
//   Belgabor (Tobias Minich) - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////


#include "impView.h"

#include "impViewProject.h"
#include "impViewReferences.h"

using namespace impTypes;

void impView::initInfo(const wxString& path_) {
    if (!m_info.type) {
        m_info.type = getType();
        m_info.path = path_;
        m_info.wnd = dynamic_cast<wxWindow*>(this);
    }
}

void impView::update(const wxString& value) {
    updateNotebookPage();
}

void impView::updateNotebookPage() {
    if (m_nb) {
        m_nb->SetPageText(m_page, getName());
    }
}

void impView::setNotebook(wxAuiNotebook* nb) {
    wxWindow* me = dynamic_cast<wxWindow*>(this);
    if (nb && me) {
        m_nb = nb;
        m_page = nb->GetPageIndex(me);
        updateNotebookPage();
    }
}

impView* impView::createView(wxWindow* parent, VIEWTYPE type, const wxString& path) {
    switch (type) {
    case VIEWTYPE_PROJECT:
        return new impViewProject(parent);
    case VIEWTYPE_REFERENCES:
        return new impViewReferences(parent, path);
    default:
        wxLogError(_("Tried to create unimplemented view type"));
    }
    return NULL;
}

