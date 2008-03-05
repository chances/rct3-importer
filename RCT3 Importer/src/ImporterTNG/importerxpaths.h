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

#ifndef IMPORTERXPATHS_H_INCLUDED
#define IMPORTERXPATHS_H_INCLUDED

#include "xmldefs.h"

#define XPATH_ANY_METADATA          "r:" XML_TAG_METADATA "[@role='importer']"
#define XPATH_ANY_NAME              XPATH_ANY_METADATA "/r:" XML_TAG_METADATA_NAME
#define XPATH_ANY_GUID              XPATH_ANY_METADATA "/r:" XML_TAG_METADATA_GUID
#define XPATH_ANY_FILE              "@file"
#define XPATH_PROJECT_NAME          "/r:" XML_TAG_OVL "[@role='project']/" XPATH_ANY_NAME
#define XPATH_PROJECT_METADATA      "/r:" XML_TAG_OVL "[@role='project']/" XPATH_ANY_METADATA

#define XPATH_FORMAT_OVLID_NAME "//r:" XML_TAG_OVL "[@role='%s' and r:" XML_TAG_METADATA "[@role='importer']/r:" XML_TAG_METADATA_NAME "='%s']"
#define XPATH_FORMAT_OVLID_GUID "//r:" XML_TAG_OVL "[@role='%s' and r:" XML_TAG_METADATA "[@role='importer']/r:" XML_TAG_METADATA_GUID "='%s']"
#define XPATH_FORMAT_OVLID_FILE "//r:" XML_TAG_OVL "[@role='%s' and @file='%s']"

#endif // IMPORTERXPATHS_H_INCLUDED
