///////////////////////////////////////////////////////////////////////////////
//
// raw ovl xml interpreter
// Command line ovl creation utility
// Copyright (C) 2007 Tobias Minch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////



#include "RawOvlMake.h"

#include "OVLManagers.h"
#include "RCT3Exception.h"
#include "RCT3Macros.h"
#include "SCNFile.h"
#include "gximage.h"
#include "rct3log.h"

#define RAWXML_GSI wxT("gsi")
#define RAWXML_SID wxT("sid")
#define RAWXML_SVD wxT("svd")
#define RAWXML_TEX wxT("tex")
#define RAWXML_TXT wxT("txt")

#define RAWXML_SID_GROUP            wxT("group")
#define RAWXML_SID_TYPE             wxT("type")
#define RAWXML_SID_POSITION         wxT("position")
#define RAWXML_SID_COLOURS          wxT("colours")
#define RAWXML_SID_FLAGS            wxT("flags")
#define RAWXML_SID_IMPORTERUNKNOWNS wxT("importerunknowns")
#define RAWXML_SID_STALLUNKNOWNS    wxT("stallunknowns")
#define RAWXML_SID_PARAMETER        wxT("parameter")

#define RAWXML_IMPORT    wxT("import")
#define RAWXML_REFERENCE wxT("reference")
#define RAWXML_SYMBOL    wxT("symbol")

#define OPTION_PARSE(t, v, p) \
    try { \
        t x = p; \
        v = x; \
    } catch (RCT3InvalidValueException) { \
        throw; \
    } catch (RCT3Exception) {}

#define OPTION_PARSE_STRING(v, node, a) \
    { \
        wxString t = node->GetPropVal(a, wxT("")); \
        if (!t.IsEmpty()) \
            v = t.c_str(); \
    }

class RCT3InvalidValueException: public RCT3Exception {
public:
    RCT3InvalidValueException(const wxString& message):RCT3Exception(message){};
};

cRawOvl::cRawOvl(const wxFileName& file, const wxFileName& outputdir, const wxFileName& output) {
    m_input = file;
    m_output = output;
    if (outputdir == wxT("")) {
        m_outputbasedir.SetPath(file.GetPathWithSep());
    } else {
        m_outputbasedir.SetPath(outputdir.GetPathWithSep());
    }
    wxXmlDocument doc;
    if (!doc.Load(file.GetFullPath()))
        throw RCT3Exception(wxT("Error loading xml file ")+file.GetFullPath());
    Load(doc.GetRoot());
}

cRawOvl::cRawOvl(wxXmlNode* root, const wxFileName& file, const wxFileName& outputdir, const wxFileName& output) {
    m_input = file;
    m_output = output;
    if (outputdir == wxT("")) {
        m_outputbasedir.SetPath(file.GetPathWithSep());
    } else {
        m_outputbasedir.SetPath(outputdir.GetPathWithSep());
    }
    Load(root);
}

cOvlType cRawOvl::ParseType(wxXmlNode* node, const wxString& nodes, const wxString& attribute) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    if (t == wxT("common")) {
        return OVLT_COMMON;
    } else if (t == wxT("unique")) {
        return OVLT_UNIQUE;
    } else {
        throw RCT3InvalidValueException(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t);
    }
}

wxString cRawOvl::ParseString(wxXmlNode* node, const wxString& nodes, const wxString& attribute) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    return t;
}

unsigned long cRawOvl::ParseUnsigned(wxXmlNode* node, const wxString& nodes, const wxString& attribute) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    unsigned long i;
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    if (!t.ToULong(&i))
        throw RCT3InvalidValueException(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t);
    return i;
}

long cRawOvl::ParseSigned(wxXmlNode* node, const wxString& nodes, const wxString& attribute) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    long i;
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    if (!t.ToLong(&i))
        throw RCT3InvalidValueException(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t);
    return i;
}

double cRawOvl::ParseFloat(wxXmlNode* node, const wxString& nodes, const wxString& attribute) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    double i;
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    if (!t.ToDouble(&i))
        throw RCT3InvalidValueException(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t);
    return i;
}

