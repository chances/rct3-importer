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

#include "impViewReferences.h"
#include "impControlHtlbReferences.h"
#include "xmldefs.h"

using namespace xmlcpp;

impViewReferences::impViewReferences(wxWindow* parent, const wxString& path): rcpanReferences(parent) {
    initInfo(path);
    m_htlbReferences->init(*this, path + wxT("//r:" XML_TAG_REFERENCE), path);
}

impViewReferences::~impViewReferences()
{
    //dtor
}

wxString impViewReferences::getName() {
    return _("References");
}

void impViewReferences::update(const wxString& value) {
    update_controls(value);
    m_btReferenceClear->Enable(m_htlbReferences->GetItemCount());
    m_btReferenceDel->Enable(m_htlbReferences->GetSelection() != wxNOT_FOUND);
}

void impViewReferences::OnList( wxCommandEvent& WXUNUSED(event) ) {
    m_btReferenceDel->Enable(m_htlbReferences->GetSelection() != wxNOT_FOUND);
}

void impViewReferences::OnListDClick( wxCommandEvent& WXUNUSED(event) ) {
    int p = m_htlbReferences->GetSelection();
    if (p != wxNOT_FOUND) {
        m_textReference->ChangeValue(m_htlbReferences->getItemNode(p).wxcontent());
    }
    m_htlbReferences->SetSelectionBackground(wxColour(0xEE, 0xAA, 0xAA));
    m_htlbReferences->RefreshAll();
}

void impViewReferences::OnAdd( wxCommandEvent& WXUNUSED(event) ) {
    wxString ref = m_textReference->GetValue();
    if (!ref.IsEmpty()) {
        cXmlNode newref(XML_TAG_REFERENCE, ref.utf8_str(), getXml().defaultNs());
        newNode(getInfo().path, newref);
    }
}

void impViewReferences::OnDel( wxCommandEvent& WXUNUSED(event) ) {
    int p = m_htlbReferences->GetSelection();
    if (p != wxNOT_FOUND) {
        cXmlNode n = m_htlbReferences->getItemNode(p);
        deleteNode(getInfo().path, n);
    }
}

void impViewReferences::OnClear( wxCommandEvent& WXUNUSED(event) ) {
    impRawovlFileManager::UpdateFreezer fr = getFreezer();
    for (size_t i = m_htlbReferences->GetItemCount(); i; --i) {
        cXmlNode n = m_htlbReferences->getItemNode(i - 1);
        deleteNode(getInfo().path, n);
    }
}
