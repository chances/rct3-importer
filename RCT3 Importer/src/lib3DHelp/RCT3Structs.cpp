///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// C++ structures for the importer
// Copyright (C) 2006 Tobias Minch
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

#include "RCT3Structs.h"

#include <boost/format.hpp>

#include "confhelp.h"
#include "lib3Dconfig.h"
#include "matrix.h"
#include "ManagerCommon.h"
#include "RCT3Exception.h"
#include "texcheck.h"
#include "gximage.h"
#include "xmldefs.h"
#include "xmlhelper.h"

#include "bmy.pal.h"

using namespace r3;
using namespace xmlcpp;

///////////////////////////////////////////////////////////////
//
// Matrix Xml Helpers
//
///////////////////////////////////////////////////////////////

bool XmlParseMatrixNode(cXmlNode& node, MATRIX* matrix, wxString* name, unsigned long version) {
    bool ret = true;
    if (!node)
        return false;
    if (!node(RCT3XML_MATRIX))
        return false;

    if (node.hasProp("name")) {
        *name = node.wxgetPropVal("name");
    } else {
        ret = false;
    }
    *matrix = matrixGetUnity();

    int rows = 0;
    std::string temp;
    cXmlNode child(node.children());
    while(child) {
        if (child("row1")) {
            temp = child();
            if (!parseMatrixRow(temp, matrix->_11, matrix->_12, matrix->_13, matrix->_14)) {
                wxLogError(_("Parsing matrix row 1 failed."));
                ret = false;
            }
            rows++;
        } else if (child("row2")) {
            temp = child();
            if (!parseMatrixRow(temp, matrix->_21, matrix->_22, matrix->_23, matrix->_24)) {
                wxLogError(_("Parsing matrix row 2 failed."));
                ret = false;
            }
            rows++;
        } else if (child("row3")) {
            temp = child();
            if (!parseMatrixRow(temp, matrix->_31, matrix->_32, matrix->_33, matrix->_34)) {
                wxLogError(_("Parsing matrix row 3 failed."));
                ret = false;
            }
            rows++;
        } else if (child("row4")) {
            temp = child();
            if (!parseMatrixRow(temp, matrix->_41, matrix->_42, matrix->_43, matrix->_44)) {
                wxLogError(_("Parsing matrix row 4 failed."));
                ret = false;
            }
            rows++;
        }
        ++child;
    }
    if (rows != 4) {
        wxLogError(_("Matrix lacks rows."));
        return false;
    }
    return ret;
}

cXmlNode XmlMakeMatrixNode(const MATRIX& matrix, const wxString& name) {
    cXmlNode node(RCT3XML_MATRIX);
    if (name != wxT(""))
        node.prop("name", name);

    cXmlNode row1("row1", boost::str(boost::format("%8f %8f %8f %8f") % matrix._11 % matrix._12 % matrix._13 % matrix._14).c_str());
    node.appendChildren(row1);

    cXmlNode row2("row2", boost::str(boost::format("%8f %8f %8f %8f") % matrix._21 % matrix._22 % matrix._23 % matrix._24).c_str());
    node.appendChildren(row2);

    cXmlNode row3("row3", boost::str(boost::format("%8f %8f %8f %8f") % matrix._31 % matrix._32 % matrix._33 % matrix._34).c_str());
    node.appendChildren(row3);

    cXmlNode row4("row4", boost::str(boost::format("%8f %8f %8f %8f") % matrix._41 % matrix._42 % matrix._43 % matrix._44).c_str());
    node.appendChildren(row4);

//    wxXmlNode* lastnode;
//    //wxXmlNode* childnode = new wxXmlNode(node, wxXML_ELEMENT_NODE, wxT("row1"));
//    //wxXmlNode* textnode = new wxXmlNode(childnode, wxXML_TEXT_NODE, wxT(""));
//    wxXmlNode* childnode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("row1"));
//    wxXmlNode* textnode = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""));
//    textnode->SetContent(wxString::Format(wxT("%8f %8f %8f %8f"), matrix._11, matrix._12, matrix._13, matrix._14));
//    childnode->SetChildren(textnode);
//    node->SetChildren(childnode);
//    lastnode = childnode;
//
//    //childnode = new wxXmlNode(node, wxXML_ELEMENT_NODE, wxT("row2"));
//    //textnode = new wxXmlNode(childnode, wxXML_TEXT_NODE, wxT(""));
//    childnode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("row2"));
//    textnode = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""));
//    textnode->SetContent(wxString::Format(wxT("%8f %8f %8f %8f"), matrix._21, matrix._22, matrix._23, matrix._24));
//    childnode->SetChildren(textnode);
//    lastnode->SetNext(childnode);
//    lastnode = childnode;
//
//    //childnode = new wxXmlNode(node, wxXML_ELEMENT_NODE, wxT("row3"));
//    //textnode = new wxXmlNode(childnode, wxXML_TEXT_NODE, wxT(""));
//    childnode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("row3"));
//    textnode = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""));
//    textnode->SetContent(wxString::Format(wxT("%8f %8f %8f %8f"), matrix._31, matrix._32, matrix._33, matrix._34));
//    childnode->SetChildren(textnode);
//    lastnode->SetNext(childnode);
//    lastnode = childnode;
//
//    //childnode = new wxXmlNode(node, wxXML_ELEMENT_NODE, wxT("row4"));
//    //textnode = new wxXmlNode(childnode, wxXML_TEXT_NODE, wxT(""));
//    childnode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("row4"));
//    textnode = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""));
//    textnode->SetContent(wxString::Format(wxT("%8f %8f %8f %8f"), matrix._41, matrix._42, matrix._43, matrix._44));
//    childnode->SetChildren(textnode);
//    lastnode->SetNext(childnode);
//    lastnode = childnode;

    return node;
}


///////////////////////////////////////////////////////////////
//
// TXYZ Xml Helpers
//
///////////////////////////////////////////////////////////////

#define TXYZ_READPROP(pr, s) \
    temp = node.getPropVal(pr, "0.0"); \
    if (!parseFloat(temp, v.s)) { \
        ret = false; \
        wxLogError(_("Parsing txyz property '" pr "' failed.")); \
        v.s = 0.0; \
    }

bool cTXYZ::FromNode(cXmlNode& node, const wxString& path, unsigned long version) {
    bool ret = true;
    std::string temp;
    if (!node)
        return false;
    if (!node(RCT3XML_CTXYZ))
        return false;

    TXYZ_READPROP("time", Time)
    TXYZ_READPROP("x", X)
    TXYZ_READPROP("y", Y)
    TXYZ_READPROP("z", Z)

    return ret;
}

cXmlNode cTXYZ::GetNode(const wxString& path) {
    cXmlNode node(RCT3XML_CTXYZ);

    node.prop("time", boost::str(boost::format("%8f") % v.Time).c_str());
    node.prop("x", boost::str(boost::format("%8f") % v.X));
    node.prop("y", boost::str(boost::format("%8f") % v.Y));
    node.prop("z", boost::str(boost::format("%8f") % v.Z));

    return node;
}
/*
txyz& operator= (txyz& t, cTXYZ& v) {
    t = v.v;
    return t;
}
*/

txyz cTXYZ::GetFixed(c3DLoaderOrientation ori) const {
    txyz r = v;
    txyzFixOrientation(r, ori);
    return r;
}

///////////////////////////////////////////////////////////////
//
// cMeshStruct
//
///////////////////////////////////////////////////////////////

void cMeshStruct::Init() {
    disabled = true;
    TXS = wxT("SIOpaque");
    FTX = wxT("");
    place = r3::Constants::Mesh::Transparency::None;
    flags = 0;
    unknown = r3::Constants::Mesh::Sides::Singlesided;
    Name = wxT("");
    fudgenormals = 0;
    valid = false;
    faces = 0;
    effectpoint = 0;
    bone = 0;
}

void cMeshStruct::CopySettingsFrom(const cMeshStruct& from) {
    disabled = from.disabled;
    FTX = from.FTX;
    TXS = from.TXS;
    flags = from.flags;
    place = from.place;
    unknown = from.unknown;
    fudgenormals = from.fudgenormals;

    bone = from.bone;
    bonename = from.bonename;

    algo_x = from.algo_x;
    algo_y = from.algo_y;
    algo_z = from.algo_z;
}

