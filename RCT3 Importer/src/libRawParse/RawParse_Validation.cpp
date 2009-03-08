///////////////////////////////////////////////////////////////////////////////
//
// raw ovl xml interpreter
// Command line ovl creation utility
// Copyright (C) 2009 Tobias Minch
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

#include "cXmlInputOutputCallbackString.h"
#include "cXmlValidatorIsoSchematron.h"
#include "cXmlValidatorMulti.h"
#include "cXmlValidatorRNVRelaxNG.h"

#include "rng/rct3xml-raw-v1.rnc.gz.h"
#include "rng/rct3xml-raw-v1.sch.gz.h"

boost::shared_ptr<cXmlValidator> cRawParser::Validator() {
	wxLogDebug("cRawParser::Validator()");
    cXmlInputOutputCallbackString::Init();
    XMLCPP_RES_ADD_ONCE(rct3xml_raw_v1, rnc);
    XMLCPP_RES_ADD_ONCE(rct3xml_raw_v1, sch);

    boost::shared_ptr<cXmlValidatorMulti> val(new cXmlValidatorMulti());
	wxLogDebug("cRawParser::Validator(), primary");
    val->primary(boost::shared_ptr<cXmlValidator>(new cXmlValidatorRNVRelaxNG(XMLCPP_RES_USE(rct3xml_raw_v1, rnc).c_str())));
	wxLogDebug("cRawParser::Validator(), secondary");
    val->secondary(boost::shared_ptr<cXmlValidator>(new cXmlValidatorIsoSchematron(XMLCPP_RES_USE(rct3xml_raw_v1, sch).c_str())));
    if (!val->primary()->ok()) {
        wxString error(_("Internal Error: could not load raw RelaxNG schema:\n"));
        error += val->primary()->wxgetErrorList();
        throw RCT3Exception(error);
    }
    if (!val->secondary()->ok()) {
        wxString error(_("Internal Error: could not load raw Schematron schema:\n"));
        error += val->secondary()->wxgetErrorList();
        throw RCT3Exception(error);
    }
    return val;
}

