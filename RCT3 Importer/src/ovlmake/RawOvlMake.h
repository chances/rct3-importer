

#ifndef _RAWOVLMAKE_H_
#define _RAWOVLMAKE_H_

#include "wx_pch.h"

#include <wx/filename.h>
#include <wx/xml/xml.h>

#include "OVLng.h"

#define RAWXML_ROOT wxT("rawovl")
#define RAWXML_SUBROOT wxT("subovl")

class cRawOvl {
private:
    wxFileName m_output;
    wxFileName m_outputbasedir;
    wxFileName m_input;  // For path relativity
    cOvl m_ovl;

    void Load(wxXmlNode* root);
    cOvlType ParseType(wxXmlNode* node, const wxString& nodes, const wxString& attribute = wxT("target"));
    wxString ParseString(wxXmlNode* node, const wxString& nodes, const wxString& attribute);
    unsigned long ParseUnsigned(wxXmlNode* node, const wxString& nodes, const wxString& attribute);
    long ParseSigned(wxXmlNode* node, const wxString& nodes, const wxString& attribute);
    double ParseFloat(wxXmlNode* node, const wxString& nodes, const wxString& attribute);

    void ParseSID(wxXmlNode* node);
    void ParseTEX(wxXmlNode* node);
public:
    cRawOvl(const wxFileName& file, const wxFileName& outputdir = wxString(wxT("")), const wxFileName& output = wxString(wxT("")));
    cRawOvl(wxXmlNode* root, const wxFileName& file, const wxFileName& outputdir = wxString(wxT("")), const wxFileName& output = wxString(wxT("")));
};

#endif