bool cMeshStruct::FromCompilerXml(cXmlNode& node, const wxString& path) {
    if (!node)
        return false;

    bone = 0;
    disabled = false;
    flags = 0;

    Name = wxString::FromUTF8(node.getPropVal("name").c_str());
    if (Name.IsEmpty())
        throw RCT3Exception(_("GEOMOBJ tag lacks name attribute"));

    unsigned long t;
    std::string temp;

    if (node.hasProp("bone")) {
        temp = node.getPropVal("bone");
        if (parseULong(temp, t)) {
            bone = t;
        } else {
            bonename = wxString::FromUTF8(temp.c_str());
            bone = 0xFFFFFFFF;
        }
    }

    unknown = (node.getPropVal("doublesided", "0") == "1")?1:3;

    wxString sflags = wxString::FromUTF8(node.getPropVal("flags").c_str()).Lower();
    FTX = wxString::FromUTF8(node.getPropVal("ftx").c_str());
    TXS = wxString::FromUTF8(node.getPropVal("txs").c_str());

    if (sflags.IsSameAs(wxT("sign"))) {
        flags = 12;
        TXS = wxT("SIOpaqueUnlit");
    } else if (sflags.IsSameAs(wxT("terrain"))) {
        flags = 12288;
        FTX = wxT("useterraintexture");
        TXS = wxT("SIOpaque");
    } else if (sflags.IsSameAs(wxT("cliff"))) {
        flags = 20480;
        FTX = wxT("useclifftexture");
        TXS = wxT("SIOpaque");
    } else if (sflags.IsSameAs(wxT("water"))) {
        FTX = wxT("siwater");
        TXS = wxT("SIWater");
    } else if (sflags.IsSameAs(wxT("watermask"))) {
        FTX = wxT("watermask");
        TXS = wxT("SIFillZ");
    } else if (sflags.IsSameAs(wxT("billboard"))) {
        flags = 32788;
        FTX = wxT("UseAdTexture");
        TXS = wxT("SIOpaque");
    } else if (sflags.IsSameAs(wxT("animatedbillboard"))) {
        flags = 32768;
        FTX = wxT("UseAdTexture");
        TXS = wxT("SIOpaque");
    } else if (!sflags.IsEmpty()) {
        throw RCT3Exception(wxString::Format(_("GEOMOBJ '%s': Unknown flags value '%s'."), Name.c_str(), sflags.c_str()));
    }

    if (FTX.IsEmpty() || TXS.IsEmpty()) {
        throw RCT3Exception(wxString::Format(_("GEOMOBJ '%s': Missing ftx or txs attribute."), Name.c_str()));
    }

    if (node.hasProp("placing")) {
        wxString placing = wxString::FromUTF8(node.getPropVal("placing", "both").c_str()).Lower();
        if (placing.IsSameAs(wxT("texture"))) {
            place = 1;
        } else if (placing.IsSameAs(wxT("glass"))) {
            place = 2;
        } else if (placing.IsSameAs(wxT("both"))) {
            place = 0;
        } else if (placing.IsSameAs(wxT("transparent"))) {
            place = 1;
        } else if (placing.IsSameAs(wxT("glassy"))) {
            place = 2;
        } else if (placing.IsSameAs(wxT("solid"))) {
            place = 0;
        } else {
            throw RCT3Exception(wxString::Format(_("GEOMOBJ '%s': Unknown placing value '%s'."), Name.c_str(), placing.c_str()));
        }
        wxLogWarning(_("The 'placing' attribute is deprecated. Please use 'transparency'."));
    } else {
        if (node.hasProp("transparency")) {
            wxString placing = wxString::FromUTF8(node.getPropVal("transparency", "none").c_str()).Lower();
            if (placing.IsSameAs(wxT("masked"))) {
                place = 1;
            } else if (placing.IsSameAs(wxT("regular"))) {
                place = 2;
            } else if (placing.IsSameAs(wxT("none"))) {
                place = 0;
            } else {
                throw RCT3Exception(wxString::Format(_("GEOMOBJ '%s': Unknown transparency value '%s'."), Name.c_str(), placing.c_str()));
            }
        } else {
            unsigned long right_value = 0;
            if (TXS.StartsWith(wxT("SIAlphaMask")))
                right_value = 1;
            else if (TXS.StartsWith(wxT("SIAlpha")) || TXS.IsSameAs(wxT("SIGlass")) || TXS.IsSameAs(wxT("SIGlass")))
                right_value = 2;
            place = right_value;
        }
    }

    if (node.getPropVal("sortx", &temp)) {
        algo_x = wxString::FromUTF8(temp.c_str());
    }
    if (node.getPropVal("sorty", &temp)) {
        algo_y = wxString::FromUTF8(temp.c_str());
    }
    if (node.getPropVal("sortz", &temp)) {
        algo_z = wxString::FromUTF8(temp.c_str());
    }

    return true;
}

void cMeshStruct::Check(const wxString& modelname) {
    unsigned long right_value = 0;
    if (TXS.StartsWith(wxT("SIAlphaMask")))
        right_value = 1;
    else if (TXS.StartsWith(wxT("SIAlpha")) || TXS.IsSameAs(wxT("SIGlass")) || TXS.IsSameAs(wxT("SIGlass")))
        right_value = 2;
    if ((place != right_value) && (!READ_RCT3_EXPERTMODE()))
        wxLogWarning(wxString::Format(_("Model '%s', Group '%s': Texture style %s and transparency setting do not match."), modelname.c_str(), Name.c_str(), TXS.c_str()));
}

bool cMeshStruct::FromNode(cXmlNode& node, const wxString& path, unsigned long version) {
    bool ret = true;
    std::string temp;
    if (!node)
        return false;
    if (!node(RCT3XML_CMESHSTRUCT))
        return false;

    disabled = node.getPropVal("disabled", "0") == "1";
    TXS = wxString::FromUTF8(node.getPropVal("txs", "SIOpaque").c_str());
    FTX = wxString::FromUTF8(node.getPropVal("ftx").c_str());

    if (node.hasProp("place")) {
        temp = node.getPropVal("place");
        if (parseULong(temp, place)) {
            if (place > r3::Constants::Mesh::Transparency::Complex) {
                place = r3::Constants::Mesh::Transparency::None;
                wxLogError(_("Mesh, place has illegal value."));
                ret = false;
            }
        } else {
            place = r3::Constants::Mesh::Transparency::None;
            wxLogError(_("Mesh, place failed parsing."));
            ret = false;
        }
    } else {
        place = r3::Constants::Mesh::Transparency::None;
    }
    if (node.hasProp("transparency")) {
        temp = node.getPropVal("transparency");
        if (parseULong(temp, place)) {
            if (place > r3::Constants::Mesh::Transparency::Complex) {
                place = r3::Constants::Mesh::Transparency::None;
                wxLogError(_("Mesh, transparency has illegal value."));
                ret = false;
            }
        } else {
            place = r3::Constants::Mesh::Transparency::None;
            wxLogError(_("Mesh, transparency failed parsing."));
            ret = false;
        }
    }
    if (node.hasProp("flags")) {
        temp = node.getPropVal("flags");
        if (!parseULong(temp, flags)) {
            flags = 0;
            wxLogError(_("Mesh, flags failed parsing."));
            ret = false;
        }
    } else {
        flags = 0;
    }
    if (node.hasProp("unknown")) {
        temp = node.getPropVal("unknown");
        if (!parseULong(temp, unknown)) {
            unknown = r3::Constants::Mesh::Sides::Singlesided;
            wxLogError(_("Mesh, unknown failed parsing."));
            ret = false;
        }
    } else {
        unknown = r3::Constants::Mesh::Sides::Singlesided;
    }
    if (node.hasProp("fudgenormals")) {
        temp = node.getPropVal("fudgenormals");
        if (!parseULong(temp, fudgenormals)) {
            fudgenormals = 0;
            wxLogError(_("Mesh, fudgenormals failed parsing."));
            ret = false;
        } else {
            if (fudgenormals >= CMS_FUDGE_ENUMSIZE) {
                fudgenormals = 0;
                wxLogError(_("Mesh, fudgenormals has illegal value."));
                ret = false;
            }
        }
    } else {
        fudgenormals = 0;
    }

    if (node.getPropVal("name", &temp)) {
        Name = wxString::FromUTF8(temp.c_str());
    } else {
        Name = _("Missing!!!");
        wxLogError(_("Mesh, name missing."));
        ret = false;
    }
    if (node.getPropVal("sortx", &temp)) {
        algo_x = wxString::FromUTF8(temp.c_str());
    }
    if (node.getPropVal("sorty", &temp)) {
        algo_y = wxString::FromUTF8(temp.c_str());
    }
    if (node.getPropVal("sortz", &temp)) {
        algo_z = wxString::FromUTF8(temp.c_str());
    }

    return ret;
}

cXmlNode cMeshStruct::GetNode(const wxString& path) {
    cXmlNode node(RCT3XML_CMESHSTRUCT);
    if (disabled)
        node.prop("disabled", "1");
    if (!TXS.IsEmpty())
        node.prop("txs", TXS);
    if (!FTX.IsEmpty())
        node.prop("ftx", FTX);
    node.prop("transparency", boost::str(boost::format("%lu") % place).c_str());
    if (flags)
        node.prop("flags", boost::str(boost::format("%lu") % flags).c_str());
    if (unknown != 3)
        node.prop("unknown", boost::str(boost::format("%lu") % unknown).c_str());
    if (fudgenormals)
        node.prop("fudgenormals", boost::str(boost::format("%lu") % fudgenormals).c_str());
    node.prop("name", Name);
    if (!algo_x.IsEmpty())
        node.prop("sortx", algo_x);
    if (!algo_y.IsEmpty())
        node.prop("sorty", algo_y);
    if (!algo_z.IsEmpty())
        node.prop("sortz", algo_z);
    return node;
}


///////////////////////////////////////////////////////////////
//
// cFlexiTextureAnim
//
///////////////////////////////////////////////////////////////

bool cFlexiTextureAnim::FromNode(cXmlNode& node, const wxString& path, unsigned long version) {
    bool ret = true;
    std::string temp;
    unsigned long t;

    if (!node)
        return false;
    if (!node(RCT3XML_CFLEXITEXTUREANIM))
        return false;

    if (node.getPropVal("frame", &temp)) {
        if (!parseULong(temp, t)) {
            frame(0);
            wxLogError(_("FlexiTextureAnim, frame failed parsing."));
            ret = false;
        } else {
            frame(t);
        }
    } else {
        frame(0);
    }
    if (node.getPropVal("count", &temp)) {
        if (!parseULong(temp, t)) {
            count(1);
            wxLogError(_("FlexiTextureAnim, count failed parsing."));
            ret = false;
        } else {
            count(t);
        }
    } else {
        count(1);
    }

    return ret;
}

cXmlNode cFlexiTextureAnim::GetNode(const wxString& path) {
    cXmlNode node(RCT3XML_CFLEXITEXTUREANIM);
    node.prop("frame", boost::str(boost::format("%lu") % frame()).c_str());
    if (count() != 1)
        node.prop("count", boost::str(boost::format("%lu") % count()).c_str());
    return node;
}


///////////////////////////////////////////////////////////////
//
// cFlexiTextureFrame
//
///////////////////////////////////////////////////////////////

bool cFlexiTextureFrame::Check(cFlexiTexture* parent) {
    bool ret = true;
    try {
        checkRCT3Texture(texture().GetFullPath());
    } catch (RCT3TextureException& e) {
        throw RCT3Exception(wxString::Format(_("Texture '%s', file '%s': %s"), parent->Name.c_str(), texture().GetFullPath().fn_str(), e.what()));
    }

    if (alphasource() == CFTF_ALPHA_EXTERNAL) {
        try {
            checkRCT3Texture(alpha().GetFullPath());
        } catch (RCT3TextureException& e) {
            alphasource(CFTF_ALPHA_NONE);
            ret = false;
            wxLogWarning(_("Texture '%s', file '%s': Problem with alpha channel file. Alpha deactivated. (Problem was %s)"), parent->Name.c_str(), alpha().GetFullPath().fn_str(), e.what());
        }
    } else if (alphasource() == CFTF_ALPHA_INTERNAL) {
        wxGXImage img(texture().GetFullPath());
        if (!img.HasAlpha()) {
            alphasource(CFTF_ALPHA_NONE);
            ret = false;
            wxLogWarning(_("Texture '%s', file '%s': No alpha channel in file. Alpha deactivated."), parent->Name.c_str(), texture().GetFullPath().fn_str());
        }
    }

    // Make sure recolorable flag matches
    recolorable(parent->Recolorable);
    // Init for next step
    used = false;

    return ret;
}

