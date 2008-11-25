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

#include "ManagerCID.h"

#define RAWXML_CID_SHAPE            "shape"
#define RAWXML_CID_MORE             "more"
#define RAWXML_CID_EXTRA            "extra"
#define RAWXML_CID_SETTINGS         "settings"
#define RAWXML_CID_TRASH            "trash"
#define RAWXML_CID_SOAKED           "soaked"
#define RAWXML_CID_SIZEUNK          "priceUnknowns"
#define RAWXML_CID_UNK              "unknowns"

void cRawParser::ParseCID(cXmlNode& node) {
    USE_PREFIX(node);
    cCarriedItem cid;
    cid.name = ParseString(node, wxT(RAWXML_CID), wxT("name"), NULL, useprefix).ToAscii();
    cid.nametxt = ParseString(node, wxT(RAWXML_CID), wxT("nametxt"), NULL, useprefix).ToAscii();
    cid.pluralnametxt = ParseString(node, wxT(RAWXML_CID), wxT("pluralnametxt"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding cid %s to %s."), wxString(cid.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_CID_SHAPE)) {
            USE_PREFIX(child);
            cid.shape.shaped = ParseUnsigned(child, wxT(RAWXML_CID_SHAPE), wxT("shaped"));
            if (cid.shape.shaped) {
                cid.shape.shape1 = ParseString(child, wxT(RAWXML_CID_SHAPE), wxT("shape1"), NULL, useprefix).ToAscii();
                cid.shape.shape2 = ParseString(child, wxT(RAWXML_CID_SHAPE), wxT("shape1"), NULL, useprefix).ToAscii();
            } else {
                cid.shape.MakeBillboard();
                cid.shape.fts = ParseString(child, wxT(RAWXML_CID_SHAPE), wxT("ftx"), NULL, useprefix).ToAscii();
            }
            OPTION_PARSE(float, cid.shape.unk9, ParseFloat(child, wxT(RAWXML_CID_SHAPE), wxT("distance")));
            OPTION_PARSE(unsigned long, cid.shape.defaultcolour, ParseUnsigned(child, wxT(RAWXML_CID_SHAPE), wxT("defaultcolour")));
            OPTION_PARSE(float, cid.shape.scalex, ParseFloat(child, wxT(RAWXML_CID_SHAPE), wxT("scalex")));
            OPTION_PARSE(float, cid.shape.scaley, ParseFloat(child, wxT(RAWXML_CID_SHAPE), wxT("scaley")));
        } else if (child(RAWXML_CID_MORE)) {
            USE_PREFIX(child);
            OPTION_PARSE(unsigned long, cid.addonpack, ParseUnsigned(child, wxT(RAWXML_CID_MORE), wxT("addonpack")));
            ParseStringOption(cid.icon, child, wxT("icon"), NULL, useprefix);
        } else if (child(RAWXML_CID_EXTRA)) {
            USE_PREFIX(child);
            wxString extra = ParseString(child, wxT(RAWXML_CID_EXTRA), wxT("name"), NULL, useprefix);
            cid.extras.push_back(std::string(extra.ToAscii()));
        } else if (child(RAWXML_CID_SETTINGS)) {
            OPTION_PARSE(unsigned long, cid.settings.flags, ParseUnsigned(child, wxT(RAWXML_CID_SETTINGS), wxT("flags")));
            OPTION_PARSE(long, cid.settings.ageclass, ParseSigned(child, wxT(RAWXML_CID_SETTINGS), wxT("ageclass")));
            OPTION_PARSE(unsigned long, cid.settings.type, ParseUnsigned(child, wxT(RAWXML_CID_SETTINGS), wxT("type")));
            OPTION_PARSE(float, cid.settings.hunger, ParseFloat(child, wxT(RAWXML_CID_SETTINGS), wxT("hunger")));
            OPTION_PARSE(float, cid.settings.thirst, ParseFloat(child, wxT(RAWXML_CID_SETTINGS), wxT("thirst")));
            OPTION_PARSE(float, cid.settings.lightprotectionfactor, ParseFloat(child, wxT(RAWXML_CID_SETTINGS), wxT("lightprotectionfactor")));
        } else if (child(RAWXML_CID_TRASH)) {
            USE_PREFIX(child);
            ParseStringOption(cid.trash.wrapper, child, wxT("wrapper"), NULL, useprefix);
            OPTION_PARSE(float, cid.trash.trash1, ParseFloat(child, wxT(RAWXML_CID_TRASH), wxT("trash1")));
            OPTION_PARSE(long, cid.trash.trash2, ParseSigned(child, wxT(RAWXML_CID_TRASH), wxT("trash2")));
        } else if (child(RAWXML_CID_SOAKED)) {
            ParseStringOption(cid.soaked.male_morphmesh_body, child, wxT("malebody"), NULL, false);
            ParseStringOption(cid.soaked.male_morphmesh_legs, child, wxT("malelegs"), NULL, false);
            ParseStringOption(cid.soaked.female_morphmesh_body, child, wxT("femalebody"), NULL, false);
            ParseStringOption(cid.soaked.female_morphmesh_legs, child, wxT("femalelegs"), NULL, false);
            OPTION_PARSE(unsigned long, cid.soaked.unk38, ParseUnsigned(child, wxT(RAWXML_CID_SOAKED), wxT("unknown")));
            ParseStringOption(cid.soaked.textureoption, child, wxT("textureoption"), NULL, false);
        } else if (child(RAWXML_CID_SIZEUNK)) {
            OPTION_PARSE(unsigned long, cid.size.unk17, ParseUnsigned(child, wxT(RAWXML_CID_SIZEUNK), wxT("u1")));
            OPTION_PARSE(unsigned long, cid.size.unk18, ParseUnsigned(child, wxT(RAWXML_CID_SIZEUNK), wxT("u2")));
            OPTION_PARSE(unsigned long, cid.size.unk19, ParseUnsigned(child, wxT(RAWXML_CID_SIZEUNK), wxT("u3")));
            OPTION_PARSE(unsigned long, cid.size.unk20, ParseUnsigned(child, wxT(RAWXML_CID_SIZEUNK), wxT("u4")));
        } else if (child(RAWXML_CID_UNK)) {
            OPTION_PARSE(unsigned long, cid.unknowns.unk1, ParseUnsigned(child, wxT(RAWXML_CID_UNK), wxT("u1")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk3, ParseUnsigned(child, wxT(RAWXML_CID_UNK), wxT("u3")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk26, ParseUnsigned(child, wxT(RAWXML_CID_UNK), wxT("u26")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk27, ParseUnsigned(child, wxT(RAWXML_CID_UNK), wxT("u27")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk28, ParseUnsigned(child, wxT(RAWXML_CID_UNK), wxT("u28")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk33, ParseUnsigned(child, wxT(RAWXML_CID_UNK), wxT("u33")));
        } else if (child.element()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in cid tag"), child.wxname().c_str()), child);
        }
        child.go_next();
    }

    ovlCIDManager* c_cid = m_ovl.GetManager<ovlCIDManager>();
    c_cid->AddItem(cid);
}

