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

#include "impControlHtlb.h"

#include "cXmlXPath.h"
#include "cXmlXPathResult.h"

using namespace xmlcpp;

impControlHtlb::impControlHtlb(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int n, const wxString choices[], long style):
    wxColourHtmlListBox(parent, id, pos, size, style) {
    //ctor
}

impControlHtlb::~impControlHtlb() {
    //dtor
}

void impControlHtlb::update(const wxString& value) {
    updateContents();
}

void impControlHtlb::updateContents() {
    cXmlXPath p = getXPath();
    if (p) {
        cXmlXPathResult res = p(m_path.utf8_str());
        SetItemCount(res.size());
        size_t sel = GetSelection();
        if (sel >= res.size())
            sel = wxNOT_FOUND;
        RefreshAll();
        SetSelection(sel);
    } else {
        wxLogError(wxT("PANIC! XPath slot unconnected"));
    }

}

wxString impControlHtlb::OnGetItem(size_t n) const {
    cXmlNode r =  getItemNode(n);
    if (r)
        return onGetItem(r);
    else
        return wxT("PANIC! Item disappeared.");
}

cXmlNode impControlHtlb::getItemNode(size_t n) const {
    cXmlXPath p = getXPath();
    if (p) {
        wxString path = m_path;
        path += wxString::Format(wxT("[%d]"), n+1);
        cXmlXPathResult res = p(path.utf8_str());
        if (res.size()) {
            return res[0];
        } else {
            wxLogError("PANIC! Item disappeared.");
            return cXmlNode();
        }
    } else {
        wxLogError("PANIC! XPath slot unconnected");
        return cXmlNode();
    }
}
