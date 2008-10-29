#ifndef __wxexception_libxmlcpp__
#define __wxexception_libxmlcpp__

#include "wxexception.h"
#include "cXmlErrorHandler.h"

inline wxe_xml_error_info makeXmlErrorInfo(const xmlcpp::cXmlStructuredError& err) {
	wxe_xml_error_info e(wxString::FromUTF8(err.message.c_str()));
	e.line = err.getLine();
	switch (err.level) {
		case XML_ERR_NONE:
			e.severity = "Info";
			break;
		case XML_ERR_WARNING:
			e.severity = "Warning";
			break;
		case XML_ERR_ERROR:
			e.severity = "Error";
			break;
		case XML_ERR_FATAL:
			e.severity = "Fatal";
			break;
	}
	return e;
}

inline int transferXmlErrors(const xmlcpp::cXmlErrorHandler& from, wxe_xml_error_infos& to) {
	int eline = 0;
	foreach(const xmlcpp::cXmlStructuredError& se, from.getStructuredErrors()) {
		to.push_back(makeXmlErrorInfo(se));
		if (!eline) 
			eline = se.getLine();
	}
	foreach(const std::string& ge, from.getGenericErrors())
		to.push_back(wxString::FromUTF8(ge.c_str()));
	return eline;
}

#endif