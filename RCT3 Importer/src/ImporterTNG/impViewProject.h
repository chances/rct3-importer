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

#ifndef IMPVIEWPROJECT_H
#define IMPVIEWPROJECT_H

#include "resources/res_viewproject.h"
#include "impView.h"

class impViewProject : public rcpanViewProject, public impView {
public: // Slots
    virtual void update(const wxString& value);

public:
    impViewProject(wxWindow* parent);
    virtual ~impViewProject();

    inline virtual impTypes::VIEWTYPE getType() { return impTypes::VIEWTYPE_PROJECT; }
    virtual wxString getName();
protected:
private:
};

#endif // IMPVIEWPROJECT_H
