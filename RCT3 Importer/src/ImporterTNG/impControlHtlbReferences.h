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

#ifndef IMPCONTROLHTLBREFERENCES_H
#define IMPCONTROLHTLBREFERENCES_H

#include "impControlHtlb.h"


class impControlHtlbReferences : public impControlHtlb {
public:
    impControlHtlbReferences(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n=0, const wxString choices[]=NULL, long style=0);
    virtual ~impControlHtlbReferences();
protected:
    virtual wxString onGetItem(xmlcpp::cXmlNode& item) const;
private:
};

#endif // IMPCONTROLHTLBREFERENCES_H
