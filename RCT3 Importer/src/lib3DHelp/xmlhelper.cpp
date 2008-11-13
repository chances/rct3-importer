///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// Xml helper functions & macros
// Copyright (C) 2006-2008 Tobias Minch
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
// Parts of this code are based on code written for rct3tool (Copyright 2005
// Jonathan Wilson) by DragonsIOA.
//
///////////////////////////////////////////////////////////////////////////////

#include <string>
#include <boost/spirit.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "boneanim.h"
#include "vertex.h"

using namespace r3;
using namespace std;
using namespace boost::spirit;

bool parseFloat_(std::string& str, float& a) {
    boost::algorithm::trim(str);
    real_parser<float, real_parser_policies<float> > real_pf;
    return parse(str.c_str(), real_pf[assign_a(a)], space_p).full;
}

bool parseFloat_(std::string& str, double& a) {
    boost::algorithm::trim(str);
    return parse(str.c_str(), real_p[assign_a(a)], space_p).full;
}

bool parseULong_(std::string& str, unsigned long& a) {
    boost::algorithm::trim(str);
	if (parse(str.c_str(), uint_p[assign_a(a)], space_p).full) {
		return true;
	} else {
		if (str == "true") {
			a = 1;
			return true;
		}
		if (str == "false") {
			a = 0;
			return true;
		}
		return false;
	}
}

bool parseHexULong_(std::string& str, unsigned long& a) {
    boost::algorithm::trim(str);
    return parse(str.c_str(), hex_p[assign_a(a)], space_p).full;
}

bool parseMatrixRow(std::string& str, float& a, float& b, float& c, float& d) {
    boost::algorithm::trim(str);
    real_parser<float, real_parser_policies<float> > real_pf;
    return parse(str.c_str(), real_pf[assign_a(a)] >> real_pf[assign_a(b)] >> real_pf[assign_a(c)] >> real_pf[assign_a(d)], space_p).full;
}

bool parseMatrix(std::string& str, MATRIX& m) {
    boost::algorithm::trim(str);
    real_parser<float, real_parser_policies<float> > real_pf;
    return parse(str.c_str(),
        real_pf[assign_a(m._11)] >> real_pf[assign_a(m._12)] >> real_pf[assign_a(m._13)] >> real_pf[assign_a(m._14)]
        >> real_pf[assign_a(m._21)] >> real_pf[assign_a(m._22)] >> real_pf[assign_a(m._23)] >> real_pf[assign_a(m._24)]
        >> real_pf[assign_a(m._31)] >> real_pf[assign_a(m._32)] >> real_pf[assign_a(m._33)] >> real_pf[assign_a(m._34)]
        >> real_pf[assign_a(m._41)] >> real_pf[assign_a(m._42)] >> real_pf[assign_a(m._43)] >> real_pf[assign_a(m._44)]
        , space_p).full;
}

bool parseCompilerVector(std::string& str, txyz& v) {
    boost::algorithm::trim(str);
    real_parser<float, real_parser_policies<float> > real_pf;
    return parse(str.c_str(), real_pf[assign_a(v.X)] >> real_pf[assign_a(v.Y)] >> real_pf[assign_a(v.Z)], space_p).full;
}