bool cFlexiTextureFrame::FromNode(cXmlNode& node, const wxString& path, unsigned long version) {
    bool ret = true;
    std::string temp;
    unsigned long t;

    if (!node)
        return false;
    if (!node(RCT3XML_CFLEXITEXTUREFRAME))
        return false;

    texture(wxString::FromUTF8(node.getPropVal("texture").c_str()));
    if (!texture().GetFullPath().IsSameAs(wxT(""))) {
        if (!texture().IsAbsolute()) {
            texture_nc().MakeAbsolute(path);
        }
    }
    alpha(wxString::FromUTF8(node.getPropVal("alpha").c_str()));
    if (!alpha().GetFullPath().IsSameAs(wxT(""))) {
        if (!alpha().IsAbsolute()) {
            alpha_nc().MakeAbsolute(path);
        }
    }

    if (node.getPropVal("recolorable", &temp)) {
        if (!parseULong(temp, t)) {
            recolorable(0);
            wxLogError(_("FlexiTextureFrame, recolorable failed parsing."));
            ret = false;
        } else {
            recolorable(t);
        }
    } else {
        recolorable(0);
    }
    if (node.getPropVal("alphacutoff", &temp)) {
        if (!parseULong(temp, t)) {
            alphacutoff(0);
            wxLogError(_("FlexiTextureFrame, alphacutoff failed parsing."));
            ret = false;
        } else {
            alphacutoff(t);
        }
    } else {
        alphacutoff(0);
    }
    if (node.getPropVal("alphasource", &temp)) {
        if (!parseULong(temp, t)) {
            alphasource(CFTF_ALPHA_NONE);
            wxLogError(_("FlexiTextureFrame, alphasource failed parsing."));
            ret = false;
        } else {
            alphasource(t);
        }
    } else {
        alphasource(CFTF_ALPHA_NONE);
    }

    if (alphasource() != CFTF_ALPHA_EXTERNAL)
        alpha(wxFileName());

    return ret;
}

cXmlNode cFlexiTextureFrame::GetNode(const wxString& path) {
    cXmlNode node(RCT3XML_CFLEXITEXTUREFRAME);
    wxFileName temp = texture();
    temp.MakeRelativeTo(path);
    node.prop("texture", temp.GetFullPath());
    if (alphasource() == CFTF_ALPHA_EXTERNAL) {
        temp = alpha();
        temp.MakeRelativeTo(path);
        node.prop("alpha", temp.GetFullPath());
    }
    if (recolorable())
        node.prop("recolorable", boost::str(boost::format("%lu") % recolorable()).c_str());
    if (alphacutoff())
        node.prop("alphacutoff", boost::str(boost::format("%hhu") % alphacutoff()).c_str());
    if (alphasource())
        node.prop("alphasource", boost::str(boost::format("%lu") % alphasource()).c_str());
    return node;
}


///////////////////////////////////////////////////////////////
//
// cFlexiTexture
//
///////////////////////////////////////////////////////////////

bool cFlexiTexture::Check() {
    bool ret = true;

    if (!Frames.size()) {
        // No texture frames added.
        throw RCT3Exception(wxString::Format(_("Texture '%s': No texture frames defined."), Name.c_str()));
    }

    if ((FPS == 0) && ((Frames.size()>1) || (Animation.size()>1))) {
        // Non-animated Animation
        if (READ_RCT3_EXPERTMODE()) {
            wxLogWarning(_("Texture '%s': Animation defined but FPS is 0."), Name.c_str());
        } else {
            throw RCT3Exception(wxString::Format(_("Texture '%s': Animation defined but FPS is 0."), Name.c_str()));
        }
    }

    // The validators should make these checks unnecessary, but better safe than sorry
    for (cFlexiTextureFrame::iterator i_ftxfr = Frames.begin(); i_ftxfr != Frames.end(); ++i_ftxfr) {
        if (!i_ftxfr->Check(this))
            ret = false;
/*
        try {
            checkRCT3Texture(i_ftxfr->texture().GetFullPath());
        } catch (RCT3TextureException& e) {
            throw RCT3Exception(wxString::Format(_("Texture '%s', file '%s': %s"), i_ftx->Name.c_str(), i_ftxfr->texture().GetFullPath().fn_str(), e.what()));
        }

        if (i_ftxfr->alphasource() == CFTF_ALPHA_EXTERNAL) {
            try {
                checkRCT3Texture(i_ftxfr->alpha().GetFullPath());
            } catch (RCT3TextureException& e) {
                i_ftxfr->alphasource(CFTF_ALPHA_NONE);
                warning = true;
                wxLogWarning(_("Texture '%s', file '%s': Problem with alpha channel file. Alpha deactivated. (Problem was %s)"), i_ftx->Name.c_str(), i_ftxfr->alpha().GetFullPath().fn_str(), e.what());
            }
        } else if (i_ftxfr->alphasource() == CFTF_ALPHA_INTERNAL) {
            wxGXImage img(i_ftxfr->texture().GetFullPath());
            if (!img.HasAlpha()) {
                i_ftxfr->alphasource(CFTF_ALPHA_NONE);
                warning = true;
                wxLogWarning(_("Texture '%s', file '%s': No alpha channel in file. Alpha deactivated."), i_ftx->Name.c_str(), i_ftxfr->texture().GetFullPath().fn_str());
            }
        }

        // Make sure recolorable flag matches
        i_ftxfr->recolorable(i_ftx->Recolorable);
        // Init for next step
        i_ftxfr->used = false;
*/
    }

    // Check the animation
    if (Animation.size()) {
        for (unsigned long i = 0; i < Animation.size(); i++) {
            if (Animation[i].frame() >= Frames.size()) {
                // Illegal reference
                throw RCT3Exception(wxString::Format(_("Texture '%s': Animation step %d references non-existing frame."), Name.c_str(), i+1));
            }
            Frames[Animation[i].frame()].used = true;
        }
    } else {
        // We need to add one refering to the first frame
        cFlexiTextureAnim an(0);
        Animation.push_back(an);
        Frames[0].used = true;
    }


    // Now we go through the frames again and fix unreferenced ones
    // Doing this inverse to make removing easier
    for (long i = Frames.size() - 1; i >=0 ; i--) {
        if (!Frames[i].used) {
            if (READ_RCT3_EXPERTMODE()) {
                ret = false;
                wxLogWarning(_("Texture '%s', frame %d: Unused."), Name.c_str(), i);
            } else {
                // Great -.-
                Frames.erase(Frames.begin() + i);
                // Fix animation
                for (cFlexiTextureAnim::iterator i_anim = Animation.begin(); i_anim != Animation.end(); ++i_anim) {
                    if (i_anim->frame() > i)
                        i_anim->frame(i_anim->frame() - 1);
                }
                ret = false;
                wxLogWarning(_("Texture '%s', frame %d: Unused. It will not be written to the ovl file."), Name.c_str(), i);
            }
        }
    }

    // Zero FPS if there is only one animation step
    if (Animation.size()<=1) {
        if (READ_RCT3_EXPERTMODE()) {
            if (FPS) {
                ret = false;
                wxLogWarning(_("Texture '%s': FPS > 0, but no animation defined."), Name.c_str());
            }
        } else {
            FPS = 0;
        }
    }

    return true;
}

bool cFlexiTexture::FromCompilerXml(cXmlNode& node, const wxString& path) {
    if (!node)
        return false;

    Name = wxString::FromUTF8(node.getPropVal("name").c_str());
    if (Name.IsEmpty())
        throw RCT3Exception(_("Texture lacks name attribute"));

    wxString tex = wxString::FromUTF8(node.getPropVal("image").c_str());
    if (tex.IsEmpty())
        throw RCT3Exception(_("Texture lacks image attribute"));
    wxFileName texfn = tex;
    if (!texfn.IsAbsolute())
        texfn.MakeAbsolute(path);

    cFlexiTextureFrame frame;
    frame.texture(texfn);

    wxString alph = wxString::FromUTF8(node.getPropVal("alpha").c_str());
    if (!alph.IsEmpty()) {
        wxFileName alphfn = alph;
        if (!alphfn.IsAbsolute())
            alphfn.MakeAbsolute(path);
        frame.alpha(alphfn);
        frame.alphasource(CFTF_ALPHA_EXTERNAL);
    }
    Frames.push_back(frame);

    return true;
}

bool cFlexiTexture::FromNode(cXmlNode& node, const wxString& path, unsigned long version) {
    bool ret = true;
    std::string temp;
    if (!node)
        return false;
    if (!node(RCT3XML_CFLEXITEXTURE))
        return false;

    Name = wxString::FromUTF8(node.getPropVal("name").c_str());
    if (node.getPropVal("recolorable", &temp)) {
        if (!parseULong(temp, Recolorable)) {
            Recolorable = 0;
            wxLogError(_("FlexiTexture, recolorable failed parsing."));
            ret = false;
        }
    } else {
        Recolorable = 0;
    }
    if (node.getPropVal("fps", &temp)) {
        if (!parseULong(temp, FPS)) {
            FPS = 0;
            wxLogError(_("FlexiTexture, fps failed parsing."));
            ret = false;
        }
    } else {
        FPS = 0;
    }

    cXmlNode child(node.children());
    while(child) {
        if (child(RCT3XML_CFLEXITEXTUREANIM)) {
            cFlexiTextureAnim anim;
            if (!anim.FromNode(child, path, version))
                ret = false;
            Animation.push_back(anim);
        } else if (child(RCT3XML_CFLEXITEXTUREFRAME)) {
            cFlexiTextureFrame frame;
            if (!frame.FromNode(child, path, version))
                ret = false;
            Frames.push_back(frame);
        }
        ++child;
    }
    return ret;
}

cXmlNode cFlexiTexture::GetNode(const wxString& path) {
    cXmlNode node(RCT3XML_CFLEXITEXTURE);

    node.prop("name", Name);
    if (FPS)
        node.prop("fps", boost::str(boost::format("%lu") % FPS));
    if (Recolorable)
        node.prop("recolorable", boost::str(boost::format("%lu") % Recolorable));
    for (cFlexiTextureAnim::iterator it = Animation.begin(); it != Animation.end(); it++) {
        cXmlNode newnode = it->GetNode(path);
        node.appendChildren(newnode);
    }
    for (cFlexiTextureFrame::iterator it = Frames.begin(); it != Frames.end(); it++) {
        cXmlNode newnode = it->GetNode(path);
        node.appendChildren(newnode);
    }
    return node;
}

