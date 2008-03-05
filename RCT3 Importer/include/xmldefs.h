
#ifndef _XMLDEFS_H_
#define _XMLDEFS_H_

#define WXW(x) wxString(wxT(x))

#define UTF8CHARWRAP(c) wxString(c, wxConvUTF8)
#define UTF8STRINGWRAP(c) wxString(c.c_str(), wxConvUTF8)
#define STRING_FOR_FORMAT(c) UTF8STRINGWRAP(c).c_str()

#define WX2UTF8(c) (c.mb_str(wxConvUTF8))
#define WXC2UTF8(c) WX2UTF8(wxString(c))

#define RAW_NS(c) (XML_NAMESPACE_RAW ":" c)


#define XML_NAMESPACE_SCENERY       "http://rct3.sourceforge.net/rct3xml/scenery"
#define XML_SCHEMALOCATION_SCENERY  "http://rct3.sourceforge.net/xml/rct3xml-scenery-v1.xsd"
#define XML_NAMESPACE_RAW           "http://rct3.sourceforge.net/rct3xml/raw"
#define XML_SCHEMALOCATION_RAW      "http://rct3.sourceforge.net/xml/rct3xml-raw-v1.xsd"
#define XML_NAMESPACE_COMPILER      "http://rct3.sourceforge.net/rct3xml/ovlcompiler"
#define XML_SCHEMALOCATION_COMPILER "http://rct3.sourceforge.net/xml/rct3xml-ovlcompiler-v1.xsd"

#define XML_NAMESPACE_SCHEMAINSTANCE "http://www.w3.org/2001/XMLSchema-instance"

#define XML_TAG_OVL             "rawovl"

#define XML_TAG_METADATA        "metadata"
#define XML_TAG_METADATA_NAME   "name"
#define XML_TAG_METADATA_GUID   "guid"

#define XML_TAG_REFERENCE       "reference"

#endif