void cRawOvl::ParseSID(wxXmlNode* node) {
    cSid sid;
    sid.name = ParseString(node, RAWXML_SID, wxT("name")).c_str();
    sid.ovlpath = ParseString(node, RAWXML_SID, wxT("ovlpath")).c_str();
    sid.ui.name = ParseString(node, RAWXML_SID, wxT("nametxt")).c_str();
    sid.ui.icon = ParseString(node, RAWXML_SID, wxT("icon")).c_str();
    wxString svd = ParseString(node, RAWXML_SID, wxT("svd"));
    sid.svds.push_back(svd.c_str());

    wxXmlNode *child = node->GetChildren();
    while (child) {
        if (child->GetName() == RAWXML_SID_GROUP) {
            wxString name = ParseString(child, RAWXML_SID_GROUP, wxT("name"));
            wxString icon = ParseString(child, RAWXML_SID_GROUP, wxT("icon"));
            sid.ui.group = name.c_str();
            sid.ui.groupicon = icon.c_str();
        } else if (child->GetName() == RAWXML_SID_TYPE) {
            OPTION_PARSE(unsigned long, sid.ui.type, ParseUnsigned(child, RAWXML_SID_TYPE, wxT("scenerytype")));
            OPTION_PARSE(long, sid.ui.cost, ParseSigned(child, RAWXML_SID_TYPE, wxT("cost")));
            OPTION_PARSE(long, sid.ui.removal_cost, ParseSigned(child, RAWXML_SID_TYPE, wxT("removalcost")));
        } else if (child->GetName() == RAWXML_SID_POSITION) {
            OPTION_PARSE(unsigned long, sid.position.positioningtype, ParseUnsigned(child, RAWXML_SID_POSITION, wxT("type")));
            OPTION_PARSE(unsigned long, sid.position.xsquares, ParseUnsigned(child, RAWXML_SID_POSITION, wxT("xsquares")));
            OPTION_PARSE(unsigned long, sid.position.ysquares, ParseUnsigned(child, RAWXML_SID_POSITION, wxT("ysquares")));
            OPTION_PARSE(float, sid.position.xpos, ParseFloat(child, RAWXML_SID_POSITION, wxT("xpos")));
            OPTION_PARSE(float, sid.position.ypos, ParseFloat(child, RAWXML_SID_POSITION, wxT("ypos")));
            OPTION_PARSE(float, sid.position.zpos, ParseFloat(child, RAWXML_SID_POSITION, wxT("zpos")));
            OPTION_PARSE(float, sid.position.xsize, ParseFloat(child, RAWXML_SID_POSITION, wxT("xsize")));
            OPTION_PARSE(float, sid.position.ysize, ParseFloat(child, RAWXML_SID_POSITION, wxT("ysize")));
            OPTION_PARSE(float, sid.position.zsize, ParseFloat(child, RAWXML_SID_POSITION, wxT("zsize")));
            OPTION_PARSE_STRING(sid.position.supports, child, wxT("supports"));
        } else if (child->GetName() == RAWXML_SID_COLOURS) {
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[0], ParseUnsigned(child, RAWXML_SID_COLOURS, wxT("choice1")));
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[1], ParseUnsigned(child, RAWXML_SID_COLOURS, wxT("choice2")));
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[2], ParseUnsigned(child, RAWXML_SID_COLOURS, wxT("choice3")));
        } else if (child->GetName() == RAWXML_SID_FLAGS) {
            wxString flags = ParseString(child, RAWXML_SID_FLAGS, wxT("set"));
            if (flags.Length() != 64)
                throw RCT3InvalidValueException(_("The set attribute of a sid/flags tag is of invalid length"));
            const char* fl = flags.c_str();
            for (int i = 0; i < 64; ++i) {
                if (fl[i] == '0') {
                    sid.flags.flag[i] = false;
                } else if (fl[i] == '1') {
                    sid.flags.flag[i] = true;
                } else {
                    throw RCT3InvalidValueException(_("The set attribute of a sid/flags tag has an invalid character"));
                }
            }
        } else if (child->GetName() == RAWXML_SID_IMPORTERUNKNOWNS) {
            OPTION_PARSE(unsigned long, sid.importerunknowns.unk1, ParseUnsigned(child, RAWXML_SID_IMPORTERUNKNOWNS, wxT("u1")));
            OPTION_PARSE(unsigned long, sid.importerunknowns.unk2, ParseUnsigned(child, RAWXML_SID_IMPORTERUNKNOWNS, wxT("u2")));
            OPTION_PARSE(unsigned long, sid.importerunknowns.unk6, ParseUnsigned(child, RAWXML_SID_IMPORTERUNKNOWNS, wxT("u6")));
            OPTION_PARSE(unsigned long, sid.importerunknowns.unk7, ParseUnsigned(child, RAWXML_SID_IMPORTERUNKNOWNS, wxT("u7")));
            if (child->HasProp(wxT("u8"))) {
                OPTION_PARSE(unsigned long, sid.importerunknowns.unk8, ParseUnsigned(child, RAWXML_SID_IMPORTERUNKNOWNS, wxT("u8")));
                sid.importerunknowns.use_unk8 = true;
            }
            OPTION_PARSE(unsigned long, sid.importerunknowns.unk9, ParseUnsigned(child, RAWXML_SID_IMPORTERUNKNOWNS, wxT("u9")));
        } else if (child->GetName() == RAWXML_SID_STALLUNKNOWNS) {
            sid.stallunknowns.MakeStall();
            OPTION_PARSE(unsigned long, sid.stallunknowns.unk1, ParseUnsigned(child, RAWXML_SID_STALLUNKNOWNS, wxT("u1")));
            OPTION_PARSE(unsigned long, sid.stallunknowns.unk2, ParseUnsigned(child, RAWXML_SID_STALLUNKNOWNS, wxT("u2")));
        } else if (child->GetName() == RAWXML_SVD) {
            svd = ParseString(node, RAWXML_SVD, wxT("name"));
            sid.svds.push_back(svd.c_str());
        } else if (child->GetName() == RAWXML_SID_PARAMETER) {
            cSidParam param;
            param.name = ParseString(child, RAWXML_SID_GROUP, wxT("name")).c_str();
            OPTION_PARSE_STRING(param.param, child, wxT("param"));
            sid.parameters.push_back(param);
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in sid tag."), child->GetName().c_str()));
        }

        child = child->GetNext();
    }

    ovlSIDManager* c_sid = m_ovl.GetManager<ovlSIDManager>();
    c_sid->AddSID(sid);
}

