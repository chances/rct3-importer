
#include "version.h"

wxString GetAppVersion() {
    return wxString::Format(wxT("RCT3 Importer v%ld.%ld%s (Build %ld; svn %s)"),
                AutoVersion::MAJOR, AutoVersion::MINOR, AutoVersion::STATUS_SHORT, AutoVersion::BUILD, AutoVersion::SVN_REVISION);
}

