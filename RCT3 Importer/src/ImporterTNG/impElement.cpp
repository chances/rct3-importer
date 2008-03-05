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

#include "impElement.h"

#include "importerresourcefiles.h"

using namespace std;

map<impTypes::ELEMENTTYPE, impElement> impElements;
map<string, impTypes::ELEMENTTYPE> impElementRoles;

impTypes::ELEMENTTYPE impElementChildrenProject[] = {
    impTypes::ELEMENTTYPE_CATEGORY_SCENERY,
    impTypes::ELEMENTTYPE_CATEGORY_TEXTURE,
    impTypes::ELEMENTTYPE_CATEGORY_ALL,
    impTypes::ELEMENTTYPE_NONE
};

impElement impElementsRaw[] = {
    impElement(impTypes::ELEMENTTYPE_NONE, NULL, NULL, NULL, NULL, NULL, NULL, NULL, impTypes::VIEWTYPE_ABSTRACT, NULL, impTypes::COMMAND_NOTHING),
    impElement(impTypes::ELEMENTTYPE_PROJECT,
        impElementChildrenProject,
        "project",
        NULL,
        NULL,
        "//r:" XML_TAG_OVL "[@role='project']",
        NULL,
        MEMORY_PREFIX RES_TEMPLATE_PROJECT,
        impTypes::VIEWTYPE_PROJECT,
        NULL,
        impTypes::COMMAND_VIEW
    ),
    impElement(impTypes::ELEMENTTYPE_CATEGORY_SCENERY,
        NULL,
        NULL,
        NULL,
        wxTRANSLATE("Scenery OVLs"),
        NULL,
        "//r:" XML_TAG_OVL "[@role='scenery']",
        NULL,
        impTypes::VIEWTYPE_ABSTRACT,
        NULL,
        impTypes::COMMAND_NEW
    ),
    impElement(impTypes::ELEMENTTYPE_CATEGORY_TEXTURE,
        NULL,
        NULL,
        NULL,
        wxTRANSLATE("Texture OVLs"),
        NULL,
        "//r:" XML_TAG_OVL "[@role='texture']",
        NULL,
        impTypes::VIEWTYPE_ABSTRACT,
        NULL,
        impTypes::COMMAND_NEW
    ),
    impElement(impTypes::ELEMENTTYPE_CATEGORY_ALL,
        NULL,
        NULL,
        NULL,
        wxTRANSLATE("All"),
        NULL,
        "//r:" XML_TAG_OVL "[@role and @role != 'project']",
        NULL,
        impTypes::VIEWTYPE_ABSTRACT,
        NULL,
        impTypes::COMMAND_NOTHING
    ),
    impElement(impTypes::ELEMENTTYPE_OVL_SCENERY,
        NULL,
        "scenery",
        NULL,
        NULL,
        "//r:" XML_TAG_OVL "[@role='scenery']",
        NULL,
        NULL,
        impTypes::VIEWTYPE_REFERENCES,
        NULL,
        impTypes::COMMAND_VIEW
    ),
    impElement(impTypes::ELEMENTTYPE_OVL_TEXTURE,
        NULL,
        "texture",
        NULL,
        NULL,
        "//r:" XML_TAG_OVL "[@role='texture']",
        NULL,
        NULL,
        impTypes::VIEWTYPE_ABSTRACT,
        NULL,
        impTypes::COMMAND_VIEW
    )
};

void InitElements() {
    for (int i = 0; i < impTypes::ELEMENTTYPE_END; ++i) {
        impElements[impElementsRaw[i].type] = impElementsRaw[i];
        if (impElementsRaw[i].role) {
            impElementRoles[impElementsRaw[i].role] = impElementsRaw[i].type;
        }
    }

}
