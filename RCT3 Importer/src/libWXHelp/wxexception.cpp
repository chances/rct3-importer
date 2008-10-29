#include "wxexception.h"

#include "pretty.h"

WXException::WXException(const wxString& message): m_message(message)
{
}

WXException::WXException(WXException const& ex): boost::exception(ex), m_message(ex.m_message)
{
}

WXException::WXException(const wxString& message, boost::exception const& ex): boost::exception(ex), m_message(message)
{
}


wxString WXException::formatLong() const {
	wxString er = "Error: " + m_message + ".";
	{
		boost::shared_ptr<wxString const> err = boost::get_error_info<wxe_file>(*this);
		if( err )
			er += "\nFile: " + *err;
	}
	{
		boost::shared_ptr<int const> err = boost::get_error_info<wxe_xml_node_line>(*this);
		if( err ) {
			if (*err)
				er += wxString::Format("\nLine: %d", *err);
		}
	}
	{
		boost::shared_ptr<wxe_xml_error_infos const> err = boost::get_error_info<wxe_xml_errors>(*this);
		if( err ) {
			if (err->size()) {
				er += "\nXML errors:";
				foreach(const wxe_xml_error_info& inf, *err) {
					er += wxString::Format("\n%03d: [%s] %s", inf.line, inf.severity.c_str(), inf.error.c_str());
				}
			}
		}
	}
	return er;
}

wxString WXException::formatBrief() const {
	return "Not implemented";
}


