///////////////////////////////////////////////////////////////////////////////
//
// libXmlCpp
// A light C++ wrapper for libxml2, libxslt and libexslt
// Copyright (C) 2008 Tobias Minch
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
//   Tobias Minich - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CXMLVALIDATORRESULT_H_INCLUDED
#define CXMLVALIDATORRESULT_H_INCLUDED

namespace xmlcpp {

struct cXmlValidatorResult {
    enum LEVEL {
        VR_NONE = 0,
        VR_WARNING = 1,
        VR_ERROR = 2,
        VR_FATAL = 3
    };
    union {
        int levels[4];
        struct {
            int none;
            int warning;
            int error;
            int fatal;
        };
    };
    LEVEL level;
    int multi;
    bool internal_error;
    cXmlValidatorResult(): none(0), warning(0), error(0), fatal(0), level(VR_ERROR), multi(0), internal_error(false) {}
    cXmlValidatorResult(LEVEL deflevel): none(0), warning(0), error(0), fatal(0), level(deflevel), multi(0), internal_error(false) {}

    void merge(const cXmlValidatorResult& other) {
        none += other.none;
        warning += other.warning;
        error += other.error;
        fatal += other.fatal;
        if (other.internal_error)
            internal_error = true;
    }

    bool ok(LEVEL lev) const {
        if (internal_error)
            return false;
        int ret = 0;
        if (lev <= VR_NONE)
            ret += none;
        if (lev <= VR_WARNING)
            ret += warning;
        if (lev <= VR_ERROR)
            ret += error;
        if (lev <= VR_FATAL)
            ret += fatal;
        return !ret;
    }
    inline bool ok() const {
        return ok(level);
    }

    // Logic reversed, false means no error.
    inline bool operator() (LEVEL lev) const { return !ok(lev); }

    typedef LEVEL cXmlValidatorResult::*unspecified_bool_type;
    // Logic reversed, false means no error.
    inline operator unspecified_bool_type() const { return ok()?NULL:(&cXmlValidatorResult::level); }
};

};

#endif // CXMLVALIDATORRESULT_H_INCLUDED