COLOURQUAD cFlexiTexture::g_rgbPalette[256];
bool cFlexiTexture::g_rgbPaletteCreated = false;
COLOURQUAD cFlexiTexture::g_bmyPalette[256];
bool cFlexiTexture::g_bmyPaletteCreated = false;

COLOURQUAD* cFlexiTexture::GetRGBPalette() {
    if (!g_rgbPaletteCreated) {
        memset(&g_rgbPalette, 0, sizeof(g_rgbPalette));
        for (int i = 1; i <= 85; i++) {
            g_rgbPalette[i].red = ((86 - i) * 255) / 85;
            g_rgbPalette[i + 85].green = ((86 - i) * 255) / 85;
            g_rgbPalette[i + 170].blue = ((86 - i) * 255) / 85;
        }
        g_rgbPaletteCreated = true;
    }
    return reinterpret_cast<COLOURQUAD*>(&g_rgbPalette);
}

COLOURQUAD* cFlexiTexture::GetBMYPalette() {
    if (!g_bmyPaletteCreated) {
        memset(&g_bmyPalette, 0, sizeof(g_rgbPalette));
        for (int i = 0; i < 256; i++) {
            g_bmyPalette[i].red = bmy_pal[(i * 4)+2];
            g_bmyPalette[i].green = bmy_pal[(i * 4)+1];
            g_bmyPalette[i].blue = bmy_pal[(i * 4)];
        }
        g_bmyPaletteCreated = true;
    }
    return reinterpret_cast<COLOURQUAD*>(&g_bmyPalette);
}

///////////////////////////////////////////////////////////////
//
// cEffectPoint
//
///////////////////////////////////////////////////////////////

cEffectPoint::cEffectPoint(const cModelBone& bone) {
    name = bone.name;
    transforms = bone.positions1;
    transformnames = bone.position1names;
};

bool cEffectPoint::FromNode(cXmlNode& node, const wxString& path, unsigned long version) {
    bool ret = true;
    std::string temp;
    if (!node)
        return false;
    if (!node(RCT3XML_CEFFECTPOINT))
        return false;

    name = wxString::FromUTF8(node.getPropVal("name").c_str());

    cXmlNode child(node.children());
    while(child) {
        if (child(RCT3XML_MATRIX)) {
            wxString n = _("Error");
            MATRIX m;
            if (!XmlParseMatrixNode(child, &m, &n, version ))
                ret = false;
            transforms.push_back(m);
            transformnames.Add(n);
        }
        ++child;
    }
    return ret;
}

cXmlNode cEffectPoint::GetNode(const wxString& path) {
    cXmlNode node(RCT3XML_CEFFECTPOINT);
    node.prop("name", name);
    for (unsigned int i = 0; i < transforms.size(); i++) {
        cXmlNode newchild = XmlMakeMatrixNode(transforms[i], transformnames[i]);
        node.appendChildren(newchild);
    }
    return node;
}


///////////////////////////////////////////////////////////////
//
// cModel
//
///////////////////////////////////////////////////////////////

cModel::cModel(const cAnimatedModel& model) {
    name = model.name;
    file = model.file;
    transforms = model.transforms;
    transformnames = model.transformnames;
    meshstructs = model.meshstructs;
    usedorientation = model.usedorientation;
    fileorientation = model.fileorientation;
    error = model.error;
    fatal_error = model.fatal_error;
    for (vector<cModelBone>::const_iterator it = model.modelbones.begin(); it != model.modelbones.end(); it++) {
        cEffectPoint p(*it);
        effectpoints.push_back(p);
    }
}

cModel::cModel(MATRIX def, c3DLoaderOrientation ori): name(wxT("")), file(wxT("")), usedorientation(ori), fileorientation(ORIENTATION_UNKNOWN), fatal_error(false) {
    transforms.push_back(def);
    transformnames.push_back(_("Default Matrix"));
}

void cModel::SetupFileProperties(cMeshStruct* ms, const counted_ptr<c3DLoader>& obj, unsigned int n) {
    if ((obj->GetObjectName(n).CmpNoCase(wxT("root")) == 0)
            || (obj->GetObjectVertexCount(n) < 3)
            || (!obj->IsObjectValid(n))) {
        ms->valid = false;
        if (!ms->disabled)
            error.push_back(wxString::Format(_("Mesh '%s' has been automatically disabled as it is invalid (broken or missing uv-mapping)."), obj->GetObjectName(n).c_str()));
        ms->disabled = true;
    } else {
        ms->valid = true;
    }
    if (obj->GetObjectVertexCount(n) == 1) {
        ms->effectpoint = true;
        ms->effectpoint_vert = obj->GetObjectVertex(n, 0).position;
    } else {
        ms->effectpoint = false;
    }
    ms->faces = obj->GetObjectIndexCount(n) / 3;
}

cMeshStruct cModel::MakeMesh(const counted_ptr<c3DLoader>& obj, unsigned int n) {
    cMeshStruct ms;
    if ((obj->GetObjectName(n).CmpNoCase(wxT("root")) == 0)
            || (obj->GetObjectVertexCount(n) < 3)
            || (!obj->IsObjectValid(n))) {
        ms.disabled = true;
    } else {
        ms.disabled = false;
    }
    SetupFileProperties(&ms, obj, n);
    ms.Name = obj->GetObjectName(n);
/*
    ms.flags = 0;
    ms.place = 0;
    ms.unknown = 3;
    ms.FTX = "";
    ms.TXS = "";
*/
    return ms;
}

bool cModel::Load() {
    counted_ptr<c3DLoader> obj = c3DLoader::LoadFile(file.GetFullPath().c_str());

    if (!obj.get()) {
        fatal_error = true;
        error.Add(wxString::Format(_("Couldn't load file '%s'. Wrong format or file not found."), file.GetFullPath().c_str()));
        return false;
    }

    if (name == wxT(""))
        name = obj->GetName();

    if (name == wxT("")) {
        wxFileName t = file;
        name = t.GetName();
    }
    fatal_error = false;
    error.clear();

    fileorientation = obj->GetOrientation();

    meshstructs.clear();
    // Load the object into mesh structs
    for (long j = 0; j < obj->GetObjectCount(); j++) {
        meshstructs.push_back(MakeMesh(obj, j));
    }

    if (obj->GetWarnings().size() > 0) {
        error.Add(_("The 3D model loader reported the following problems:"));
        error.insert(error.end(), obj->GetWarnings().begin(), obj->GetWarnings().end());
    }

    return true;
}

