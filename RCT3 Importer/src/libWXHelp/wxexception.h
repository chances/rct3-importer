#ifndef __wxexception__
#define __wxexception__

#include <exception>
#include <vector>
#include <boost/exception/all.hpp>
#include <wx/string.h>

struct wxe_xml_error_info {
	int line;
	wxString error;
	wxString severity;
	wxe_xml_error_info(int _line, const wxString& _error, const wxString& _severity): line(_line), error(_error), severity(_severity) {}
	wxe_xml_error_info(const wxString& _error): line(0), error(_error), severity("Generic") {}
};
typedef std::vector<wxe_xml_error_info> wxe_xml_error_infos;


typedef boost::error_info<struct tag_wxe_file, wxString> wxe_file;
typedef boost::error_info<struct tag_wxe_xml_node_line, int> wxe_xml_node_line;
typedef boost::error_info<struct tag_wxe_xml_errors, wxe_xml_error_infos> wxe_xml_errors;

class WXException : public boost::exception, public std::exception {

public:
	WXException(const wxString& message);
	WXException(WXException const& ex);
	WXException(const wxString& message, boost::exception const& ex);
	virtual ~WXException() throw() {};

    virtual const char* what() const throw() {
        return m_message.mb_str(wxConvLocal);
    }
    virtual const wxString& wxwhat() const throw() {
        return m_message;
    }
	virtual WXException* duplicate() const {
		return new WXException(*this);
	}
	
	virtual wxString formatLong() const;
	virtual wxString formatBrief() const;

private:
	wxString m_message;
};

#define INHERIT_WXEXCEPTION(e) \
	e(const wxString& message): WXException(message) {} \
	e(e const& ex): WXException(ex) {} \
	e(const wxString& message, boost::exception const& ex): WXException(message, ex) {} \
	virtual e* duplicate() const { return new e(*this); }
	
#endif // __wxexception__
