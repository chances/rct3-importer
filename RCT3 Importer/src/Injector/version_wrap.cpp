
#include "version_injector.h"

#include <wx/string.h>

wxString GetAppVersion() {
    wxString ver(wxString::Format(wxT("RCT3 Injector v%ld.%ld%s (Build %ld"),
                AutoVersion::MAJOR, AutoVersion::MINOR, AutoVersion::STATUS_SHORT, AutoVersion::BUILD));
    if (strcmp(AutoVersion::SVN_REVISION, "0"))
        ver += wxString::Format(wxT("; svn %s"), AutoVersion::SVN_REVISION);
    return ver + ")";
}