bool cModel::Sync() {
    fatal_error = false;
    error.clear();

    // No model file, no need to sync
    if (file == wxT(""))
        return true;

    // No mesh structs, reinit from the file
    if (meshstructs.size() == 0)
        return Load();

    // Load & check the object file
    counted_ptr<c3DLoader> obj = c3DLoader::LoadFile(file.GetFullPath().c_str());
    if (!obj.get()) {
        fatal_error = true;
        error.push_back(wxString::Format(_("Model file '%s' not found or of an unknown format."), file.GetFullPath().c_str()));
        return false;
    }

    fileorientation = obj->GetOrientation();

    // Predeclare to make goto work
    wxString notify;
    std::vector<cMeshStruct> fixup;
    std::vector<unsigned int> unfound;
    int msinfile = 0;
    int nrfound = 0;

    // See if it's an old scn file where the mesh names were not stored
    bool oldfile = (meshstructs[0].Name == wxT(""));

    if (oldfile) {
        if (obj->GetObjectCount() == meshstructs.size()) {
            // Number of meshes unchanged. We simply assume the order hasn't changed
            for(unsigned int i = 0; i < meshstructs.size(); i++)
                meshstructs[i].Name = obj->GetObjectName(i);
        } else if (obj->GetObjectCount() > meshstructs.size()) {
            // New meshes in the model file
            // Warn about it
            error.push_back(wxString::Format(_("%d new mesh(es) in the model file.\nAs the scn file is too old to map the meshes, the ones known were assigned in order and the new ones appended.\nYou should probably check if everything is allright."),
                        obj->GetObjectCount()-meshstructs.size()));
            // Assign the ones present
            for(unsigned int i = 0; i < meshstructs.size(); i++)
                meshstructs[i].Name = obj->GetObjectName(i);
            // Initialize the new ones
            for(unsigned int i = meshstructs.size(); i < obj->GetObjectCount(); i++)
                meshstructs.push_back(MakeMesh(obj, i));
        } else if (obj->GetObjectCount() < meshstructs.size()) {
            // Meshes were removed from the model file
            // Warn about it
            error.push_back(wxString::Format(_("%d mesh(es) were removed from the model file.\nAs the scn file is too old to map the meshes, the ones known were assigned in order and the rest was deleted.\nYou should probably check if everything is allright."),
                        meshstructs.size()-obj->GetObjectCount()));
            // Assign the ones present
            for(unsigned int i = 0; i < obj->GetObjectCount(); i++)
                meshstructs[i].Name = obj->GetObjectName(i);
            // Delete the unused entries
            meshstructs.erase(meshstructs.begin() + obj->GetObjectCount(), meshstructs.end());
        }
        goto fixupsinglevertexmeshes;
    }

    // We have a newer scn file with stored mesh names
    // before getting serious we do a simple check to see if everything is allright
    if (obj->GetObjectCount() == meshstructs.size()) {
        bool allright = true;
        for(unsigned int i = 0; i < meshstructs.size(); i++) {
            if (meshstructs[i].Name.CmpNoCase(obj->GetObjectName(i)) == 0) {
                // It matches, we update the name in case the case changed
                meshstructs[i].Name = obj->GetObjectName(i);
            } else {
                // It does not match
                allright = false;
                break;
            }
        }
        if (allright)
            goto fixupsinglevertexmeshes;
    }

    // We need to fix things
    msinfile = meshstructs.size();

    // First round of fixups
    nrfound = 0;
    for (unsigned int i = 0; i < obj->GetObjectCount(); i++) {
        bool found = false;
        for (cMeshStruct::iterator ms = meshstructs.begin(); ms != meshstructs.end(); ms++) {
            if (ms->Name.CmpNoCase(obj->GetObjectName(i)) == 0) {
                // Found!
                // We update the name in case the case changed
                ms->Name = obj->GetObjectName(i);
                fixup.push_back(*ms);
                found = true;
                nrfound++;
                meshstructs.erase(ms);
                break;
            }
        }
        if (!found) {
            unfound.push_back(i);
            fixup.push_back(MakeMesh(obj, i));
        }
    }

    // If the fixups were successful, meshstructs is empty
    if (meshstructs.size() == 0) {
        notify = _("The model file contents changed, everything could be matched up.");
        if (msinfile != obj->GetObjectCount()) {
            // More meshes in the model file
            notify += wxString::Format(_("\nThere were %d new meshes in the model file."), obj->GetObjectCount() - nrfound);
        }
        error.push_back(notify);
        meshstructs = fixup;
        goto fixupsinglevertexmeshes;
    }

    // Didn't work. Yay! -.-
    // Special case: Nothing worked, but we have the same number of meshes/objects
    if ((obj->GetObjectCount() == meshstructs.size()) && (obj->GetObjectCount() == msinfile)) {
        error.push_back(_("The model file contents changed completely.\nAs the number of meshes stayed constant, the settings were transferred in order.\nYou should prabably check if everything is allright."));
        for (unsigned int i = 0; i < obj->GetObjectCount(); i++) {
            meshstructs[i].Name = obj->GetObjectName(i);
        }
        goto fixupsinglevertexmeshes;
    }

    // Special case: Meshes/Objects disappeared
    if (nrfound == obj->GetObjectCount()) {
        error.push_back(_("The model file contents changed, it seems like you deleted at least one mesh.\nAll meshes in the object file could be matched, so probably everything is ok.\nYou should still check if everything is allright."));
        meshstructs.clear();
        meshstructs = fixup;
        goto fixupsinglevertexmeshes;
    }

    // Now we blindly apply unassigned mesh info
    notify = _("The model file contents changed, an attempt was made to match everything up. Log:");
    for (;;) {
        if ((!unfound.size()) || (!meshstructs.size())) {
            // No more model file or scn file meshes to assign, bail out
            break;
        }
        unsigned int a = unfound[0];
        unfound.erase(unfound.begin());
        notify += wxString::Format(_("\n- The settings of mesh '%s' were applied to mesh '%s' from the model file."), meshstructs[0].Name.c_str(), fixup[a].Name.c_str());
        fixup[a].CopySettingsFrom(meshstructs[0]);
        meshstructs.erase(meshstructs.begin());
    }
    // Whatever could be done has been done. warn about the rest
    if (unfound.size()) {
        // Additional meshes in the model file.
        notify += _("\nThere were additinal meshes in the model file, they were added to the list:");
        for (std::vector<unsigned int>::iterator it = unfound.begin(); it != unfound.end(); it++) {
            notify += wxString::Format(wxT("\n- %s"), fixup[*it].Name.c_str());
        }
    } else if (meshstructs.size()) {
        // Additional meshes in the model file.
        notify += _("\nThere were not enough meshes in the model file, surplus one were deleted:");
        for (cMeshStruct::iterator ms = meshstructs.begin(); ms != meshstructs.end(); ms++) {
            notify += wxString::Format(wxT("\n- %s"), ms->Name.c_str());
        }
    } else {
        notify += _("\nThe number of meshes in the model file matched.\n(Note: This is the result you probably get if you renamed a mesh in the model file.)");
    }
    error.push_back(notify + _("\nYou should check if everything is allright."));
    meshstructs.clear();
    meshstructs = fixup;

fixupsinglevertexmeshes:
    for (unsigned int i = 0; i < obj->GetObjectCount(); i++)
        SetupFileProperties(&meshstructs[i], obj, i);

    if (obj->GetWarnings().size() > 0) {
        error.Add(_("The 3D model loader reported the following problems:"));
        error.insert(error.end(), obj->GetWarnings().begin(), obj->GetWarnings().end());
    }

    return true;
}

bool cModel::GetTransformationMatrices(MATRIX& transform, MATRIX& undodamage) const {
    transform = matrixMultiply(matrixMultiply(transforms), matrixGetFixOrientation(usedorientation));

    int m = 0;
    for(m = transformnames.size()-1; m>=0; m--) {
        if (transformnames[m] == wxT("-"))
            break;
    }
    if (m>0) {
        // Note: if m=0, the first is the separator, so this can fall through to no separator
        std::vector<MATRIX> undostack;
        for (int n = m; n < transforms.size(); n++)
            undostack.push_back(transforms[n]);
        undostack.push_back(matrixGetFixOrientation(usedorientation));
        undodamage = matrixInverse(matrixMultiply(undostack));
    } else {
        undodamage = matrixInverse(transform);
    }
    return (!matrixIsEqual(transform, matrixGetUnity())) ||
           (!matrixIsEqual(undodamage, matrixGetUnity()));
}


bool cModel::CheckMeshes(bool animated) {
    wxLogDebug(wxT("Trace, cModel::CheckMeshes '%s', %d"), name.c_str(), animated);
    bool warning = false;

    // Fix up orientaton
    if (usedorientation == ORIENTATION_UNKNOWN)
        usedorientation = fileorientation;
    if (usedorientation == ORIENTATION_UNKNOWN)
        usedorientation = ORIENTATION_LEFT_YUP;

    if (!(file.IsOk() && file.FileExists())) {
        fatal_error = true;
        wxLogWarning(_("Model '%s': Model file not found."), name.c_str());
        return false;
    }
    counted_ptr<c3DLoader> object = c3DLoader::LoadFile(file.GetFullPath().c_str());
    if (!object.get()) {
        fatal_error = true;
        wxLogWarning(_("Model '%s': Error loading model file."), name.c_str());
        return false;
    }

    int valid_meshes = 0;
    int c_mesh = 0;
    for (cMeshStruct::iterator i_mesh = meshstructs.begin(); i_mesh != meshstructs.end(); ++i_mesh) {
        if (i_mesh->disabled)
            continue;

        if ((!i_mesh->disabled) && (!i_mesh->valid)) {
            i_mesh->disabled = true;
            warning = true;
            wxLogWarning(_("Model '%s': Group '%s' deactivated because it's invalid."), name.c_str(), i_mesh->Name.c_str());
            continue;
        }
        if ((!i_mesh->disabled) && ((i_mesh->FTX == wxT("")) || (i_mesh->TXS == wxT("")))) {
            i_mesh->disabled = true;
            warning = true;
            wxLogWarning(_("Model '%s': Group '%s' deactivated due to missing data."), name.c_str(), i_mesh->Name.c_str());
            continue;
        }

        i_mesh->Check(name);

        if (!i_mesh->disabled)
            valid_meshes++;
        // Initialize
        i_mesh->bone = 0;

        if (!i_mesh->algo_x.IsEmpty()) {
            cTriangleSortAlgorithm::Algorithm sortalgo = cTriangleSortAlgorithm::GetAlgo(i_mesh->algo_x.mb_str(wxConvLocal));
            if (sortalgo == cTriangleSortAlgorithm::EnumSize) {
                throw RCT3Exception(wxString::Format(_("Model '%s': Group '%s' has unknown triangle sort algorithm: %s"), name.c_str(), i_mesh->Name.c_str(), i_mesh->algo_x.c_str()));
            }
        }
        if (!i_mesh->algo_y.IsEmpty()) {
            cTriangleSortAlgorithm::Algorithm sortalgo = cTriangleSortAlgorithm::GetAlgo(i_mesh->algo_y.mb_str(wxConvLocal));
            if (sortalgo == cTriangleSortAlgorithm::EnumSize) {
                throw RCT3Exception(wxString::Format(_("Model '%s': Group '%s' has unknown triangle sort algorithm: %s"), name.c_str(), i_mesh->Name.c_str(), i_mesh->algo_y.c_str()));
            }
        }
        if (!i_mesh->algo_z.IsEmpty()) {
            cTriangleSortAlgorithm::Algorithm sortalgo = cTriangleSortAlgorithm::GetAlgo(i_mesh->algo_z.mb_str(wxConvLocal));
            if (sortalgo == cTriangleSortAlgorithm::EnumSize) {
                throw RCT3Exception(wxString::Format(_("Model '%s': Group '%s' has unknown triangle sort algorithm: %s"), name.c_str(), i_mesh->Name.c_str(), i_mesh->algo_z.c_str()));
            }
        }

        if (animated) {
            if (object->GetObjectVertexCount(c_mesh)>USHRT_MAX){
                throw RCT3Exception(wxString::Format(_("Animated model '%s': Group '%s' has too many vertices for an animated mesh. This is an internal limitation, but as the limit is at %d, your object has too many faces anyways."), name.c_str(), i_mesh->Name.c_str(), USHRT_MAX));
            }
            c_mesh++;
        }
    }
    if (!valid_meshes) {
        fatal_error = true;
        wxLogWarning(_("Model '%s': No usable groups left. Model invalid."), name.c_str());
        return false;
    }
    return true;
}

bool cModel::Check(cModelMap& modnames) {
    wxLogDebug(wxT("Trace, cModel::Check '%s'"), name.c_str());
    if (fatal_error)
        return false;

    bool warning = !CheckMeshes();

    // Initialize for the next step
    modnames[name] = this;
    if (READ_RCT3_EXPERTMODE()) {
        used = true;
    } else {
        used = false;
    }

    return !warning;
}

bool cModel::FromNode(cXmlNode& node, const wxString& path, unsigned long version) {
    bool ret = true;
    std::string temp;
    if (!node)
        return false;
    if (!node(RCT3XML_CMODEL))
        return false;

    name = wxString::FromUTF8(node.getPropVal("name").c_str());
    file = wxString::FromUTF8(node.getPropVal("file").c_str());
    if (!file.GetFullPath().IsSameAs(wxT(""))) {
        if (!file.IsAbsolute()) {
            file.MakeAbsolute(path);
        }
    }
    if (node.getPropVal("orientation", &temp)) {
        unsigned long l = 0;
        if (!parseULong(temp, l)) {
            usedorientation = ORIENTATION_UNKNOWN;
            wxLogError(_("Model, usedorientation failed parsing."));
            ret = false;
        } else {
            usedorientation = static_cast<c3DLoaderOrientation>(l);
        }
    } else {
        usedorientation = ORIENTATION_UNKNOWN;
    }

    cXmlNode child(node.children());
    while(child) {
        if (child(RCT3XML_MATRIX)) {
            wxString n = _("Error");
            MATRIX m;
            if (!XmlParseMatrixNode(child, &m, &n, version))
                ret = false;
            transforms.push_back(m);
            transformnames.Add(n);
        } else if (child(RCT3XML_CMESHSTRUCT)) {
            cMeshStruct m;
            if (!m.FromNode(child, path, version))
                ret = false;
            meshstructs.push_back(m);
        } else if (child(RCT3XML_CEFFECTPOINT)) {
            cEffectPoint e;
            if (!e.FromNode(child, path, version))
                ret = false;
            effectpoints.push_back(e);
        }

        ++child;
    }
    Sync();
    return ret;
}

