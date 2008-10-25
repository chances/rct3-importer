
#include "version_importer.h"

#include <wx/string.h>

wxString GetAppVersion() {
    wxString ver(wxString::Format(wxT("RCT3 Importer v%ld%s%ld (Build %ld"),
                AutoVersion::MAJOR, AutoVersion::STATUS_SHORT, AutoVersion::MINOR, AutoVersion::BUILD));
    if (strcmp(AutoVersion::SVN_REVISION, "0"))
        ver += wxString::Format(wxT("; svn %s"), AutoVersion::SVN_REVISION);
    return ver + ")";
}