void cRawOvl::ParseTEX(wxXmlNode* node) {
    wxString name = ParseString(node, RAWXML_TEX, wxT("name"));
    wxFileName texture = ParseString(node, RAWXML_TEX, wxT("texture"));
    if (!texture.IsAbsolute())
        texture.MakeAbsolute(m_input.GetPathWithSep());
    unsigned char* data = NULL;
    if (!texture.IsFileReadable())
        throw RCT3Exception(_("tex tag: File not readable: ") + texture.GetFullPath());

    try {
        wxGXImage img(texture.GetFullPath(), false);

        int width = img.GetWidth();
        int height = img.GetHeight();
        if (width != height) {
            throw Magick::Exception("Icon texture is not square");
        }
        if ((1 << local_log2(width)) != width) {
            throw Magick::Exception("Icon texture's width/height is not a power of 2");
        }
        img.flip();
        img.dxtCompressionMethod(squish::kColourIterativeClusterFit | squish::kWeightColourByAlpha);
        int datasize = img.GetDxtBufferSize(wxDXT3);
        data = new unsigned char[datasize];
        img.DxtCompress(data, wxDXT3);

        ovlTEXManager* c_tex = m_ovl.GetManager<ovlTEXManager>();
        c_tex->AddTexture(name.c_str(), width, datasize, reinterpret_cast<unsigned long*>(data));
    } catch (Magick::Exception& e) {
        delete[] data;
        throw RCT3Exception(wxString::Format(_("tex tag: GarphicsMagik exception: %s"), e.what()));
    }
    delete[] data;
}