void cModel::AddNodeContent(cXmlNode& node, const wxString& path, bool do_local) {
    node.prop("name", name);
    wxFileName temp = file;
    temp.MakeRelativeTo(path);
    node.prop("file", temp.GetFullPath());
    unsigned long l = usedorientation;
    node.prop("orientation", boost::str(boost::format("%lu") % l).c_str());

    for (unsigned int i = 0; i < transforms.size(); i++) {
        cXmlNode newchild = XmlMakeMatrixNode(transforms[i], transformnames[i]);
        node.appendChildren(newchild);
    }
    for (cMeshStruct::iterator it = meshstructs.begin(); it != meshstructs.end(); it++) {
        cXmlNode newchild = it->GetNode(path);
        node.appendChildren(newchild);
    }
    if (do_local) {
        for (cEffectPoint::iterator it = effectpoints.begin(); it != effectpoints.end(); it++) {
            cXmlNode newchild = it->GetNode(path);
            node.appendChildren(newchild);
        }
    }
}


///////////////////////////////////////////////////////////////
//
// cModelBone
//
///////////////////////////////////////////////////////////////

#define COMPILER_POS1 "pos1"
#define COMPILER_POS2 "pos2"

bool cModelBone::FromCompilerXml(cXmlNode& node, const wxString& path) {
    if (!node)
        return false;

    name = wxString::FromUTF8(node.getPropVal("name").c_str());
    if (name.IsEmpty())
        throw RCT3Exception(_("BONE tag lacks name attribute"));
    parent = wxString::FromUTF8(node.getPropVal("parent").c_str());

    cXmlNode child(node.children());
    while(child) {
        if (child(COMPILER_POS1)) {
            if (positions1.size())
                throw RCT3Exception(wxString::Format(_("Duplicate pos1 tag of bone '%s'."), name.c_str()));

            MATRIX t;
            std::string positions = child();
            if (!parseMatrix(positions, t)) {
                throw RCT3Exception(wxString::Format(_("POS1 tag of bone '%s' must contain 16 decimal values."), name.c_str()));
            }
            positions1.push_back(t);
            position1names.push_back(_("Imported pos1 matrix"));
        } else if (child(COMPILER_POS2)) {
            if (positions2.size())
                throw RCT3Exception(wxString::Format(_("Duplicate pos2 tag of bone '%s'."), name.c_str()));

            MATRIX t;
            std::string positions = child();
            if (!parseMatrix(positions, t)) {
                throw RCT3Exception(wxString::Format(_("POS2 tag of bone '%s' must contain 16 decimal values."), name.c_str()));
            }
            positions2.push_back(t);
            position2names.push_back(_("Imported pos2 matrix"));
        } else if (child.element()) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in bone tag."), STRING_FOR_FORMAT(child.name())));
        }
        ++child;
    }

    usepos2 = positions2.size();

    return true;
}

bool cModelBone::FromNode(cXmlNode& node, const wxString& path, unsigned long version) {
    bool ret = true;
    std::string temp;
    if (!node)
        return false;
    if (!node(RCT3XML_CMODELBONE))
        return false;

    name = wxString::FromUTF8(node.getPropVal("name").c_str());
    parent = wxString::FromUTF8(node.getPropVal("parent").c_str());
    usepos2 = node.getPropVal("usepos2", "0") == "1";

    cXmlNode child(node.children());
    while(child) {
        if (child(RCT3XML_CMODELBONE_P1)) {
            cXmlNode schild(child.children());
            while(schild) {
                if (schild(RCT3XML_MATRIX)) {
                    wxString n = _("Error");
                    MATRIX m;
                    if (!XmlParseMatrixNode(schild, &m, &n, version))
                        ret = false;
                    positions1.push_back(m);
                    position1names.Add(n);
                }
                ++schild;
            }
        } else if (child(RCT3XML_CMODELBONE_P2)) {
            cXmlNode schild(child.children());
            while(schild) {
                if (schild(RCT3XML_MATRIX)) {
                    wxString n = _("Error");
                    MATRIX m;
                    if (!XmlParseMatrixNode(schild, &m, &n, version))
                        ret = false;
                    positions2.push_back(m);
                    position2names.Add(n);
                }
                ++schild;
            }
        } else if (child(RCT3XML_CMODELBONE_MESH)) {
            wxString mesh = wxString::FromUTF8(child.getPropVal("name", "ERROR").c_str());
            meshes.Add(mesh);
        }
        ++child;
    }
    return ret;
}

cXmlNode cModelBone::GetNode(const wxString& path) {
    cXmlNode node(RCT3XML_CMODELBONE);
    node.prop("name", name);
    if (!parent.IsEmpty())
        node.prop("parent", parent);
    if (usepos2)
        node.prop("usepos2", "1");

    cXmlNode pos1(RCT3XML_CMODELBONE_P1);
    for (unsigned int i = 0; i < positions1.size(); i++) {
        cXmlNode newchild = XmlMakeMatrixNode(positions1[i], position1names[i]);
        pos1.appendChildren(newchild);
    }
    node.appendChildren(pos1);

    cXmlNode pos2(RCT3XML_CMODELBONE_P2);
    for (unsigned int i = 0; i < positions2.size(); i++) {
        cXmlNode newchild = XmlMakeMatrixNode(positions2[i], position2names[i]);
        pos2.appendChildren(newchild);
    }
    node.appendChildren(pos2);

    for (cStringIterator it = meshes.begin(); it != meshes.end(); it++) {
        //wxXmlNode* newchild = new wxXmlNode(node, wxXML_ELEMENT_NODE, RCT3XML_CMODELBONE_MESH);
        cXmlNode newchild(RCT3XML_CMODELBONE_MESH);
        newchild.prop("name", *it);
        node.appendChildren(newchild);
    }

    return node;
}

///////////////////////////////////////////////////////////////
//
// cAnimatedModel
//
///////////////////////////////////////////////////////////////

cAnimatedModel::cAnimatedModel(const cModel& model) {
    name = model.name;
    file = model.file;
    transforms = model.transforms;
    transformnames = model.transformnames;
    meshstructs = model.meshstructs;
    usedorientation = model.usedorientation;
    fileorientation = model.fileorientation;
    error = model.error;
    fatal_error = model.fatal_error;
    for (vector<cEffectPoint>::const_iterator it = model.effectpoints.begin(); it != model.effectpoints.end(); ++it) {
        cModelBone b(*it);
        modelbones.push_back(b);
    }
}

bool cAnimatedModel::Check(cAnimatedModelMap& amodnames) {
    wxLogDebug(wxT("Trace, cAnimatedModel::Check '%s'"), name.c_str());
    if (fatal_error)
        return false;

    bool warning = !CheckMeshes(true);

    if (fatal_error)
        return false;

    cMeshStructMap meshmap;
    for (cMeshStruct::iterator i_ms = meshstructs.begin(); i_ms != meshstructs.end(); ++i_ms) {
        if (!i_ms->disabled)
            meshmap[i_ms->Name] = &(*i_ms);
    }

    if (modelbones.size() == 0) {
        warning = true;
        wxLogWarning(_("Animated Model '%s' has no bones. Making it static would be better."), name.c_str());
    } else {
        // check for duplicates and assign parents
        for (unsigned int i = 0; i < modelbones.size(); ++i) {
            wxLogDebug(wxT("Trace, cAnimatedModel::Check bone %s, %d"), modelbones[i].name.c_str(), modelbones[i].meshes.size());
            if (modelbones[i].name == modelbones[i].parent) {
                throw RCT3Exception(wxString::Format(_("Animated Model '%s': Bone '%s' is it's own parent."), name.c_str(), modelbones[i].name.c_str()));
            }
            modelbones[i].nparent = -1;
            for (unsigned int j = 0; j < modelbones.size(); ++j) {
                if (j == i)
                    continue;
                if (modelbones[i].name == modelbones[j].name) {
                    throw RCT3Exception(wxString::Format(_("Animated Model '%s': Duplicate bone name '%s'."), name.c_str(), modelbones[i].name.c_str()));
                }
                if (modelbones[i].parent == modelbones[j].name) {
                    modelbones[i].nparent = j+1;
                }
            }
            if ((modelbones[i].nparent == -1) && (modelbones[i].parent != wxT(""))) {
                throw RCT3Exception(wxString::Format(_("Animated Model '%s': Bone '%s' misses it's parent '%s'."), name.c_str(), modelbones[i].name.c_str(), modelbones[i].parent.c_str()));
            }

            // Assign meshes
            for (cStringIterator i_st = modelbones[i].meshes.begin(); i_st != modelbones[i].meshes.end(); ++i_st) {
                cMeshStructMap::iterator a_ms = meshmap.find(*i_st);
                if (a_ms == meshmap.end()) {
                    throw RCT3Exception(wxString::Format(_("Animated Model '%s': Mesh '%s' was assigned to bone '%s' but is disabled, invalid or not there."), name.c_str(), i_st->c_str(), modelbones[i].name.c_str()));
                }
                if (a_ms->second->bone != 0) {
                    warning = true;
                    wxLogWarning(_("Animated Model '%s': Group '%s' was assigned twice. It will stay assigned to bone '%s'."), name.c_str(), i_st->c_str(), modelbones[a_ms->second->bone-1].name.c_str());
                    continue;
                } else {
                    a_ms->second->bone = i+1;
                    wxLogDebug(_("Animated Model '%s': Group '%s' was assigned to bone '%s'."), name.c_str(), i_st->c_str(), modelbones[i].name.c_str());
                }
            }

            // Set pos2
            if (!modelbones[i].usepos2)
                modelbones[i].positions2 = modelbones[i].positions1;
        }
    }

    // Initialize for the next step
    amodnames[name] = this;
    used = false;

    return !warning;
}

