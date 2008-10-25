
#include "version_ovlmake.h"

#include <wx/string.h>

inline void maximize(int& domax, const int withmax) {
    if (withmax>domax)
        domax = withmax;
}

wxString GetAppVersion() {
    wxString strdate = "Built on ";
	strdate += wxString(AutoVersion::DATE, wxConvLocal) + wxT(".")
                          + wxString(AutoVersion::MONTH, wxConvLocal) + wxT(".")
                          + wxString(AutoVersion::YEAR, wxConvLocal);
    wxString strversion = wxString::Format(wxT("ovlmake v%ld.%ld, Build %ld, svn %s, "), AutoVersion::MAJOR, AutoVersion::MINOR, AutoVersion::BUILD, AutoVersion::SVN_REVISION)+ wxString(AutoVersion::STATUS, wxConvLocal);
    wxString compversion("Compiler: ");
#ifdef __GNUC__
    compversion += "GCC ";
#else
    compversion += "Non-GCC ";
#endif
    compversion += wxString(__VERSION__);
#ifdef UNICODE
    strdate += wxT(", Unicode");
#else
    strdate += wxT(", ANSI");
#endif
#ifdef PUBLICDEBUG
    strdate += wxT(" Debug");
#endif
    strdate += " version";
    int linelen = strdate.size();
    maximize(linelen, strversion.size());
    maximize(linelen, compversion.size());
    wxString line(wxT('-'), linelen);
	
	wxString ret = "\n";
	ret += line;
	ret += "\n";
	ret += strversion;
	ret += "\n";
	ret += strdate;
	ret += "\n";
	ret += compversion;
	ret += "\nCopyright (C) ";
	ret += wxString(AutoVersion::YEAR, wxConvLocal);
	ret += " Belgabor\n";
	ret += line;
	ret += "\n\n"
			"This program comes with ABSOLUTELY NO WARRANTY.\n"
			"This is free software, and you are welcome to redistribute it\n"
			"under certain conditions; see License.txt for details.\n\n";
	return ret;
}
