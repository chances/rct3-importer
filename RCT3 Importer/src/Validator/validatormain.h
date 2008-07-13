///////////////////////////////////////////////////////////////////////////////
//
// Schema Validator
// Validator for RelaxNG and Schematron
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

#ifndef VALIDATORMAIN_H_INCLUDED
#define VALIDATORMAIN_H_INCLUDED

#include <boost/shared_ptr.hpp>
#include <wx/stc/stc.h>

#include "cXmlValidatorMulti.h"

#include "resources/res_main.h"

class dlgKeyframes;
class frmMain: public rcfrmMain {
public:
    frmMain(wxWindow* parent);
    virtual ~frmMain();

    virtual void OnMenuLoad( wxCommandEvent& event );
    virtual void OnMenuSave( wxCommandEvent& event );
    virtual void OnMenuSaveAs( wxCommandEvent& event );
    virtual void OnSchema( wxCommandEvent& event );
    virtual void OnSTCMarginClick (wxStyledTextEvent &event);
    virtual void OnSTCChange (wxStyledTextEvent &event);
    //virtual void OnSTCKeyDown (wxKeyEvent &event);
    virtual void OnSTCCharAdded (wxStyledTextEvent &event);
    virtual void OnValidate( wxCommandEvent& event );
    virtual void OnReloadSchema( wxCommandEvent& event );
    virtual void OnReloadXml( wxCommandEvent& event );

    virtual void OnInsertXInclude( wxCommandEvent& event );
    virtual void OnAbbreviation( wxCommandEvent& event );
    virtual void OnAbbreviationFull( wxCommandEvent& event );
    virtual void OnApplyXIncludes( wxCommandEvent& event );

    virtual void OnSchemaCompiler( wxCommandEvent& event );
    virtual void OnSchemaRaw( wxCommandEvent& event );
    virtual void OnSchemaScenery( wxCommandEvent& event );
    virtual void OnSchemaModel( wxCommandEvent& event );
    virtual void OnSchemaMS3D( wxCommandEvent& event );

    virtual void OnSchemaProfile10( wxCommandEvent& event );
    virtual void OnSchemaProfile100( wxCommandEvent& event );
    virtual void OnSchemaProfile1000( wxCommandEvent& event );

    virtual void OnRelaxNG( wxCommandEvent& event );
    virtual void OnSchematron( wxCommandEvent& event );

    virtual void OnReloadBoth( wxCommandEvent& event );
    virtual void OnKeyframe( wxCommandEvent& event );

private:
    enum {
        INT_EXTERNAL = 0,
        INT_SCENERY,
        INT_COMPILER,
        INT_RAW,
        INT_MODEL,
        INT_MS3D,

        INT_UNDEFINED
    };
    boost::shared_ptr<wxFileDialog> m_fdlgSchema;
    boost::shared_ptr<wxFileDialog> m_fdlgFile;
    xmlcpp::cXmlValidatorMulti m_val;
    bool m_schemachanged;
    int m_internal;
    wxString m_xmlfile;
    boost::shared_ptr<dlgKeyframes> m_kf;
    typedef std::map<wxString, wxString>::iterator abbfind;
    std::map<wxString, wxString> m_abbreviations;

    void MarkLine(wxStyledTextCtrl* ctl, int line, int level);

    void InitXMLSTC(wxStyledTextCtrl* ctl);
    void InitCommonSTC(wxStyledTextCtrl* ctl);
    bool DoValidate();
    void DoProfileValidation(int times);
    void DoAbbreviation(bool quick);

    void SaveFile();

    void DisplayRelaxNG();
    void DisplaySchematron();
};

#endif // MANAGERMAIN_H_INCLUDED