#define COMPILER_GEOMOBJ "geomobj"
#define COMPILER_BONE "bone"

bool cAnimatedModel::FromCompilerXml(cXmlNode& node, const wxString& path) {
    bool ret = true;
    if (!node)
        return false;

    name = wxString::FromUTF8(node.getPropVal("name").c_str());
    if (name.IsEmpty())
        throw RCT3Exception(_("BSH tag lacks name attribute"));

    wxString f = wxString::FromUTF8(node.getPropVal("model").c_str());
    if (f.IsEmpty())
        f = wxString::FromUTF8(node.getPropVal("ase").c_str());
    if (f.IsEmpty())
        throw RCT3Exception(_("BSH tag lacks model attribute"));
    file = f;
    if (!file.IsAbsolute())
        file.MakeAbsolute(path);

    cMeshStruct::vec filestructs;
    cXmlNode child(node.children());
    while(child) {
        if (child(COMPILER_GEOMOBJ)) {
            cMeshStruct mesh;
            if (!mesh.FromCompilerXml(child, path))
                ret = false;
            filestructs.push_back(mesh);
        } else if (child(COMPILER_BONE)) {
            cModelBone bone;
            if (!bone.FromCompilerXml(child, path))
                ret = false;
            modelbones.push_back(bone);
        } else if (child.element()) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in bsh tag."), STRING_FOR_FORMAT(child.name())));
        }
        ++child;
    }

    fatal_error = false;
    Load();
    if (fatal_error)
        throw RCT3Exception(error[error.size()-1]);

    for (cMeshStruct::iterator it = meshstructs.begin(); it != meshstructs.end(); ++it) {
        it->disabled = true;
        for (cMeshStruct::iterator sr = filestructs.begin(); sr != filestructs.end(); ++sr) {
            if (sr->Name == it->Name) {
                it->CopySettingsFrom(*sr);
                it->disabled = false;
                filestructs.erase(sr);
                break;
            }
        }
    }

    if (filestructs.size())
        throw RCT3Exception(wxString::Format(_("GEOMOBJ '%s' references unknown group in model file."), filestructs[0].Name.c_str()));

    for (cModelBone::iterator it = modelbones.begin(); it != modelbones.end(); ++it) {
        if (it->parent.IsEmpty())
            continue;

        unsigned long t;
        wxString temp;
        if (!it->parent.ToULong(&t)) {
            // Assume that parent is given as string. Discrepancies will be found in Check
            //throw RCT3Exception(wxString::Format(_("Bone '%s': Bad parent value '%s'."), it->name.c_str(), it->parent.c_str()));
        } else {
            if (t > modelbones.size())
                throw RCT3Exception(wxString::Format(_("Bone '%s': Bad parent value '%s'."), it->name.c_str(), it->parent.c_str()));
            it->parent = modelbones[t - 1].name;
        }
    }
    for (cMeshStruct::iterator it = meshstructs.begin(); it != meshstructs.end(); ++it) {
        if (it->bone > 0) {
            if (it->bone == 0xFFFFFFFF) {
                bool found = false;
                for (cModelBone::iterator itb = modelbones.begin(); itb != modelbones.end(); ++itb) {
                    if (itb->name == it->bonename) {
                        itb->meshes.Add(it->Name);
                        found = true;
                        break;
                    }
                }
                if (!found)
                    throw RCT3Exception(wxString::Format(_("GEOMOBJ '%s' assigned to unknown bone '%s'."), it->Name.c_str(), it->bonename.c_str()));
            } else {
                if (it->bone > modelbones.size())
                    throw RCT3Exception(wxString::Format(_("GEOMOBJ '%s' assigned to unknown bone."), it->Name.c_str()));
                modelbones[it->bone - 1].meshes.Add(it->Name);
            }
            it->bone = 0;
        }
    }

    return ret && (!error.size());
}

bool cAnimatedModel::FromNode(cXmlNode& node, const wxString& path, unsigned long version) {
    bool ret = true;
    std::string temp;
    if (!node)
        return false;
    if (!node(RCT3XML_CANIMATEDMODEL))
        return false;

    name = wxString::FromUTF8(node.getPropVal("name").c_str());
    file = wxString::FromUTF8(node.getPropVal("file").c_str());
    if (!file.GetFullPath().IsSameAs(wxT(""))) {
        if (!file.IsAbsolute()) {
            file.MakeAbsolute(path);
        }
    }
    if (node.getPropVal("orientation", &temp)) {
        unsigned long l = 0;
        if (!parseULong(temp, l)) {
            usedorientation = ORIENTATION_UNKNOWN;
            wxLogError(_("AnimatedModel, usedorientation failed parsing."));
            ret = false;
        } else {
            usedorientation = static_cast<c3DLoaderOrientation>(l);
        }
    } else {
        usedorientation = ORIENTATION_UNKNOWN;
    }

    cXmlNode child = node.children();
    while(child) {
        if (child(RCT3XML_MATRIX)) {
            wxString n = _("Error");
            MATRIX m;
            if (!XmlParseMatrixNode(child, &m, &n, version))
                ret = false;
            transforms.push_back(m);
            transformnames.Add(n);
        } else if (child(RCT3XML_CMESHSTRUCT)) {
            cMeshStruct m;
            if (!m.FromNode(child, path, version))
                ret = false;
            meshstructs.push_back(m);
        } else if (child(RCT3XML_CMODELBONE)) {
            cModelBone b;
            if (!b.FromNode(child, path, version))
                ret = false;
            modelbones.push_back(b);
        }

        ++child;
    }
    Sync();
    return ret;
}

void cAnimatedModel::AddNodeContent(cXmlNode& node, const wxString& path, bool do_local) {
    cModel::AddNodeContent(node, path, false);
    if (do_local) {
        for (cModelBone::iterator it = modelbones.begin(); it != modelbones.end(); it++) {
            cXmlNode newchild = it->GetNode(path);
            node.appendChildren(newchild);
        }
    }
}


///////////////////////////////////////////////////////////////
//
// cBoneAnimation
//
///////////////////////////////////////////////////////////////

#define COMPILER_TRANSLATE "translate"
#define COMPILER_ROTATE "rotate"

bool cBoneAnimation::FromCompilerXml(cXmlNode& node, const wxString& path) {

    bool ret = true;
    std::string temp;
    if (!node)
        return false;

    name = wxString::FromUTF8(node.getPropVal("name").c_str());
    if (name.IsEmpty())
        throw RCT3Exception(_("BONE tag lacks name attribute"));

    cXmlNode child = node.children();
    while(child) {
        if (child(COMPILER_TRANSLATE)) {
            temp = child.getPropVal("time");
            if (temp == "")
                throw RCT3Exception(_("TRANSLATE tag lacks time attribute"));
            txyz v;
            if (!parseFloat(temp, v.Time))
                throw RCT3Exception(_("TRANSLATE tag has malformed time attribute"));

            temp = child();
            if (!parseCompilerVector(temp, v))
                throw RCT3Exception(_("TRANSLATE tag has malformed content"));

            translations.push_back(v);
        } else if (child(COMPILER_ROTATE)) {
            temp = child.getPropVal("time");
            if (temp == "")
                throw RCT3Exception(_("ROTATE tag lacks time attribute"));
            txyz v;
            if (!parseFloat(temp, v.Time))
                throw RCT3Exception(_("ROTATE tag has malformed time attribute"));

            temp = child();
            if (!parseCompilerVector(temp, v))
                throw RCT3Exception(_("ROTATE tag has malformed content"));

            rotations.push_back(v);
        } else if (child.element()) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in bone tag."), STRING_FOR_FORMAT(child.name())));
        }
        ++child;
    }
    return ret;
}

bool cBoneAnimation::FromNode(cXmlNode& node, const wxString& path, unsigned long version) {
    bool ret = true;
    std::string temp;
    if (!node)
        return false;
    if (!node(RCT3XML_CBONEANIMATION))
        return false;

    name = wxString::FromUTF8(node.getPropVal("name").c_str());

    cXmlNode child = node.children();
    while(child) {
        if (child(RCT3XML_CBONEANIMATION_TRANSLATIONS)) {
            cXmlNode schild = child.children();
            while(schild) {
                if (schild(RCT3XML_CTXYZ)) {
                    cTXYZ v;
                    if (!v.FromNode(schild, path, version))
                        ret = false;
                    translations.push_back(v);
                }
                ++schild;
            }
        } else if (child(RCT3XML_CBONEANIMATION_ROTATIONS)) {
            cXmlNode schild = child.children();
            while(schild) {
                if (schild(RCT3XML_CTXYZ)) {
                    cTXYZ v;
                    if (!v.FromNode(schild, path, version))
                        ret = false;
                    rotations.push_back(v);
                }
                ++schild;
            }
        }
        ++child;
    }
    return ret;
}

cXmlNode cBoneAnimation::GetNode(const wxString& path) {
    cXmlNode node(RCT3XML_CBONEANIMATION);
    node.prop("name", name);

    cXmlNode trans(RCT3XML_CBONEANIMATION_TRANSLATIONS);
    for (cTXYZ::iterator it = translations.begin(); it != translations.end(); it++) {
        cXmlNode newchild = it->GetNode(path);
        trans.appendChildren(newchild);
    }
    node.appendChildren(trans);

    cXmlNode rot(RCT3XML_CBONEANIMATION_ROTATIONS);
    for (cTXYZ::iterator it = rotations.begin(); it != rotations.end(); it++) {
        cXmlNode newchild = it->GetNode(path);
        rot.appendChildren(newchild);
    }
    node.appendChildren(rot);

    return node;
}


///////////////////////////////////////////////////////////////
//
// cAnimation
//
///////////////////////////////////////////////////////////////

