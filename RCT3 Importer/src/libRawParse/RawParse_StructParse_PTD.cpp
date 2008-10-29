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

#include "ManagerPTD.h"

#define DO_PTD(a,b) \
        } else if (cname.IsSameAs(wxT(RAWXML_PTD_ ## a))) { \
            wxString c = child.wxcontent(); \
            MakeVariable(c); \
            ptd.b[0] = c.ToAscii();

#define DO_PTD_TURN(a, b) DO_PTD(TURN_ ## a, turn_ ## b)
#define DO_PTD_CORNER(a, b) DO_PTD(CORNER_ ## a, corner_ ## b)
#define DO_PTD_COMPLEX(a, c) \
    DO_PTD(a, c.b) \
    DO_PTD(a ## _FC, c.fc) \
    DO_PTD(a ## _BC, c.bc) \
    DO_PTD(a ## _TC, c.tc)

void cRawParser::ParsePTD(cXmlNode& node) {
    USE_PREFIX(node);

    cPath ptd;
    wxString name = ParseString(node, wxT(RAWXML_PTD), wxT("name"), NULL, useprefix);
    ptd.name = name.ToAscii();
    ParseStringOption(ptd.internalname, node, wxT("internalname"), NULL, useprefix);
    ptd.nametxt = ParseString(node, wxT(RAWXML_PTD), wxT("nametxt"), NULL, useprefix).ToAscii();
    ptd.icon = ParseString(node, wxT(RAWXML_PTD), wxT("icon"), NULL, useprefix).ToAscii();
    unsigned long underwater = 0;
    unsigned long extended = 0;
    OPTION_PARSE(unsigned long, underwater, ParseUnsigned(node, wxT(RAWXML_PTD), wxT("underwater")));
    OPTION_PARSE(unsigned long, extended, ParseUnsigned(node, wxT(RAWXML_PTD), wxT("extended")));
    if (underwater)
        ptd.unk1 |= cPath::FLAG_UNDERWATER;
    if (extended)
        ptd.unk1 |= cPath::FLAG_EXTENDED;
    wxLogVerbose(wxString::Format(_("Adding ptd %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        wxString cname = child.wxname();

        if (cname == wxT(RAWXML_PTD_TEXTURE_A)) {
            wxString c = child.wxcontent();
            MakeVariable(c);
            ptd.texture_a = c.ToAscii();
        } else if (cname == wxT(RAWXML_PTD_TEXTURE_B)) {
            wxString c = child.wxcontent();
            MakeVariable(c);
            ptd.texture_b = c.ToAscii();
        DO_PTD(FLAT, flat)
        DO_PTD(STRAIGHT_A, straight_a)
        DO_PTD(STRAIGHT_B, straight_b)
        DO_PTD_TURN(U, u)
        DO_PTD_TURN(LA, la)
        DO_PTD_TURN(LB, lb)
        DO_PTD_TURN(TA, ta)
        DO_PTD_TURN(TB, tb)
        DO_PTD_TURN(TC, tc)
        DO_PTD_TURN(X, x)
        DO_PTD_CORNER(A, a)
        DO_PTD_CORNER(B, b)
        DO_PTD_CORNER(C, c)
        DO_PTD_CORNER(D, d)
        DO_PTD_COMPLEX(SLOPE, slope)
        DO_PTD_COMPLEX(SLOPESTRAIGHT, slopestraight)
        DO_PTD_COMPLEX(SLOPESTRAIGHTLEFT, slopestraightleft)
        DO_PTD_COMPLEX(SLOPESTRAIGHTRIGHT, slopestraightright)
        DO_PTD_COMPLEX(SLOPEMID, slopemid)
        DO_PTD(FLAT_FC, flat_fc)
        } else if (cname == wxT(RAWXML_PTD_PAVING)) {
            wxString c = child.wxcontent();
            MakeVariable(c);
            ptd.paving = c.ToAscii();
        } else if (child.element()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in ptd tag '%s'"), child.wxname().c_str(), name.c_str()), child);
        }

        child.go_next();
    }

    ovlPTDManager* c_ptd = m_ovl.GetManager<ovlPTDManager>();
    c_ptd->AddPath(ptd);

}

