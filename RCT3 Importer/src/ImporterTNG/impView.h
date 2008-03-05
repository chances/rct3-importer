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

#ifndef _IMPVIEW_H
#define _IMPVIEW_H

#include <sigc++/signal.h>
#include <sigc++/trackable.h>
#include <wx/aui/auibook.h>

#include "cXmlDoc.h"
#include "cXmlXPath.h"

#include "impRawovlFileManagerBaseConnecter.h"
#include "impTypes.h"
#include "sigchelper.h"

struct impViewInfo {
    int type;
    wxString path;
    wxWindow* wnd;

    impViewInfo(): type(0), wnd(NULL) {}
};

class impView : public impRawovlFileManagerBaseConnecter {
public:
    impView(): m_nb(NULL), m_page(0) {}

    virtual ~impView() {
        unregister(*this);
    }

public: // Signals
    sigc::signal<void, const impView&> unregister;
    sigc::signal<void, const wxString&> update_controls;
    sigc::signal<void, impTypes::COMMAND, impTypes::ELEMENTTYPE, impTypes::VIEWTYPE, const wxString> executeCommand;

public: // Slots
    virtual void update(const wxString& value) = 0;

private:
    impViewInfo m_info;

protected:
    wxAuiNotebook* m_nb;
    size_t m_page;

    void initInfo(const wxString& path_);
    virtual void updateNotebookPage();

public:
    virtual wxString getName() = 0;
    inline virtual impTypes::VIEWTYPE getType() { return impTypes::VIEWTYPE_ABSTRACT; }
    virtual const impViewInfo& getInfo() const {
        return m_info;
    }
    virtual void setNotebook(wxAuiNotebook* nb);

    static impView* createView(wxWindow* parent, impTypes::VIEWTYPE type, const wxString& path);
};

#endif