bool cAnimation::Check(const wxSortedArrayString& presentbones) {
    bool ret = true;

    // Remove bad bones
    for (int i = boneanimations.size() - 1; i >= 0; --i) {
        if (presentbones.Index(boneanimations[i].name) == wxNOT_FOUND) {
            ret = false;
            if (READ_RCT3_EXPERTMODE()) {
                wxLogWarning(_("Animation '%s': Bone '%s' is not present in any animated model."), name.c_str(), boneanimations[i].name.c_str());
            } else {
                wxLogWarning(_("Animation '%s': Bone '%s' is not present in any animated model and was removed."), name.c_str(), boneanimations[i].name.c_str());
                boneanimations.erase(boneanimations.begin() + i);
            }
        }
    }

    // Find total time and check order
    totaltime = 0.0;
    float lasttime;
    for (cBoneAnimation::iterator i_anim = boneanimations.begin(); i_anim != boneanimations.end(); ++i_anim) {
        lasttime = -1.0;
        for (cTXYZ::iterator i_txyz = i_anim->translations.begin(); i_txyz != i_anim->translations.end(); ++i_txyz) {
            if (i_txyz->v.Time < 0.0)
                throw RCT3Exception(wxString::Format(_("Animation '%s': Bone '%s' has transformation with time below 0."), name.c_str(), i_anim->name.c_str()));
            if (i_txyz->v.Time <= lasttime)
                throw RCT3Exception(wxString::Format(_("Animation '%s': Bone '%s' has transformation with duplicate or out-of-order time."), name.c_str(), i_anim->name.c_str()));
            if (i_txyz->v.Time > totaltime)
                totaltime = i_txyz->v.Time;
            lasttime = i_txyz->v.Time;
        }
        lasttime = -1.0;
        for (cTXYZ::iterator i_txyz = i_anim->rotations.begin(); i_txyz != i_anim->rotations.end(); ++i_txyz) {
            if (i_txyz->v.Time < 0.0)
                throw RCT3Exception(wxString::Format(_("Animation '%s': Bone '%s' has rotation with time below 0."), name.c_str(), i_anim->name.c_str()));
            if (i_txyz->v.Time <= lasttime)
                throw RCT3Exception(wxString::Format(_("Animation '%s': Bone '%s' has rotation with duplicate or out-of-order time."), name.c_str(), i_anim->name.c_str()));
            if (i_txyz->v.Time > totaltime)
                totaltime = i_txyz->v.Time;
            lasttime = i_txyz->v.Time;
        }
    }
    return ret;
}

bool cAnimation::FromCompilerXml(cXmlNode& node, const wxString& path) {
    bool ret = true;
    if (!node)
        return false;

    name = wxString::FromUTF8(node.getPropVal("name").c_str());
    if (name.IsEmpty())
        throw RCT3Exception(_("BAN tag lacks name attribute"));

    cXmlNode child = node.children();
    while(child) {
        if (child(COMPILER_BONE)) {
            cBoneAnimation bone;
            if (!bone.FromCompilerXml(child, path))
                ret = false;
            boneanimations.push_back(bone);
        } else if (child.element()) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ban tag."), STRING_FOR_FORMAT(child.name())));
        }
        ++child;
    }
    return ret;
}

bool cAnimation::FromNode(cXmlNode& node, const wxString& path, unsigned long version) {
    bool ret = true;
    std::string temp;
    if (!node)
        return false;
    if (!node(RCT3XML_CANIMATION))
        return false;

    name = wxString::FromUTF8(node.getPropVal("name").c_str());

    if (node.getPropVal("orientation", &temp)) {
        unsigned long l = 0;
        if (!parseULong(temp, l)) {
            usedorientation = ORIENTATION_UNKNOWN;
            wxLogError(_("Animation, usedorientation failed parsing."));
            ret = false;
        } else {
            usedorientation = static_cast<c3DLoaderOrientation>(l);
        }
    } else {
        usedorientation = ORIENTATION_UNKNOWN;
    }

    cXmlNode child = node.children();
    while(child) {
        if (child(RCT3XML_CBONEANIMATION)) {
            cBoneAnimation ban;
            ban.FromNode(child, path, version);
            boneanimations.push_back(ban);
        }
        ++child;
    }
    return ret;
}

cXmlNode cAnimation::GetNode(const wxString& path) {
    cXmlNode node(RCT3XML_CANIMATION);
    node.prop("name", name);
    unsigned long l = usedorientation;
    node.prop("orientation", boost::str(boost::format("%lu") % l).c_str());

    for (cBoneAnimation::iterator it = boneanimations.begin(); it != boneanimations.end(); it++) {
        cXmlNode newchild = it->GetNode(path);
        node.appendChildren(newchild);
    }

    return node;
}


///////////////////////////////////////////////////////////////
//
// cLOD
//
///////////////////////////////////////////////////////////////

bool cLOD::FromNode(cXmlNode& node, const wxString& path, unsigned long version) {
    bool ret = true;
    std::string temp;
    if (!node)
        return false;
    if (!node(RCT3XML_CLOD))
        return false;

    modelname = wxString::FromUTF8(node.getPropVal("modelname").c_str());
    animated = node.getPropVal("animated", "0") == "1";
    temp = node.getPropVal("distance", "0.0");
    if (!parseFloat(temp, distance)) {
        wxLogError(_("LOD, distance failed parsing."));
        ret = false;
    }
    if (node.getPropVal("unk2", &temp)) {
        if (!parseULong(temp, unk2)) {
            unk2 = 0;
            wxLogError(_("LOD, unk2 failed parsing."));
            ret = false;
        }
    } else {
        unk2 = 0;
    }
    if (node.getPropVal("unk4", &temp)) {
        if (!parseULong(temp, unk4)) {
            unk4 = 0;
            wxLogError(_("LOD, unk4 failed parsing."));
            ret = false;
        }
    } else {
        unk4 = 0;
    }
    if (node.getPropVal("unk14", &temp)) {
        if (!parseULong(temp, unk14)) {
            unk14 = 0;
            wxLogError(_("LOD, unk14 failed parsing."));
            ret = false;
        }
    } else {
        unk14 = 0;
    }

    cXmlNode child = node.children();
    while(child) {
        if (child(RCT3XML_CLOD_ANIMATION)) {
            wxString anim = child.wxgetPropVal("name", "ERROR");
            animations.Add(anim);
        }
        ++child;
    }
    return ret;
}

cXmlNode cLOD::GetNode(const wxString& path) {
    cXmlNode node(RCT3XML_CLOD);
    node.prop("modelname", modelname);
    if (animated)
        node.prop("animated", "1");
    node.prop("distance", boost::str(boost::format("%8f") % distance).c_str());
    node.prop("unk2", boost::str(boost::format("%lu") % unk2).c_str());
    node.prop("unk4", boost::str(boost::format("%lu") % unk4).c_str());
    node.prop("unk14", boost::str(boost::format("%lu") % unk14).c_str());

    for (cStringIterator it = animations.begin(); it != animations.end(); it++) {
        cXmlNode newchild(RCT3XML_CLOD_ANIMATION);
        newchild.prop("name", *it);
        node.appendChildren(newchild);
    }

    return node;
}


///////////////////////////////////////////////////////////////
//
// cSIVSettings
//
///////////////////////////////////////////////////////////////

bool cSIVSettings::FromNode(cXmlNode& node, const wxString& path, unsigned long version) {
    bool ret = true;
    std::string temp;
    if (!node)
        return false;
    if (!node(RCT3XML_CSIVSETTINGS))
        return false;

    if (node.getPropVal("sivflags", &temp)) {
        unsigned long l;
        if (!parseULong(temp, l)) {
            sivflags = 0;
            wxLogError(_("SIV, sivflags failed parsing."));
            ret = false;
        } else {
            sivflags = l;
        }
    } else {
        sivflags = 0;
    }
    temp = node.getPropVal("sway", "0.0");
    if (!parseFloat(temp, sway)) {
        wxLogError(_("SIV, sway failed parsing."));
        ret = false;
    }
    temp = node.getPropVal("brightness", "0.0");
    if (!parseFloat(temp, brightness)) {
        wxLogError(_("SIV, brightness failed parsing."));
        ret = false;
    }
    temp = node.getPropVal("unknown", "0.0");
    if (!parseFloat(temp, unknown)) {
        wxLogError(_("SIV, unknown failed parsing."));
        ret = false;
    }
    temp = node.getPropVal("scale", "0.0");
    if (!parseFloat(temp, scale)) {
        wxLogError(_("SIV, scale failed parsing."));
        ret = false;
    }
    if (node.getPropVal("unk6", &temp)) {
        if (!parseULong(temp, unk6)) {
            unk6 = 0;
            wxLogError(_("SIV, unk6 failed parsing."));
            ret = false;
        }
    } else {
        unk6 = 0;
    }
    if (node.getPropVal("unk7", &temp)) {
        if (!parseULong(temp, unk7)) {
            unk7 = 0;
            wxLogError(_("SIV, unk7 failed parsing."));
            ret = false;
        }
    } else {
        unk7 = 0;
    }
    if (node.getPropVal("unk8", &temp)) {
        if (!parseULong(temp, unk8)) {
            unk8 = 0;
            wxLogError(_("SIV, unk8 failed parsing."));
            ret = false;
        }
    } else {
        unk8 = 0;
    }
    if (node.getPropVal("unk9", &temp)) {
        if (!parseULong(temp, unk9)) {
            unk9 = 0;
            wxLogError(_("SIV, unk9 failed parsing."));
            ret = false;
        }
    } else {
        unk9 = 0;
    }
    if (node.getPropVal("unk10", &temp)) {
        if (!parseULong(temp, unk10)) {
            unk10 = 0;
            wxLogError(_("SIV, unk10 failed parsing."));
            ret = false;
        }
    } else {
        unk10 = 0;
    }
    if (node.getPropVal("unk11", &temp)) {
        if (!parseULong(temp, unk11)) {
            unk11 = 0;
            wxLogError(_("SIV, unk11 failed parsing."));
            ret = false;
        }
    } else {
        unk11 = 0;
    }

    return ret;
}

cXmlNode cSIVSettings::GetNode(const wxString& path) {
    cXmlNode node(RCT3XML_CSIVSETTINGS);
    node.prop("sivflags", boost::str(boost::format("%u") % sivflags));
    node.prop("sway", boost::str(boost::format("%8f") % sway));
    node.prop("brightness", boost::str(boost::format("%8f") % brightness));
    node.prop("unknown", boost::str(boost::format("%8f") % unknown));
    node.prop("scale", boost::str(boost::format("%8f") % scale));
    node.prop("unk6", boost::str(boost::format("%lu") % unk6));
    node.prop("unk7", boost::str(boost::format("%lu") % unk7));
    node.prop("unk8", boost::str(boost::format("%lu") % unk8));
    node.prop("unk9", boost::str(boost::format("%lu") % unk9));
    node.prop("unk10", boost::str(boost::format("%lu") % unk10));
    node.prop("unk11", boost::str(boost::format("%lu") % unk11));

    return node;
}


