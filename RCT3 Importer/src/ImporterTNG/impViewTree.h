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

#ifndef IMPVIEWTREE_H
#define IMPVIEWTREE_H

#include <wx/dataview.h>

#include "impView.h"

class impViewTreeModel;
class impViewTree : public impView, public wxDataViewCtrl {
public:
    impViewTree(wxWindow* parent);
    virtual ~impViewTree();

    inline virtual impTypes::VIEWTYPE getType() { return impTypes::VIEWTYPE_TREE; }
    virtual wxString getName();
public: // Slots
    virtual void update(const wxString& value);
protected:
private:
    wxObjectDataPtr<impViewTreeModel> m_model;
    wxDataViewColumn* m_treecolumn;

    void OnResize(wxSizeEvent& event);
    void OnActivate(wxDataViewEvent& event);
};

#endif // IMPVIEWTREE_H
