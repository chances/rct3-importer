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

#ifndef IMPORTERELEMENTS_H_INCLUDED
#define IMPORTERELEMENTS_H_INCLUDED

struct impElement;

#include <map>
#include <string>

#include "importerxpaths.h"
#include "impTypes.h"

struct impElement {
    impTypes::ELEMENTTYPE type;
    const impTypes::ELEMENTTYPE* children;
    const char* role;
    const char* element;
    const char* cattitle;
    const char* path;
    const char* childrenpath;
    const char* templatefile;
    impTypes::VIEWTYPE defaultview;
    impTypes::VIEWTYPE* views;
    impTypes::COMMAND defaultcommand;

    impElement():
        type(impTypes::ELEMENTTYPE_NONE), children(NULL), role(NULL), element(NULL), cattitle(NULL),
        path(NULL), childrenpath(NULL), templatefile(NULL), defaultview(impTypes::VIEWTYPE_ABSTRACT),
        views(NULL), defaultcommand(impTypes::COMMAND_NOTHING)
    {}
    impElement(impTypes::ELEMENTTYPE _type, const impTypes::ELEMENTTYPE* _children, const char* _role, const char* _element, const char* _cattitle,
        const char* _path, const char* _childrenpath, const char* _templatefile, impTypes::VIEWTYPE _defaultview,
        impTypes::VIEWTYPE* _views, impTypes::COMMAND _defaultcommand):
        type(_type), children(_children), role(_role), element(_element), cattitle(_cattitle),
        path(_path), childrenpath(_childrenpath), templatefile(_templatefile), defaultview(_defaultview),
        views(_views), defaultcommand(_defaultcommand)
    {}
};

extern std::map<impTypes::ELEMENTTYPE, impElement> impElements;
extern std::map<std::string, impTypes::ELEMENTTYPE> impElementRoles;

void InitElements();

#endif // IMPORTERELEMENTS_H_INCLUDED
