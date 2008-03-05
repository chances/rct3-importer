

#define INFOTAG_APPLICATION
#define INFOTAG_WXWIDGETS
#define INFOTAG_BOOST
#define INFOTAG_BZLIB
#define INFOTAG_CRYPTOPP
#define INFOTAG_EXPAT
#define INFOTAG_JPEG
#define INFOTAG_GMAGICK
#define INFOTAG_PNG
#define INFOTAG_TIFF
#define INFOTAG_XML2
#define INFOTAG_XSLT
#define INFOTAG_ZLIB

#include <wx/html/m_templ.h>

#ifdef INFOTAG_APPLICATION
#include "version.h"
#endif
#ifdef INFOTAG_BOOST
#include <boost/version.hpp>
#endif
#ifdef INFOTAG_BZLIB
#include <bzlib.h>
#endif
#ifdef INFOTAG_CRYPTOPP
#include <cryptopp/config.h>
#endif
#ifdef INFOTAG_EXPAT
#include <expat.h>
#endif
#ifdef INFOTAG_JPEG
#define HAVE_BOOLEAN
#include <jpeglib.h>
#endif
#ifdef INFOTAG_GMAGICK
// To include version.h without other stuff
#define QuantumDepth 8
#define MagickExport
#include <magick/version.h>
#endif
#ifdef INFOTAG_PNG
#define PNG_VERSION_INFO_ONLY
#include <png.h>
#endif
#ifdef INFOTAG_TIFF
#include <tiffio.h>
#endif
#ifdef INFOTAG_XML2
#include <libxml/xmlversion.h>
#endif
#ifdef INFOTAG_XSLT
#include <libxslt/xslt.h>
#include <libxslt/xsltconfig.h>
#endif
#ifdef INFOTAG_ZLIB
#include <zlib.h>
#endif

TAG_HANDLER_BEGIN(WXINFOTAG, "INFO")

