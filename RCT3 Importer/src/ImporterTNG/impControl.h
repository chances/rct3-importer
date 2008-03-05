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

#ifndef IMPCONTROL_H
#define IMPCONTROL_H

#include <boost/shared_ptr.hpp>
#include <sigc++/signal.h>
#include <sigc++/trackable.h>

#include "cXmlXPath.h"

#include "impRawovlFileManagerBaseConnecter.h"
#include "impValidator.h"
#include "sigchelper.h"

class impView;
class impControl : public impRawovlFileManagerBaseConnecter {
public:
    impControl();
    virtual ~impControl();

    inline void setPath(const wxString& path) {
        m_path = path;
    }

    virtual void init(impView& view, const wxString& path, const wxString& parentpath = wxString());

    virtual wxString value() const;
    inline impValidator::STATUS valid() { return validator()(); }
    inline impValidator& validator() { return *m_valid; };
    inline void validator(const impValidator& valid) {
        m_valid.reset(valid.clone());
        m_valid->setControl(this);
    }

public: // Slots
    virtual void update(const wxString& value) = 0;

public: // Signals
    //sigc::signal<void,const wxString&, const wxString&> changeValue;

protected:
    wxString m_path;
    wxString m_parentpath;
    boost::shared_ptr<impValidator> m_valid;

    virtual void change();
private:
};

#endif // IMPCONTROL_H
