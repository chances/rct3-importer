///////////////////////////////////////////////////////////////////////////////
//
// raw ovl xml interpreter
// Command line ovl creation utility
// Copyright (C) 2007 Tobias Minch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////



#include "RawParse_cpp.h"

//#include <boost/algorithm/string.hpp>
//#include <boost/tokenizer.hpp>

#include "ManagerWAI.h"

#define RAWXML_WAI_PARAMETERS       "parameters"
#define RAWXML_WAI_UNKNOWNS         "unknowns"

void cRawParser::ParseWAI(cXmlNode& node) {
    USE_PREFIX(node);
    cWildAnimalItem wai;
    wxString name = ParseString(node, wxT(RAWXML_WAI), wxT("name"), NULL, useprefix);
    wai.name = name.ToAscii();
    wai.shortname = ParseString(node, wxT(RAWXML_WAI), wxT("shortname"), NULL).ToAscii();
    wai.nametxt = ParseString(node, wxT(RAWXML_WAI), wxT("nametxt"), NULL, useprefix).ToAscii();
    wai.description = ParseString(node, wxT(RAWXML_WAI), wxT("description"), NULL, useprefix).ToAscii();
    wai.icon = ParseString(node, wxT(RAWXML_WAI), wxT("icon"), NULL, useprefix).ToAscii();
    wai.staticshape = ParseString(node, wxT(RAWXML_WAI), wxT("staticshape"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding wai %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_WAI_PARAMETERS)) {
            OPTION_PARSE(float, wai.distance, ParseFloat(child, wxT(RAWXML_WAI_PARAMETERS), wxT("distance")));
            OPTION_PARSE(long, wai.cost, ParseSigned(child, wxT(RAWXML_WAI_PARAMETERS), wxT("cost")));
            OPTION_PARSE(long, wai.refund, ParseSigned(child, wxT(RAWXML_WAI_PARAMETERS), wxT("refund")));
            OPTION_PARSE(float, wai.invdurability, ParseFloat(child, wxT(RAWXML_WAI_PARAMETERS), wxT("inversedurability")));
            OPTION_PARSE(float, wai.x_size, ParseFloat(child, wxT(RAWXML_WAI_PARAMETERS), wxT("xsize")));
            OPTION_PARSE(float, wai.y_size, ParseFloat(child, wxT(RAWXML_WAI_PARAMETERS), wxT("ysize")));
            OPTION_PARSE(float, wai.z_size, ParseFloat(child, wxT(RAWXML_WAI_PARAMETERS), wxT("zsize")));
            OPTION_PARSE(unsigned long, wai.flags, ParseUnsigned(child, wxT(RAWXML_WAI_PARAMETERS), wxT("flags")));
            OPTION_PARSE(float, wai.weight, ParseFloat(child, wxT(RAWXML_WAI_PARAMETERS), wxT("weight")));
        } else if (child(RAWXML_WAI_UNKNOWNS)) {
            OPTION_PARSE(unsigned long, wai.unk1, ParseUnsigned(child, wxT(RAWXML_WAI_UNKNOWNS), wxT("u1")));
            OPTION_PARSE(unsigned long, wai.unk2, ParseUnsigned(child, wxT(RAWXML_WAI_UNKNOWNS), wxT("u2")));
            OPTION_PARSE(unsigned long, wai.unk3, ParseUnsigned(child, wxT(RAWXML_WAI_UNKNOWNS), wxT("u3")));
            OPTION_PARSE(unsigned long, wai.unk4, ParseUnsigned(child, wxT(RAWXML_WAI_UNKNOWNS), wxT("u4")));
            OPTION_PARSE(unsigned long, wai.unk11, ParseUnsigned(child, wxT(RAWXML_WAI_UNKNOWNS), wxT("u11")));
            OPTION_PARSE(float, wai.unk17, ParseFloat(child, wxT(RAWXML_WAI_UNKNOWNS), wxT("u17")));
            OPTION_PARSE(float, wai.unk18, ParseFloat(child, wxT(RAWXML_WAI_UNKNOWNS), wxT("u18")));
            OPTION_PARSE(float, wai.unk19, ParseFloat(child, wxT(RAWXML_WAI_UNKNOWNS), wxT("u19")));
            OPTION_PARSE(float, wai.unk20, ParseFloat(child, wxT(RAWXML_WAI_UNKNOWNS), wxT("u20")));
        } else if (child.element()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in wai tag '%s'"), child.wxname().c_str(), name.c_str()), child);
        }

        child.go_next();
    }

    ovlWAIManager* c_wai = m_ovl.GetManager<ovlWAIManager>();
    c_wai->AddItem(wai);
}