TAG_HANDLER_PROC(tag)
{
    wxString ret = wxT("INFO TAG PARSING ERROR");
    if (false) {

    }
#ifdef INFOTAG_APPLICATION
    else if (tag.HasParam(wxT("APP"))) {
        wxString version = tag.GetParam(wxT("APP"));
        if (!version.CmpNoCase(wxT("date")))
            ret = wxString(AutoVersion::DATE, wxConvLocal);
        else if (!version.CmpNoCase(wxT("month")))
            ret = wxString(AutoVersion::MONTH, wxConvLocal);
        else if (!version.CmpNoCase(wxT("year")))
            ret = wxString(AutoVersion::YEAR, wxConvLocal);
        else if (!version.CmpNoCase(wxT("status")))
            ret = wxString(AutoVersion::STATUS, wxConvLocal);
        else if (!version.CmpNoCase(wxT("status_short")))
            ret = wxString(AutoVersion::STATUS_SHORT, wxConvLocal);
        else if (!version.CmpNoCase(wxT("major")))
            ret = wxString::Format(wxT("%ld"), AutoVersion::MAJOR);
        else if (!version.CmpNoCase(wxT("minor")))
            ret = wxString::Format(wxT("%ld"), AutoVersion::MINOR);
        else if (!version.CmpNoCase(wxT("build")))
            ret = wxString::Format(wxT("%ld"), AutoVersion::BUILD);
        else if (!version.CmpNoCase(wxT("revision")))
            ret = wxString::Format(wxT("%ld"), AutoVersion::REVISION);
        else if (!version.CmpNoCase(wxT("version")))
            ret = wxString::Format(wxT("%ld.%ld.%ld"), AutoVersion::MAJOR, AutoVersion::MINOR, AutoVersion::BUILD)+wxString(AutoVersion::STATUS_SHORT, wxConvLocal);
    }
#endif
#ifdef INFOTAG_WXWIDGETS
    else if (tag.HasParam(wxT("WXWIDGETS"))) {
        wxString version = tag.GetParam(wxT("WXWIDGETS"));
        if (!version.CmpNoCase(wxT("version")))
            ret = wxVERSION_STRING;
        else if (!version.CmpNoCase(wxT("name"))) {
            ret = wxT("wxWidgets");
        }
        else if (!version.CmpNoCase(wxT("url"))) {
            ret = wxT("http://www.wxwidgets.org");
        }
        else if (!version.CmpNoCase(wxT("license"))) {
            ret = wxT("http://www.wxwidgets.org");
        }
        else if (!version.CmpNoCase(wxT("license")))
            ret = wxT("wxWindows");
        else if (!version.CmpNoCase(wxT("build"))) {
#ifdef wxUSE_UNICODE
            ret = wxT("Unicode");
#else
            ret = wxT("Ansi");
#endif
        }
        else if (!version.CmpNoCase(wxT("debug"))) {
#ifdef __WXDEBUG__
            ret = wxT("Debug");
#else
            ret = wxT("Release");
#endif
        }
        else if (!version.CmpNoCase(wxT("platform"))) {
#if defined(__WXMSW__)
            ret = wxT("Windows");
#elif defined(__WXMOTIF__)
            ret = wxT("Motif");
#elif defined(__WXPALMOS__)
            ret = wxT("PalmOS");
#elif defined(__WXGTK__)
            ret = wxT("GTK");
#elif defined(__WXPM__)
            ret = wxT("PM");
#elif defined(__WXMAC__)
            ret = wxT("Mac");
#elif defined(__WXCOCOA__)
            ret = wxT("Cocoa");
#elif defined(__X__)
            ret = wxT("X");
#elif defined(__WXMGL__)
            ret = wxT("MGL");
#elif defined(__WXDFB__)
            ret = wxT("DFB");
#elif defined(__WXX11__)
            ret = wxT("X11");
#endif
        }
    }
#endif
#ifdef INFOTAG_BOOST
    else if (tag.HasParam(wxT("BOOST"))) {
        wxString version = tag.GetParam(wxT("BOOST"));
        if (!version.CmpNoCase(wxT("version")))
            ret = wxString::Format(wxT("%d.%d.%d"), BOOST_VERSION / 100000, BOOST_VERSION / 100 % 1000, BOOST_VERSION % 100);
        else if (!version.CmpNoCase(wxT("name")))
            ret = wxT("Boost");
        else if (!version.CmpNoCase(wxT("url")))
            ret = wxT("http://www.boost.org");
        else if (!version.CmpNoCase(wxT("license")))
            ret = wxT("Boost");
        else if (!version.CmpNoCase(wxT("major")))
            ret = wxString::Format(wxT("%d"), BOOST_VERSION / 100000);
        else if (!version.CmpNoCase(wxT("minor")))
            ret = wxString::Format(wxT("%d"), BOOST_VERSION / 100 % 1000);
        else if (!version.CmpNoCase(wxT("build")))
            ret = wxString::Format(wxT("%d"), BOOST_VERSION % 100);
    }
#endif
#ifdef INFOTAG_BZLIB
    else if (tag.HasParam(wxT("BZLIB"))) {
        wxString version = tag.GetParam(wxT("BZLIB"));
        wxString bzver = wxString(BZ2_bzlibVersion(), wxConvLocal);
        bzver = bzver.BeforeFirst(wxT(','));
        if (!version.CmpNoCase(wxT("version")))
            ret = bzver;
        else if (!version.CmpNoCase(wxT("name")))
            ret = wxT("bzip2");
        else if (!version.CmpNoCase(wxT("url")))
            ret = wxT("http://www.bzip.org");
        else if (!version.CmpNoCase(wxT("license")))
            ret = wxT("BSD-like");
        else if (!version.CmpNoCase(wxT("major")))
            ret = bzver.BeforeFirst(wxT('.'));
        else if (!version.CmpNoCase(wxT("minor")))
            ret = bzver.AfterFirst(wxT('.')).BeforeFirst(wxT('.'));
        else if (!version.CmpNoCase(wxT("build")))
            ret = bzver.AfterLast(wxT('.'));
    }
#endif
#ifdef INFOTAG_CRYPTOPP
    else if (tag.HasParam(wxT("CRYPTOPP"))) {
        wxString version = tag.GetParam(wxT("CRYPTOPP"));
        if (!version.CmpNoCase(wxT("version")))
            ret = wxString::Format(wxT("%d.%d.%d"), CRYPTOPP_VERSION / 100, (CRYPTOPP_VERSION % 100) / 10, CRYPTOPP_VERSION % 10);
        else if (!version.CmpNoCase(wxT("name")))
            ret = wxT("cryptopp");
        else if (!version.CmpNoCase(wxT("url")))
            ret = wxT("http://www.cryptopp.com");
        else if (!version.CmpNoCase(wxT("license")))
            ret = wxT("Special");
        else if (!version.CmpNoCase(wxT("major")))
            ret = wxString::Format(wxT("%d"), CRYPTOPP_VERSION / 100);
        else if (!version.CmpNoCase(wxT("minor")))
            ret = wxString::Format(wxT("%d"), (CRYPTOPP_VERSION % 100) / 10);
        else if (!version.CmpNoCase(wxT("build")))
            ret = wxString::Format(wxT("%d"), CRYPTOPP_VERSION % 10);
    }
#endif
#ifdef INFOTAG_EXPAT
    else if (tag.HasParam(wxT("EXPAT"))) {
        wxString version = tag.GetParam(wxT("EXPAT"));
        XML_Expat_Version ver = XML_ExpatVersionInfo();
        if (!version.CmpNoCase(wxT("version")))
            ret = wxString::Format(wxT("%d.%d.%d"), ver.major, ver.minor, ver.micro);
        else if (!version.CmpNoCase(wxT("name")))
            ret = wxT("expat");
        else if (!version.CmpNoCase(wxT("url")))
            ret = wxT("http://www.libexpat.org");
        else if (!version.CmpNoCase(wxT("license")))
            ret = wxT("AS-IS");
        else if (!version.CmpNoCase(wxT("major")))
            ret = wxString::Format(wxT("%d"), ver.major);
        else if (!version.CmpNoCase(wxT("minor")))
            ret = wxString::Format(wxT("%d"), ver.minor);
        else if (!version.CmpNoCase(wxT("build")))
            ret = wxString::Format(wxT("%d"), ver.micro);
    }
#endif
#ifdef INFOTAG_JPEG
    else if (tag.HasParam(wxT("JPEG"))) {
        wxString version = tag.GetParam(wxT("JPEG"));
        wxChar minversion = (wxT('a')) - 1 + (JPEG_LIB_VERSION % 10);
        if (!version.CmpNoCase(wxT("version")))
            ret = wxString::Format(wxT("%d"), JPEG_LIB_VERSION / 10) + wxString(minversion, 1);
        else if (!version.CmpNoCase(wxT("name")))
            ret = wxT("jpeg");
        else if (!version.CmpNoCase(wxT("url")))
            ret = wxT("http://www.ijg.org");
        else if (!version.CmpNoCase(wxT("license")))
            ret = wxT("AS-IS like");
        else if (!version.CmpNoCase(wxT("major")))
            ret = wxString::Format(wxT("%d"), JPEG_LIB_VERSION / 10);
        else if (!version.CmpNoCase(wxT("minor")))
            ret = wxString(minversion, 1);
    }
#endif
#ifdef INFOTAG_GMAGICK
    else if (tag.HasParam(wxT("GMAGICK"))) {
        wxString version = tag.GetParam(wxT("GMAGICK"));
        wxString gver = wxString(MagickLibVersionText, wxConvLocal);
        if (!version.CmpNoCase(wxT("version")))
            ret = gver;
        else if (!version.CmpNoCase(wxT("name")))
            ret = wxString(MagickPackageName, wxConvLocal);
        else if (!version.CmpNoCase(wxT("url")))
            ret = wxT("http://www.graphicsmagick.org");
        else if (!version.CmpNoCase(wxT("license")))
            ret = wxT("X11");
        else if (!version.CmpNoCase(wxT("major")))
            ret = gver.BeforeFirst(wxT('.'));
        else if (!version.CmpNoCase(wxT("minor")))
            ret = gver.AfterFirst(wxT('.')).BeforeFirst(wxT('.'));
        else if (!version.CmpNoCase(wxT("build")))
            ret = gver.AfterLast(wxT('.'));
    }
#endif
#ifdef INFOTAG_PNG
    else if (tag.HasParam(wxT("PNG"))) {
        wxString version = tag.GetParam(wxT("PNG"));
        if (!version.CmpNoCase(wxT("version")))
            ret = wxString(PNG_LIBPNG_VER_STRING, wxConvLocal);
        else if (!version.CmpNoCase(wxT("name")))
            ret = wxT("png");
        else if (!version.CmpNoCase(wxT("url")))
            ret = wxT("http://www.libpng.org");
        else if (!version.CmpNoCase(wxT("license")))
            ret = wxT("zlib/libpng");
        else if (!version.CmpNoCase(wxT("major")))
            ret = wxString::Format(wxT("%d"), PNG_LIBPNG_VER_MAJOR);
        else if (!version.CmpNoCase(wxT("minor")))
            ret = wxString::Format(wxT("%d"), PNG_LIBPNG_VER_MINOR);
        else if (!version.CmpNoCase(wxT("build")))
            ret = wxString::Format(wxT("%d"), PNG_LIBPNG_VER_RELEASE);
    }
#endif
#ifdef INFOTAG_TIFF
    else if (tag.HasParam(wxT("TIFF"))) {
        wxString version = tag.GetParam(wxT("TIFF"));
        wxString tiffver = wxString(TIFFGetVersion(), wxConvLocal);
        // Get version, cheap-ass way
        tiffver = tiffver.BeforeFirst(wxT('\n'));
        tiffver = tiffver.AfterFirst(wxT(' '));
        tiffver = tiffver.AfterFirst(wxT(' '));
        if (!version.CmpNoCase(wxT("version")))
            ret = tiffver;
        else if (!version.CmpNoCase(wxT("name")))
            ret = wxT("tiff");
        else if (!version.CmpNoCase(wxT("url")))
            ret = wxT("http://www.libtiff.org");
        else if (!version.CmpNoCase(wxT("license")))
            ret = wxT("libtiff");
        else if (!version.CmpNoCase(wxT("major")))
            ret = tiffver.BeforeFirst(wxT('.'));
        else if (!version.CmpNoCase(wxT("minor")))
            ret = tiffver.AfterFirst(wxT('.')).BeforeFirst(wxT('.'));
        else if (!version.CmpNoCase(wxT("build")))
            ret = tiffver.AfterLast(wxT('.'));
    }
#endif
#ifdef INFOTAG_XML2
    else if (tag.HasParam(wxT("XML2"))) {
        wxString version = tag.GetParam(wxT("XML2"));
        if (!version.CmpNoCase(wxT("version")))
            ret = wxString(LIBXML_DOTTED_VERSION, wxConvLocal);
        else if (!version.CmpNoCase(wxT("name")))
            ret = wxT("xml2");
        else if (!version.CmpNoCase(wxT("url")))
            ret = wxT("http://xmlsoft.org");
        else if (!version.CmpNoCase(wxT("license")))
            ret = wxT("MIT");
        else if (!version.CmpNoCase(wxT("major")))
            ret = wxString::Format(wxT("%d"), LIBXML_VERSION / 10000);
        else if (!version.CmpNoCase(wxT("minor")))
            ret = wxString::Format(wxT("%d"), (LIBXML_VERSION % 10000) / 100);
        else if (!version.CmpNoCase(wxT("build")))
            ret = wxString::Format(wxT("%d"), LIBXML_VERSION % 100);
    }
#endif
#ifdef INFOTAG_XSLT
    else if (tag.HasParam(wxT("XSLT"))) {
        wxString version = tag.GetParam(wxT("XSLT"));
        if (!version.CmpNoCase(wxT("version")))
            ret = wxString(LIBXSLT_DOTTED_VERSION, wxConvLocal);
        else if (!version.CmpNoCase(wxT("name")))
            ret = wxT("xslt");
        else if (!version.CmpNoCase(wxT("url")))
            ret = wxString(XSLT_DEFAULT_URL, wxConvLocal);
        else if (!version.CmpNoCase(wxT("license")))
            ret = wxT("MIT");
        else if (!version.CmpNoCase(wxT("major")))
            ret = wxString::Format(wxT("%d"), LIBXSLT_VERSION / 10000);
        else if (!version.CmpNoCase(wxT("minor")))
            ret = wxString::Format(wxT("%d"), (LIBXSLT_VERSION % 10000) / 100);
        else if (!version.CmpNoCase(wxT("build")))
            ret = wxString::Format(wxT("%d"), LIBXSLT_VERSION % 100);
    }
#endif
#ifdef INFOTAG_ZLIB
    else if (tag.HasParam(wxT("ZLIB"))) {
        wxString version = tag.GetParam(wxT("ZLIB"));
        if (!version.CmpNoCase(wxT("version")))
            ret = wxString(ZLIB_VERSION, wxConvLocal);
        else if (!version.CmpNoCase(wxT("name")))
            ret = wxT("zlib");
        else if (!version.CmpNoCase(wxT("url")))
            ret = wxT("http://www.zlib.org");
        else if (!version.CmpNoCase(wxT("license")))
            ret = wxT("zlib/libpng");
        else if (!version.CmpNoCase(wxT("major")))
            ret = wxString::Format(wxT("%d"), ZLIB_VERNUM / 1000);
        else if (!version.CmpNoCase(wxT("minor")))
            ret = wxString::Format(wxT("%d"), (ZLIB_VERNUM % 1000) / 100);
        else if (!version.CmpNoCase(wxT("build")))
            ret = wxString::Format(wxT("%d"), (ZLIB_VERNUM % 100) / 10);
    }
#endif


    if (tag.HasParam(wxT("URLTEXT"))) {
        wxString ut = tag.GetParam(wxT("URLTEXT"));
        if (ut.CmpNoCase(wxT("useurl"))) {
            ret = wxString::Format(wxT("<a href='%s'>%s</a>"), ret.c_str(), ret.c_str());
        } else {
            ret = wxString::Format(wxT("<a href='%s'>%s</a>"), ret.c_str(), ut.c_str());
        }
    }

    if (tag.HasParam(wxT("MAKELINK"))) {
        wxString ut = tag.GetParam(wxT("MAKELINK"));
        ret = wxString::Format(wxT("<a href='%s'>%s</a>"), ut.c_str(), ret.c_str());
    }

    if (tag.HasParam(wxT("SP"))) {
        ret += wxT(" ");
    }

    m_WParser->GetContainer()->InsertCell(new wxHtmlWordCell(ret, *m_WParser->GetDC()));

    return false;
}

TAG_HANDLER_END(WXINFOTAG)



TAGS_MODULE_BEGIN(wxInfoTag)

    TAGS_MODULE_ADD(WXINFOTAG)

TAGS_MODULE_END(wxInfoTag)

