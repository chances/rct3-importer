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

#ifndef IMPVIEWREFERENCES_H
#define IMPVIEWREFERENCES_H

#include "resources/res_viewreferences.h"
#include "impView.h"


class impViewReferences: public rcpanReferences, public impView {
public: // Slots
    virtual void update(const wxString& value);

public:
    impViewReferences(wxWindow* parent, const wxString& path);
    virtual ~impViewReferences();

    inline virtual impTypes::VIEWTYPE getType() { return impTypes::VIEWTYPE_REFERENCES; }
    virtual wxString getName();
protected:
    virtual void OnList( wxCommandEvent& event );
    virtual void OnListDClick( wxCommandEvent& event );
    virtual void OnAdd( wxCommandEvent& event );
    virtual void OnDel( wxCommandEvent& event );
    virtual void OnClear( wxCommandEvent& event );
private:
};

#endif // IMPVIEWREFERENCES_H
