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

#include "ManagerQTD.h"

void cRawParser::ParseQTD(cXmlNode& node) {
    USE_PREFIX(node);

    cQueue qtd;
    wxString name = ParseString(node, wxT(RAWXML_QTD), wxT("name"), NULL, useprefix);
    qtd.name = name.ToAscii();
    ParseStringOption(qtd.internalname, node, wxT("internalname"), NULL, useprefix);
    qtd.nametxt = ParseString(node, wxT(RAWXML_QTD), wxT("nametxt"), NULL, useprefix).ToAscii();
    qtd.icon = ParseString(node, wxT(RAWXML_QTD), wxT("icon"), NULL, useprefix).ToAscii();
    qtd.texture = ParseString(node, wxT(RAWXML_QTD), wxT("texture"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding qtd %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        wxString cname = child.wxname();

        if (cname == wxT(RAWXML_QTD_STRAIGHT)) {
            wxString c = child.wxcontent();
            MakeVariable(c);
            qtd.straight = c.ToAscii();
        } else if (cname == wxT(RAWXML_QTD_TURN_L)) {
            wxString c = child.wxcontent();
            MakeVariable(c);
            qtd.turn_l = c.ToAscii();
        } else if (cname == wxT(RAWXML_QTD_TURN_R)) {
            wxString c = child.wxcontent();
            MakeVariable(c);
            qtd.turn_r = c.ToAscii();
        } else if (cname == wxT(RAWXML_QTD_SLOPEUP)) {
            wxString c = child.wxcontent();
            MakeVariable(c);
            qtd.slopeup = c.ToAscii();
        } else if (cname == wxT(RAWXML_QTD_SLOPEDOWN)) {
            wxString c = child.wxcontent();
            MakeVariable(c);
            qtd.slopedown = c.ToAscii();
        } else if (cname == wxT(RAWXML_QTD_SLOPESTRAIGHT)) {
            wxString c = child.wxcontent();
            MakeVariable(c);
            if (qtd.slopestraight[0] == "")
                qtd.slopestraight[0] = c.ToAscii();
            else
                qtd.slopestraight[1] = c.ToAscii();
        } else if (child.element()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in qtd tag '%s'."), cname.c_str(), name.c_str()), child);
        }

        child.go_next();
    }

    ovlQTDManager* c_qtd = m_ovl.GetManager<ovlQTDManager>();
    c_qtd->AddQueue(qtd);

}
