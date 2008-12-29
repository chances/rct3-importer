///////////////////////////////////////////////////////////////////////////////
//
// RCT3 Injector
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


#ifndef _INJECTORAPP_H_
#define _INJECTORAPP_H_

#include <wx/app.h>
#include <wx/filename.h>

class InjectorApp : public wxApp {
private:
    wxFileName m_appdir;
    wxString m_title;
	wxString m_installdir;

	bool CheckInstallDir();

public:
    virtual bool OnInit();
    virtual int OnRun();
    virtual int OnExit();
	
	virtual void OnInitCmdLine(wxCmdLineParser&  parser);
	virtual bool OnCmdLineError(wxCmdLineParser&  parser);
	virtual bool OnCmdLineHelp(wxCmdLineParser&  parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser&  parser);

    inline const wxFileName& getAppDir() const              { return m_appdir; }
    inline const wxString& getTitle() const                 { return m_title; }
    inline const wxString& getInstallDir() const            { return m_installdir; }
};

DECLARE_APP(InjectorApp)

#endif