void cRawOvl::Load(wxXmlNode* root) {
    if (!root)
        throw RCT3Exception(wxT("cRawOvl::Load, root is null"));
    bool subonly = false;

    if ((root->GetName() == RAWXML_ROOT) || (root->GetName() == RAWXML_SUBROOT)) {
        // <rawovl|subovl file="outputfile" basedir="dir outputfile is relative to">
        {
            wxString basedir = root->GetPropVal(wxT("basedir"), wxT(""));
            if (!basedir.IsEmpty()) {
                wxFileName temp;
                temp.SetPath(basedir);
                if (!temp.IsAbsolute()) {
                    temp.MakeAbsolute(m_outputbasedir.GetPathWithSep());
                }
                m_outputbasedir = temp;
            }
        }

        if (!m_outputbasedir.DirExists()) {
            //if (!::wxMkDir(m_outputbasedir.GetPathWithSep()))
            //    throw RCT3Exception(_("Failed to create directory: ")+m_outputbasedir.GetPathWithSep());
            //m_outputbasedir.Mkdir(0777, wxPATH_MKDIR_FULL);
            if (!m_outputbasedir.Mkdir(0777, wxPATH_MKDIR_FULL))
                throw RCT3Exception(_("Failed to create directory: ")+m_output.GetPathWithSep());
        }
        if (m_output == wxT("")) {
            /*
            wxString op;
            if (!root->GetPropVal(wxT("file"), &op)) {
                // No guessing output name
                throw RCT3Exception(wxT("cRawOvl::Load, no output file given"));
            }
            m_output = op;
            */
            m_output = root->GetPropVal(wxT("file"), wxT(""));
        }

        if (m_output != wxT("")) {
            if (!m_output.IsAbsolute())
                m_output.MakeAbsolute(m_outputbasedir.GetPathWithSep());
            m_output.SetExt(wxT(""));
            if (m_output.GetName().EndsWith(wxT(".common"))) {
                wxString temp = m_output.GetName();
                temp.Replace(wxT(".common"), wxT(""), true);
                m_output.SetName(temp);
            }

            if (!m_output.DirExists()) {
                //if (!::wxMkDir(m_output.GetPathWithSep()))
                //    throw RCT3Exception(_("Failed to create directory: ")+m_output.GetPathWithSep());
                // Doesn't report success correctly -.-
                //::wxMkDir(m_output.GetPathWithSep());
                if (!m_output.Mkdir(0777, wxPATH_MKDIR_FULL))
                    throw RCT3Exception(_("Failed to create directory: ")+m_output.GetPathWithSep());
            }

            {
                wxFileName temp = m_output;
                temp.SetExt(wxT("common.ovl"));
                if (temp.GetPath().IsEmpty())
                    temp.SetPath(wxT("."));
                if (temp.FileExists()) {
                    if (!temp.IsFileWritable())
                        throw RCT3Exception(_("Cannot write output file ")+temp.GetFullPath());
                } else {
                    if (!temp.IsDirWritable())
                        throw RCT3Exception(_("Cannot write output file ")+temp.GetFullPath());
                }
            }

            m_ovl.Init(m_output.GetFullPath().c_str());
            wxLogMessage(_("Writing ovl from raw: ")+m_output.GetFullPath()+wxT(".common.ovl"));
        } else {
            subonly = true;
            wxLogMessage(_("Parsing subovl-only raw. Base output dir: ")+m_outputbasedir.GetPathWithSep());
        }

        // Start parsing
        wxXmlNode *child = root->GetChildren();
        while (child) {
            if (child->GetName() == RAWXML_SUBROOT) {
                // see above or
                // <subovl include="raw xml file" />
                wxString incfile = child->GetPropVal(wxT("include"), wxT(""));
                if (incfile.IsEmpty()) {
                    cRawOvl c_raw(child, m_input, m_outputbasedir);
                } else {
                    cRawOvl c_raw(incfile, m_input, m_outputbasedir);
                }
            } else if (child->GetName() == RAWXML_IMPORT) {
                // <import file="scenery file" (name="internal svd name") />
                wxFileName filename = ParseString(child, RAWXML_IMPORT, wxT("file"));
                wxString name = child->GetPropVal(wxT("name"), wxT(""));
                if (!filename.IsAbsolute())
                    filename.MakeAbsolute(m_input.GetPathWithSep());
                cSCNFile c_scn(filename.GetFullPath());
                if (!name.IsEmpty()) {
                    c_scn.name = name;
                }
                c_scn.MakeToOvl(m_ovl);
            } else if (child->GetName() == RAWXML_GSI) {
                // <gsi name="string" tex="string" top="long int" left="long int" bottom="long int" right="long int" />
                wxString name = ParseString(child, RAWXML_GSI, wxT("name"));
                wxString tex = ParseString(child, RAWXML_GSI, wxT("tex"));
                unsigned long top = ParseUnsigned(child, RAWXML_GSI, wxT("top"));
                unsigned long left = ParseUnsigned(child, RAWXML_GSI, wxT("left"));
                unsigned long bottom = ParseUnsigned(child, RAWXML_GSI, wxT("bottom"));
                unsigned long right = ParseUnsigned(child, RAWXML_GSI, wxT("right"));

                ovlGSIManager* c_gsi = m_ovl.GetManager<ovlGSIManager>();
                c_gsi->AddItem(name.c_str(), tex.c_str(), left, top, right, bottom);
            } else if (child->GetName() == RAWXML_SID) {
                // <sid name="string" ovlpath="ovl path, relative to install dir" nametxt="txt" icon="gsi" svd="svd">
                //     (All following tags and attributes are optional, if not mentioned otherwise)
                //     <group name="txt" icon="gsi" /> (both name and gsi must be present)
                //     <type scenerytype="long int" cost="long int" removalcost="long int" />
                //       (costs are signed integers)
                //     <position type="short int" xsquares="long int" ysquares="long int"
                //               xpos="float" ypos="float" zpos="float"
                //               xsize="float" ysize="float" zsize="float"
                //               supports="string" />
                //     <colours choice1="long int" choice2="long int" choice3="long int" />
                //     <flags set="1010101010101010101010101010101010101010101010101010101010101010" />
                //       (flags are all or nothing, 64 0's and 1's. 1 set, 0 not set. 1-64 from left to right)
                //     <importerunknowns u1="long int" u2="long int" u6="long int" u7="long int" u8="long int" u9="long int" />
                //       (if you leave out u8, it will be disabled)
                //     <stallunknowns u1="long int" u2="long int" />
                //       (if you leave out both attributes, the stall defaults will be used (1440/2880))
                //     (The next tags can be used multiple times)
                //     <svd name="svd" />
                //       (add another svd to the sid. As far as I know only used for trees (2d/3d versions).
                //     <parameter name="string" param="string" />
                //       (add a parameter. name is required, param is optional in most cases. If present, it usually starts with a space.)
                // </sid>
                ParseSID(child);
            } else if (child->GetName() == RAWXML_TEX) {
                // <tex name="string" texture="texture file" />
                ParseTEX(child);
            } else if (child->GetName() == RAWXML_TXT) {
                // <txt name="string" text="string" />
                wxString name = ParseString(child, RAWXML_TXT, wxT("name"));
                wxString text = ParseString(child, RAWXML_TXT, wxT("text"));

                ovlTXTManager* c_txt = m_ovl.GetManager<ovlTXTManager>();
                c_txt->AddText(name.c_str(), text.c_str());
            } else if (child->GetName() == RAWXML_REFERENCE) {
                // <reference path="relative path" />
                wxString ref = child->GetPropVal(wxT("path"), wxT(""));
                if (ref.IsEmpty())
                    throw RCT3Exception(_("REFERENCE tag misses path attribute."));
                m_ovl.AddReference(ref);
            } else if (child->GetName() == RAWXML_SYMBOL) {
                // <symbol target="common|unique" name="string" type="int|float" data="data" />
                cOvlType target = ParseType(child, RAWXML_SYMBOL);
                wxString name = ParseString(child, RAWXML_SYMBOL, wxT("name"));
                wxString type = ParseString(child, RAWXML_SYMBOL, wxT("type"));
                unsigned long data;
                if (type == wxT("int")) {
                    name += wxT(":int");
                    data = ParseUnsigned(child, RAWXML_SYMBOL, wxT("data"));
                } else if (type == wxT("float")) {
                    name += wxT(":flt");
                    float f = ParseFloat(child, RAWXML_SYMBOL, wxT("data"));
                    data = *reinterpret_cast<unsigned long*>(&f);
                } else {
                    throw RCT3Exception(_("symbol tag has unimplemented type value."));
                }
                m_ovl.AddDataSymbol(target, name.c_str(), data);
            } else if COMPILER_WRONGTAG(child) {
                throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in rawovl tag."), child->GetName().c_str()));
            }

            child = child->GetNext();
        }

        if (!subonly) {
            m_ovl.Save();
            wxLogMessage(_("OVL written successfully"));
        }
    } else {
        throw RCT3Exception(wxT("cRawOvl::Load, wrong root"));
    }

}
