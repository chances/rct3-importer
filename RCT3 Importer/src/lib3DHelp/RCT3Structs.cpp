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

#include "confhelp.h"
#include "lib3Dconfig.h"
#include "matrix.h"
#include "RCT3Exception.h"
#include "texcheck.h"
#include "gximage.h"

///////////////////////////////////////////////////////////////
//
// Matrix Xml Helpers
//
///////////////////////////////////////////////////////////////

bool XmlParseMatrixNode(wxXmlNode* node, D3DMATRIX* matrix, wxString* name, unsigned long version) {
    bool ret = true;
    if (!node)
        return false;
    if (!node->GetName().IsSameAs(RCT3XML_MATRIX))
        return false;

    ret = node->GetPropVal(wxT("name"), name);
    *matrix = matrixGetUnity();

    int rows = 0;
    wxString temp;
    wxXmlNode* child = node->GetChildren();
    while(child) {
        if (child->GetName() == wxT("row1")) {
            temp = child->GetNodeContent();
            if (sscanf(temp.mb_str(wxConvLocal), "%f %f %f %f", &matrix->_11, &matrix->_12, &matrix->_13, &matrix->_14) != 4)
                ret = false;
            rows++;
        } else if (child->GetName() == wxT("row2")) {
            temp = child->GetNodeContent();
            if (sscanf(temp.mb_str(wxConvLocal), "%f %f %f %f", &matrix->_21, &matrix->_22, &matrix->_23, &matrix->_24) != 4)
                ret = false;
            rows++;
        } else if (child->GetName() == wxT("row3")) {
            temp = child->GetNodeContent();
            if (sscanf(temp.mb_str(wxConvLocal), "%f %f %f %f", &matrix->_31, &matrix->_32, &matrix->_33, &matrix->_34) != 4)
                ret = false;
            rows++;
        } else if (child->GetName() == wxT("row4")) {
            temp = child->GetNodeContent();
            if (sscanf(temp.mb_str(wxConvLocal), "%f %f %f %f", &matrix->_41, &matrix->_42, &matrix->_43, &matrix->_44) != 4)
                ret = false;
            rows++;
        }
        child = child->GetNext();
    }
    if (rows != 4)
        return false;
    return ret;
}

wxXmlNode* XmlMakeMatrixNode(const D3DMATRIX& matrix, const wxString& name) {
    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_MATRIX);
    node->AddProperty(wxT("name"), name);
    wxXmlNode* lastnode;
    //wxXmlNode* childnode = new wxXmlNode(node, wxXML_ELEMENT_NODE, wxT("row1"));
    //wxXmlNode* textnode = new wxXmlNode(childnode, wxXML_TEXT_NODE, wxT(""));
    wxXmlNode* childnode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("row1"));
    wxXmlNode* textnode = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""));
    textnode->SetContent(wxString::Format(wxT("%8f %8f %8f %8f"), matrix._11, matrix._12, matrix._13, matrix._14));
    childnode->SetChildren(textnode);
    node->SetChildren(childnode);
    lastnode = childnode;

    //childnode = new wxXmlNode(node, wxXML_ELEMENT_NODE, wxT("row2"));
    //textnode = new wxXmlNode(childnode, wxXML_TEXT_NODE, wxT(""));
    childnode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("row2"));
    textnode = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""));
    textnode->SetContent(wxString::Format(wxT("%8f %8f %8f %8f"), matrix._21, matrix._22, matrix._23, matrix._24));
    childnode->SetChildren(textnode);
    lastnode->SetNext(childnode);
    lastnode = childnode;

    //childnode = new wxXmlNode(node, wxXML_ELEMENT_NODE, wxT("row3"));
    //textnode = new wxXmlNode(childnode, wxXML_TEXT_NODE, wxT(""));
    childnode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("row3"));
    textnode = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""));
    textnode->SetContent(wxString::Format(wxT("%8f %8f %8f %8f"), matrix._31, matrix._32, matrix._33, matrix._34));
    childnode->SetChildren(textnode);
    lastnode->SetNext(childnode);
    lastnode = childnode;

    //childnode = new wxXmlNode(node, wxXML_ELEMENT_NODE, wxT("row4"));
    //textnode = new wxXmlNode(childnode, wxXML_TEXT_NODE, wxT(""));
    childnode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("row4"));
    textnode = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""));
    textnode->SetContent(wxString::Format(wxT("%8f %8f %8f %8f"), matrix._41, matrix._42, matrix._43, matrix._44));
    childnode->SetChildren(textnode);
    lastnode->SetNext(childnode);
    lastnode = childnode;

    return node;
}


///////////////////////////////////////////////////////////////
//
// TXYZ Xml Helpers
//
///////////////////////////////////////////////////////////////

#define TXYZ_READPROP(pr, s) \
    temp = node->GetPropVal(wxT(pr), wxT("0.0")); \
    if (sscanf(temp.mb_str(wxConvLocal), "%f", &v.s) != 1) { \
        ret = false; \
        v.s = 0.0; \
    }

bool cTXYZ::FromNode(wxXmlNode* node, const wxString& path, unsigned long version) {
    bool ret = true;
    wxString temp;
    if (!node)
        return false;
    if (!node->GetName().IsSameAs(RCT3XML_CTXYZ))
        return false;

    TXYZ_READPROP("time", Time)
    TXYZ_READPROP("x", X)
    TXYZ_READPROP("y", Y)
    TXYZ_READPROP("z", Z)

    return ret;
}

wxXmlNode* cTXYZ::GetNode(const wxString& path) {
    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_CTXYZ);

    node->AddProperty(wxT("time"), wxString::Format(wxT("%8f"), v.Time));
    node->AddProperty(wxT("x"), wxString::Format(wxT("%8f"), v.X));
    node->AddProperty(wxT("y"), wxString::Format(wxT("%8f"), v.Y));
    node->AddProperty(wxT("z"), wxString::Format(wxT("%8f"), v.Z));

    return node;
}
/*
txyz& operator= (txyz& t, cTXYZ& v) {
    t = v.v;
    return t;
}
*/

txyz cTXYZ::GetFixed(c3DLoaderOrientation ori) {
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
    place = SS2_PLACE_TEXTURE_AND_MESH;
    flags = 0;
    unknown = SS2_FACES_SINGLE_SIDED;
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
}

bool cMeshStruct::FromCompilerXml(wxXmlNode* node, const wxString& path) {
    if (!node)
        return false;

    bone = 0;
    disabled = false;
    flags = 0;

    Name = node->GetPropVal(wxT("name"), wxT(""));
    if (Name.IsEmpty())
        throw RCT3Exception(_("GEOMOBJ tag lacks name attribute"));

    unsigned long t;
    wxString temp;
    if (node->GetPropVal(wxT("bone"), &temp)) {
        if (!temp.ToULong(&t)) {
            // Improvement
            bonename = temp;
            bone = 0xFFFFFFFF;
            //throw RCT3Exception(wxString::Format(_("GEOMOBJ '%s': Bad bone value '%s'."), Name.c_str(), temp.c_str()));
        } else {
            bone = t;
        }
    }

    unknown = node->GetPropVal(wxT("doublesided"),wxT("0")).IsSameAs(wxT("1"))?1:3;
    wxString placing = node->GetPropVal(wxT("placing"),wxT("both")).Lower();
    if (placing.IsSameAs(wxT("texture"))) {
        place = 1;
    } else if (placing.IsSameAs(wxT("glass"))) {
        place = 2;
    } else if (placing.IsSameAs(wxT("both"))) {
        place = 0;
    } else {
        throw RCT3Exception(wxString::Format(_("GEOMOBJ '%s': Unknown placing value '%s'."), Name.c_str(), placing.c_str()));
    }

    wxString sflags = node->GetPropVal(wxT("flags"),wxT("")).Lower();
    FTX = node->GetPropVal(wxT("ftx"),wxT(""));
    TXS = node->GetPropVal(wxT("txs"),wxT(""));

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

    return true;
}

bool cMeshStruct::FromNode(wxXmlNode* node, const wxString& path, unsigned long version) {
    bool ret = true;
    wxString temp;
    if (!node)
        return false;
    if (!node->GetName().IsSameAs(RCT3XML_CMESHSTRUCT))
        return false;
    disabled = node->GetPropVal(wxT("disabled"), wxT("0")).IsSameAs(wxT("1"));
    TXS = node->GetPropVal(wxT("txs"), wxT("SIOpaque"));
    FTX = node->GetPropVal(wxT("ftx"), wxT(""));
    if (node->GetPropVal(wxT("place"), &temp)) {
        if (!temp.ToULong(&place)) {
            place = SS2_PLACE_TEXTURE_AND_MESH;
            ret = false;
        } else {
            if (place > SS2_PLACE_UNKNOWN) {
                place = SS2_PLACE_TEXTURE_AND_MESH;
                ret = false;
            }
        }
    } else {
        place = SS2_PLACE_TEXTURE_AND_MESH;
    }
    if (node->GetPropVal(wxT("flags"), &temp)) {
        if (!temp.ToULong(&flags)) {
            flags = 0;
            ret = false;
        }
    } else {
        flags = 0;
    }
    if (node->GetPropVal(wxT("unknown"), &temp)) {
        if (!temp.ToULong(&unknown)) {
            unknown = SS2_FACES_SINGLE_SIDED;
            ret = false;
        } else {
            if ((unknown != SS2_FACES_SINGLE_SIDED) && (unknown != SS2_FACES_DOUBLE_SIDED)) {
                unknown = SS2_FACES_SINGLE_SIDED;
                ret = false;
            }
        }
    } else {
        unknown = SS2_FACES_SINGLE_SIDED;
    }
    if (node->GetPropVal(wxT("fudgenormals"), &temp)) {
        if (!temp.ToULong(&fudgenormals)) {
            fudgenormals = 0;
            ret = false;
        } else {
            if (fudgenormals > 3) {
                fudgenormals = 0;
                ret = false;
            }
        }
    } else {
        fudgenormals = 0;
    }
    if (node->GetPropVal(wxT("name"), &temp)) {
        Name = temp;
    } else {
        Name = _("Missing!!!");
        ret = false;
    }

    return ret;
}

wxXmlNode* cMeshStruct::GetNode(const wxString& path) {
    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_CMESHSTRUCT);
    node->AddProperty(wxT("disabled"), disabled?wxT("1"):wxT("0"));
    node->AddProperty(wxT("txs"), TXS);
    node->AddProperty(wxT("ftx"), FTX);
    node->AddProperty(wxT("place"), wxString::Format(wxT("%lu"), place));
    node->AddProperty(wxT("flags"), wxString::Format(wxT("%lu"), flags));
    node->AddProperty(wxT("unknown"), wxString::Format(wxT("%lu"), unknown));
    node->AddProperty(wxT("fudgenormals"), wxString::Format(wxT("%lu"), fudgenormals));
    node->AddProperty(wxT("name"), Name);
    return node;
}


///////////////////////////////////////////////////////////////
//
// cFlexiTextureAnim
//
///////////////////////////////////////////////////////////////

bool cFlexiTextureAnim::FromNode(wxXmlNode* node, const wxString& path, unsigned long version) {
    bool ret = true;
    wxString temp;
    unsigned long t;

    if (!node)
        return false;
    if (!node->GetName().IsSameAs(RCT3XML_CFLEXITEXTUREANIM))
        return false;

    if (node->GetPropVal(wxT("frame"), &temp)) {
        if (!temp.ToULong(&t)) {
            frame(0);
            ret = false;
        } else {
            frame(t);
        }
    } else {
        frame(0);
    }
    if (node->GetPropVal(wxT("count"), &temp)) {
        if (!temp.ToULong(&t)) {
            count(1);
            ret = false;
        } else {
            count(t);
        }
    } else {
        count(1);
    }

    return ret;
}

wxXmlNode* cFlexiTextureAnim::GetNode(const wxString& path) {
    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_CFLEXITEXTUREANIM);
    node->AddProperty(wxT("frame"), wxString::Format(wxT("%lu"), frame()));
    node->AddProperty(wxT("count"), wxString::Format(wxT("%lu"), count()));
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

bool cFlexiTextureFrame::FromNode(wxXmlNode* node, const wxString& path, unsigned long version) {
    bool ret = true;
    wxString temp;
    unsigned long t;

    if (!node)
        return false;
    if (!node->GetName().IsSameAs(RCT3XML_CFLEXITEXTUREFRAME))
        return false;

    texture(node->GetPropVal(wxT("texture"), wxT("")));
    if (!texture().GetFullPath().IsSameAs(wxT(""))) {
        if (!texture().IsAbsolute()) {
            texture_nc().MakeAbsolute(path);
        }
    }
    alpha(node->GetPropVal(wxT("alpha"), wxT("")));
    if (!alpha().GetFullPath().IsSameAs(wxT(""))) {
        if (!alpha().IsAbsolute()) {
            alpha_nc().MakeAbsolute(path);
        }
    }

    if (node->GetPropVal(wxT("recolorable"), &temp)) {
        if (!temp.ToULong(&t)) {
            recolorable(0);
            ret = false;
        } else {
            recolorable(t);
        }
    } else {
        recolorable(0);
    }
    if (node->GetPropVal(wxT("alphacutoff"), &temp)) {
        if (!temp.ToULong(&t)) {
            alphacutoff(0);
            ret = false;
        } else {
            alphacutoff(t);
        }
    } else {
        alphacutoff(0);
    }
    if (node->GetPropVal(wxT("alphasource"), &temp)) {
        if (!temp.ToULong(&t)) {
            alphasource(CFTF_ALPHA_NONE);
            ret = false;
        } else {
            alphasource(t);
        }
    } else {
        alphasource(CFTF_ALPHA_NONE);
    }

    return ret;
}

wxXmlNode* cFlexiTextureFrame::GetNode(const wxString& path) {
    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_CFLEXITEXTUREFRAME);
    wxFileName temp = texture();
    temp.MakeRelativeTo(path);
    node->AddProperty(wxT("texture"), temp.GetFullPath());
    temp = alpha();
    temp.MakeRelativeTo(path);
    node->AddProperty(wxT("alpha"), temp.GetFullPath());
    node->AddProperty(wxT("recolorable"), wxString::Format(wxT("%lu"), recolorable()));
    node->AddProperty(wxT("alphacutoff"), wxString::Format(wxT("%hhu"), alphacutoff()));
    node->AddProperty(wxT("alphasource"), wxString::Format(wxT("%lu"), alphasource()));
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

bool cFlexiTexture::FromCompilerXml(wxXmlNode* node, const wxString& path) {
    if (!node)
        return false;

    Name = node->GetPropVal(wxT("name"), wxT(""));
    if (Name.IsEmpty())
        throw RCT3Exception(_("Texture lacks name attribute"));

    wxString tex = node->GetPropVal(wxT("image"), wxT(""));
    if (tex.IsEmpty())
        throw RCT3Exception(_("Texture lacks image attribute"));
    wxFileName texfn = tex;
    if (!texfn.IsAbsolute())
        texfn.MakeAbsolute(path);

    cFlexiTextureFrame frame;
    frame.texture(texfn);

    wxString alph = node->GetPropVal(wxT("alpha"), wxT(""));
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

bool cFlexiTexture::FromNode(wxXmlNode* node, const wxString& path, unsigned long version) {
    bool ret = true;
    wxString temp;
    if (!node)
        return false;
    if (!node->GetName().IsSameAs(RCT3XML_CFLEXITEXTURE))
        return false;

    Name = node->GetPropVal(wxT("name"), wxT(""));
    if (node->GetPropVal(wxT("recolorable"), &temp)) {
        if (!temp.ToULong(&Recolorable)) {
            Recolorable = 0;
            ret = false;
        }
    } else {
        Recolorable = 0;
    }
    if (node->GetPropVal(wxT("fps"), &temp)) {
        if (!temp.ToULong(&FPS)) {
            FPS = 0;
            ret = false;
        }
    } else {
        FPS = 0;
    }

    wxXmlNode* child = node->GetChildren();
    while(child) {
        if (child->GetName() == RCT3XML_CFLEXITEXTUREANIM) {
            cFlexiTextureAnim anim;
            if (!anim.FromNode(child, path, version))
                ret = false;
            Animation.push_back(anim);
        } else if (child->GetName() == RCT3XML_CFLEXITEXTUREFRAME) {
            cFlexiTextureFrame frame;
            if (!frame.FromNode(child, path, version))
                ret = false;
            Frames.push_back(frame);
        }
        child = child->GetNext();
    }
    return ret;
}

wxXmlNode* cFlexiTexture::GetNode(const wxString& path) {
    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_CFLEXITEXTURE);
    wxXmlNode* lastchild = NULL;
    node->AddProperty(wxT("name"), Name);
    node->AddProperty(wxT("fps"), wxString::Format(wxT("%lu"), FPS));
    node->AddProperty(wxT("recolorable"), wxString::Format(wxT("%lu"), Recolorable));
    for (cFlexiTextureAnim::iterator it = Animation.begin(); it != Animation.end(); it++) {
        wxXmlNode* newchild = it->GetNode(path);
        //newchild->SetParent(node);
        if (lastchild)
            lastchild->SetNext(newchild);
        else
            node->SetChildren(newchild);
        lastchild = newchild;
    }
    for (cFlexiTextureFrame::iterator it = Frames.begin(); it != Frames.end(); it++) {
        wxXmlNode* newchild = it->GetNode(path);
        //newchild->SetParent(node);
        if (lastchild)
            lastchild->SetNext(newchild);
        else
            node->SetChildren(newchild);
        lastchild = newchild;
    }
    return node;
}

RGBQUAD cFlexiTexture::g_rgbPalette[256];
bool cFlexiTexture::g_rgbPaletteCreated = false;

RGBQUAD* cFlexiTexture::GetRGBPalette() {
    if (!g_rgbPaletteCreated) {
        ZeroMemory(&g_rgbPalette, sizeof(g_rgbPalette));
        for (int i = 1; i <= 85; i++) {
            g_rgbPalette[i].rgbRed = ((86 - i) * 255) / 85;
            g_rgbPalette[i + 85].rgbGreen = ((86 - i) * 255) / 85;
            g_rgbPalette[i + 170].rgbBlue = ((86 - i) * 255) / 85;
        }
        g_rgbPaletteCreated = true;
    }
    return reinterpret_cast<RGBQUAD*>(&g_rgbPalette);
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

bool cEffectPoint::FromNode(wxXmlNode* node, const wxString& path, unsigned long version) {
    bool ret = true;
    wxString temp;
    if (!node)
        return false;
    if (!node->GetName().IsSameAs(RCT3XML_CEFFECTPOINT))
        return false;

    name = node->GetPropVal(wxT("name"), wxT(""));

    wxXmlNode* child = node->GetChildren();
    while(child) {
        if (child->GetName() == RCT3XML_MATRIX) {
            wxString n = _("Error");
            D3DMATRIX m;
            if (!XmlParseMatrixNode(child, &m, &n, version ))
                ret = false;
            transforms.push_back(m);
            transformnames.Add(n);
        }
        child = child->GetNext();
    }
    return ret;
}

wxXmlNode* cEffectPoint::GetNode(const wxString& path) {
    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_CEFFECTPOINT);
    wxXmlNode* lastchild = NULL;
    node->AddProperty(wxT("name"), name);
    for (unsigned int i = 0; i < transforms.size(); i++) {
        wxXmlNode* newchild = XmlMakeMatrixNode(transforms[i], transformnames[i]);
        //newchild->SetParent(node);
        if (lastchild)
            lastchild->SetNext(newchild);
        else
            node->SetChildren(newchild);
        lastchild = newchild;
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

cModel::cModel(D3DMATRIX def, c3DLoaderOrientation ori): name(wxT("")), file(wxT("")), usedorientation(ori), fileorientation(ORIENTATION_UNKNOWN), fatal_error(false) {
    transforms.push_back(def);
    transformnames.push_back(_("Default Matrix"));
}

void cModel::SetupFileProperties(cMeshStruct* ms, c3DLoader* obj, unsigned int n) {
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

cMeshStruct cModel::MakeMesh(c3DLoader* obj, unsigned int n) {
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
    c3DLoader *obj = c3DLoader::LoadFile(file.GetFullPath().fn_str());

    if (!obj) {
        fatal_error = true;
        error.Add(wxString::Format(_("Couldn't load file '%s'. Wrong format or file not found."), file.GetFullPath().fn_str()));
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

    delete obj;

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
    c3DLoader *obj = c3DLoader::LoadFile(file.GetFullPath().fn_str());
    if (!obj) {
        fatal_error = true;
        error.push_back(_("Model file not found or of an unknown format."));
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

    delete obj;
    return true;
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
    c3DLoader *object = c3DLoader::LoadFile(file.GetFullPath().fn_str());
    if (!object) {
        fatal_error = true;
        wxLogWarning(_("Model '%s': Error loading model file."), name.c_str());
        delete object;
        return false;
    }

    int valid_meshes = 0;
    int c_mesh = 0;
    for (cMeshStruct::iterator i_mesh = meshstructs.begin(); i_mesh != meshstructs.end(); ++i_mesh) {
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
        if (!i_mesh->disabled)
            valid_meshes++;
        // Initialize
        i_mesh->bone = 0;

        if (animated) {
            if (object->GetObjectVertexCount(c_mesh)>USHRT_MAX){
                delete object;
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
    delete object;
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

bool cModel::FromNode(wxXmlNode* node, const wxString& path, unsigned long version) {
    bool ret = true;
    wxString temp;
    if (!node)
        return false;
    if (!node->GetName().IsSameAs(RCT3XML_CMODEL))
        return false;

    name = node->GetPropVal(wxT("name"), wxT(""));
    file = node->GetPropVal(wxT("file"), wxT(""));
    if (!file.GetFullPath().IsSameAs(wxT(""))) {
        if (!file.IsAbsolute()) {
            file.MakeAbsolute(path);
        }
    }
    if (node->GetPropVal(wxT("orientation"), &temp)) {
        unsigned long l = 0;
        if (!temp.ToULong(&l)) {
            usedorientation = ORIENTATION_UNKNOWN;
            ret = false;
        } else {
            usedorientation = static_cast<c3DLoaderOrientation>(l);
        }
    } else {
        usedorientation = ORIENTATION_UNKNOWN;
    }

    wxXmlNode* child = node->GetChildren();
    while(child) {
        if (child->GetName() == RCT3XML_MATRIX) {
            wxString n = _("Error");
            D3DMATRIX m;
            if (!XmlParseMatrixNode(child, &m, &n, version))
                ret = false;
            transforms.push_back(m);
            transformnames.Add(n);
        } else if (child->GetName() == RCT3XML_CMESHSTRUCT) {
            cMeshStruct m;
            if (!m.FromNode(child, path, version))
                ret = false;
            meshstructs.push_back(m);
        } else if (child->GetName() == RCT3XML_CEFFECTPOINT) {
            cEffectPoint e;
            if (!e.FromNode(child, path, version))
                ret = false;
            effectpoints.push_back(e);
        }

        child = child->GetNext();
    }
    Sync();
    return ret;
}

wxXmlNode* cModel::AddNodeContent(wxXmlNode* node, const wxString& path, bool do_local) {
    wxXmlNode* lastchild = NULL;
    node->AddProperty(wxT("name"), name);
    wxFileName temp = file;
    temp.MakeRelativeTo(path);
    node->AddProperty(wxT("file"), temp.GetFullPath());
    unsigned long l = usedorientation;
    node->AddProperty(wxT("orientation"), wxString::Format(wxT("%lu"), l));

    for (unsigned int i = 0; i < transforms.size(); i++) {
        wxXmlNode* newchild = XmlMakeMatrixNode(transforms[i], transformnames[i]);
        //newchild->SetParent(node);
        if (lastchild)
            lastchild->SetNext(newchild);
        else
            node->SetChildren(newchild);
        lastchild = newchild;
    }
    for (cMeshStruct::iterator it = meshstructs.begin(); it != meshstructs.end(); it++) {
        wxXmlNode* newchild = it->GetNode(path);
        //newchild->SetParent(node);
        if (lastchild)
            lastchild->SetNext(newchild);
        else
            node->SetChildren(newchild);
        lastchild = newchild;
    }
    if (do_local) {
        for (cEffectPoint::iterator it = effectpoints.begin(); it != effectpoints.end(); it++) {
            wxXmlNode* newchild = it->GetNode(path);
            //newchild->SetParent(node);
            if (lastchild)
                lastchild->SetNext(newchild);
            else
                node->SetChildren(newchild);
            lastchild = newchild;
        }
    }
    return lastchild;
}


///////////////////////////////////////////////////////////////
//
// cModelBone
//
///////////////////////////////////////////////////////////////

#define COMPILER_POS1 wxT("pos1")
#define COMPILER_POS2 wxT("pos2")

bool cModelBone::FromCompilerXml(wxXmlNode* node, const wxString& path) {
    if (!node)
        return false;

    name = node->GetPropVal(wxT("name"), wxT(""));
    if (name.IsEmpty())
        throw RCT3Exception(_("BONE tag lacks name attribute"));
    parent = node->GetPropVal(wxT("parent"), wxT(""));

    wxXmlNode* child = node->GetChildren();
    while(child) {
        if (child->GetName() == COMPILER_POS1) {
            if (positions1.size())
                throw RCT3Exception(wxString::Format(_("Duplicate pos1 tag of bone '%s'."), name.c_str()));

            D3DMATRIX t;
            if (sscanf(child->GetNodeContent().mb_str(wxConvLocal), "%f %f %f %f  %f %f %f %f  %f %f %f %f  %f %f %f %f",
                    &t._11, &t._12, &t._13, &t._14, &t._21, &t._22, &t._23, &t._24,
                    &t._31, &t._32, &t._33, &t._34, &t._41, &t._42, &t._43, &t._44) != 16) {
                throw RCT3Exception(wxString::Format(_("POS1 tag of bone '%s' must contain 16 decimal values."), name.c_str()));
            }
            positions1.push_back(t);
            position1names.push_back(_("Imported pos1 matrix"));
        } else if (child->GetName() == COMPILER_POS2) {
            if (positions2.size())
                throw RCT3Exception(wxString::Format(_("Duplicate pos2 tag of bone '%s'."), name.c_str()));

            D3DMATRIX t;
            if (sscanf(child->GetNodeContent().mb_str(wxConvLocal), "%f %f %f %f  %f %f %f %f  %f %f %f %f  %f %f %f %f",
                    &t._11, &t._12, &t._13, &t._14, &t._21, &t._22, &t._23, &t._24,
                    &t._31, &t._32, &t._33, &t._34, &t._41, &t._42, &t._43, &t._44) != 16) {
                throw RCT3Exception(wxString::Format(_("POS2 tag of bone '%s' must contain 16 decimal values."), name.c_str()));
            }
            positions2.push_back(t);
            position2names.push_back(_("Imported pos2 matrix"));
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in bone tag."), child->GetName().c_str()));
        }
        child = child->GetNext();
    }

    usepos2 = positions2.size();

    return true;
}

bool cModelBone::FromNode(wxXmlNode* node, const wxString& path, unsigned long version) {
    bool ret = true;
    wxString temp;
    if (!node)
        return false;
    if (!node->GetName().IsSameAs(RCT3XML_CMODELBONE))
        return false;

    name = node->GetPropVal(wxT("name"), wxT(""));
    parent = node->GetPropVal(wxT("parent"), wxT(""));
    usepos2 = node->GetPropVal(wxT("usepos2"), wxT("0")).IsSameAs(wxT("1"));

    wxXmlNode* child = node->GetChildren();
    while(child) {
        if (child->GetName() == RCT3XML_CMODELBONE_P1) {
            wxXmlNode* schild = child->GetChildren();
            while(schild) {
                if (schild->GetName() == RCT3XML_MATRIX) {
                    wxString n = _("Error");
                    D3DMATRIX m;
                    if (!XmlParseMatrixNode(schild, &m, &n, version))
                        ret = false;
                    positions1.push_back(m);
                    position1names.Add(n);
                }
                schild = schild->GetNext();
            }
        } else if (child->GetName() == RCT3XML_CMODELBONE_P2) {
            wxXmlNode* schild = child->GetChildren();
            while(schild) {
                if (schild->GetName() == RCT3XML_MATRIX) {
                    wxString n = _("Error");
                    D3DMATRIX m;
                    if (!XmlParseMatrixNode(schild, &m, &n, version))
                        ret = false;
                    positions2.push_back(m);
                    position2names.Add(n);
                }
                schild = schild->GetNext();
            }
        } else if (child->GetName() == RCT3XML_CMODELBONE_MESH) {
            wxString mesh = child->GetPropVal(wxT("name"), wxT("ERROR"));
            meshes.Add(mesh);
        }
        child = child->GetNext();
    }
    return ret;
}

wxXmlNode* cModelBone::GetNode(const wxString& path) {
    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_CMODELBONE);
    wxXmlNode* lastchild = NULL;
    node->AddProperty(wxT("name"), name);
    if (!parent.IsEmpty())
        node->AddProperty(wxT("parent"), parent);
    if (usepos2)
        node->AddProperty(wxT("usepos2"), wxT("1"));

    //wxXmlNode* pos1 = new wxXmlNode(node, wxXML_ELEMENT_NODE, RCT3XML_CMODELBONE_P1);
    wxXmlNode* pos1 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_CMODELBONE_P1);
    for (unsigned int i = 0; i < positions1.size(); i++) {
        wxXmlNode* newchild = XmlMakeMatrixNode(positions1[i], position1names[i]);
        //newchild->SetParent(pos1);
        if (lastchild)
            lastchild->SetNext(newchild);
        else
            pos1->SetChildren(newchild);
        lastchild = newchild;
    }
    node->SetChildren(pos1);

    //wxXmlNode* pos2 = new wxXmlNode(node, wxXML_ELEMENT_NODE, RCT3XML_CMODELBONE_P2);
    wxXmlNode* pos2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_CMODELBONE_P2);
    lastchild = NULL;
    for (unsigned int i = 0; i < positions2.size(); i++) {
        wxXmlNode* newchild = XmlMakeMatrixNode(positions2[i], position2names[i]);
        //newchild->SetParent(pos2);
        if (lastchild)
            lastchild->SetNext(newchild);
        else
            pos2->SetChildren(newchild);
        lastchild = newchild;
    }
    pos1->SetNext(pos2);
    lastchild = pos2;

    for (cStringIterator it = meshes.begin(); it != meshes.end(); it++) {
        //wxXmlNode* newchild = new wxXmlNode(node, wxXML_ELEMENT_NODE, RCT3XML_CMODELBONE_MESH);
        wxXmlNode* newchild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_CMODELBONE_MESH);
        newchild->AddProperty(wxT("name"), *it);
        if (lastchild)
            lastchild->SetNext(newchild);
        else
            node->SetChildren(newchild);
        lastchild = newchild;
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

#define COMPILER_GEOMOBJ wxT("geomobj")
#define COMPILER_BONE wxT("bone")

bool cAnimatedModel::FromCompilerXml(wxXmlNode* node, const wxString& path) {
    bool ret = true;
    if (!node)
        return false;

    name = node->GetPropVal(wxT("name"), wxT(""));
    if (name.IsEmpty())
        throw RCT3Exception(_("BSH tag lacks name attribute"));

    wxString f = node->GetPropVal(wxT("model"), wxT(""));
    if (f.IsEmpty())
        f = node->GetPropVal(wxT("ase"), wxT(""));
    if (f.IsEmpty())
        throw RCT3Exception(_("BSH tag lacks model attribute"));
    file = f;
    if (!file.IsAbsolute())
        file.MakeAbsolute(path);

    cMeshStruct::vec filestructs;
    wxXmlNode* child = node->GetChildren();
    while(child) {
        if (child->GetName() == COMPILER_GEOMOBJ) {
            cMeshStruct mesh;
            if (!mesh.FromCompilerXml(child, path))
                ret = false;
            filestructs.push_back(mesh);
        } else if (child->GetName() == COMPILER_BONE) {
            cModelBone bone;
            if (!bone.FromCompilerXml(child, path))
                ret = false;
            modelbones.push_back(bone);
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in bsh tag."), child->GetName().c_str()));
        }
        child = child->GetNext();
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

bool cAnimatedModel::FromNode(wxXmlNode* node, const wxString& path, unsigned long version) {
    bool ret = true;
    wxString temp;
    if (!node)
        return false;
    if (!node->GetName().IsSameAs(RCT3XML_CANIMATEDMODEL))
        return false;

    name = node->GetPropVal(wxT("name"), wxT(""));
    file = node->GetPropVal(wxT("file"), wxT(""));
    if (!file.GetFullPath().IsSameAs(wxT(""))) {
        if (!file.IsAbsolute()) {
            file.MakeAbsolute(path);
        }
    }
    if (node->GetPropVal(wxT("orientation"), &temp)) {
        unsigned long l = 0;
        if (!temp.ToULong(&l)) {
            usedorientation = ORIENTATION_UNKNOWN;
            ret = false;
        } else {
            usedorientation = static_cast<c3DLoaderOrientation>(l);
        }
    } else {
        usedorientation = ORIENTATION_UNKNOWN;
    }

    wxXmlNode* child = node->GetChildren();
    while(child) {
        if (child->GetName() == RCT3XML_MATRIX) {
            wxString n = _("Error");
            D3DMATRIX m;
            if (!XmlParseMatrixNode(child, &m, &n, version))
                ret = false;
            transforms.push_back(m);
            transformnames.Add(n);
        } else if (child->GetName() == RCT3XML_CMESHSTRUCT) {
            cMeshStruct m;
            if (!m.FromNode(child, path, version))
                ret = false;
            meshstructs.push_back(m);
        } else if (child->GetName() == RCT3XML_CMODELBONE) {
            cModelBone b;
            if (!b.FromNode(child, path, version))
                ret = false;
            modelbones.push_back(b);
        }

        child = child->GetNext();
    }
    Sync();
    return ret;
}

wxXmlNode* cAnimatedModel::AddNodeContent(wxXmlNode* node, const wxString& path, bool do_local) {
    wxXmlNode* lastchild = cModel::AddNodeContent(node, path, false);
    if (do_local) {
        for (cModelBone::iterator it = modelbones.begin(); it != modelbones.end(); it++) {
            wxXmlNode* newchild = it->GetNode(path);
            //newchild->SetParent(node);
            if (lastchild)
                lastchild->SetNext(newchild);
            else
                node->SetChildren(newchild);
            lastchild = newchild;
        }
    }
    return lastchild;
}


///////////////////////////////////////////////////////////////
//
// cBoneAnimation
//
///////////////////////////////////////////////////////////////

#define COMPILER_TRANSLATE wxT("translate")
#define COMPILER_ROTATE wxT("rotate")

bool cBoneAnimation::FromCompilerXml(wxXmlNode* node, const wxString& path) {
    bool ret = true;
    wxString temp;
    if (!node)
        return false;

    name = node->GetPropVal(wxT("name"), wxT(""));
    if (name.IsEmpty())
        throw RCT3Exception(_("BONE tag lacks name attribute"));

    wxXmlNode* child = node->GetChildren();
    while(child) {
        if (child->GetName() == COMPILER_TRANSLATE) {
            temp = child->GetPropVal(wxT("time"), wxT(""));
            if (temp.IsEmpty())
                throw RCT3Exception(_("TRANSLATE tag lacks time attribute"));
            txyz v;
            if (sscanf(temp.mb_str(wxConvLocal), "%f", &v.Time) != 1)
                throw RCT3Exception(_("TRANSLATE tag has malformed time attribute"));

            temp = child->GetNodeContent();
            if (sscanf(temp.mb_str(wxConvLocal), "%f %f %f", &v.X, &v.Y, &v.Z) != 3)
                throw RCT3Exception(_("TRANSLATE tag has malformed content"));

            translations.push_back(v);
        } else if (child->GetName() == COMPILER_ROTATE) {
            temp = child->GetPropVal(wxT("time"), wxT(""));
            if (temp.IsEmpty())
                throw RCT3Exception(_("ROTATE tag lacks time attribute"));
            txyz v;
            if (sscanf(temp.mb_str(wxConvLocal), "%f", &v.Time) != 1)
                throw RCT3Exception(_("ROTATE tag has malformed time attribute"));

            temp = child->GetNodeContent();
            if (sscanf(temp.mb_str(wxConvLocal), "%f %f %f", &v.X, &v.Y, &v.Z) != 3)
                throw RCT3Exception(_("ROTATE tag has malformed content"));

            rotations.push_back(v);
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in bone tag."), child->GetName().c_str()));
        }
        child = child->GetNext();
    }
    return ret;
}

bool cBoneAnimation::FromNode(wxXmlNode* node, const wxString& path, unsigned long version) {
    bool ret = true;
    wxString temp;
    if (!node)
        return false;
    if (!node->GetName().IsSameAs(RCT3XML_CBONEANIMATION))
        return false;

    name = node->GetPropVal(wxT("name"), wxT(""));

    wxXmlNode* child = node->GetChildren();
    while(child) {
        if (child->GetName() == RCT3XML_CBONEANIMATION_TRANSLATIONS) {
            wxXmlNode* schild = child->GetChildren();
            while(schild) {
                if (schild->GetName() == RCT3XML_CTXYZ) {
                    cTXYZ v;
                    if (!v.FromNode(schild, path, version))
                        ret = false;
                    translations.push_back(v);
                }
                schild = schild->GetNext();
            }
        } else if (child->GetName() == RCT3XML_CBONEANIMATION_ROTATIONS) {
            wxXmlNode* schild = child->GetChildren();
            while(schild) {
                if (schild->GetName() == RCT3XML_CTXYZ) {
                    cTXYZ v;
                    if (!v.FromNode(schild, path, version))
                        ret = false;
                    rotations.push_back(v);
                }
                schild = schild->GetNext();
            }
        }
        child = child->GetNext();
    }
    return ret;
}

wxXmlNode* cBoneAnimation::GetNode(const wxString& path) {
    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_CBONEANIMATION);
    wxXmlNode* lastchild = NULL;
    node->AddProperty(wxT("name"), name);

    wxXmlNode* trans = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_CBONEANIMATION_TRANSLATIONS);
    for (cTXYZ::iterator it = translations.begin(); it != translations.end(); it++) {
        wxXmlNode* newchild = it->GetNode(path);
        if (lastchild)
            lastchild->SetNext(newchild);
        else
            trans->SetChildren(newchild);
        lastchild = newchild;
    }
    node->SetChildren(trans);

    wxXmlNode* rot = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_CBONEANIMATION_ROTATIONS);
    lastchild = NULL;
    for (cTXYZ::iterator it = rotations.begin(); it != rotations.end(); it++) {
        wxXmlNode* newchild = it->GetNode(path);
        if (lastchild)
            lastchild->SetNext(newchild);
        else
            rot->SetChildren(newchild);
        lastchild = newchild;
    }
    trans->SetNext(rot);

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

bool cAnimation::FromCompilerXml(wxXmlNode* node, const wxString& path) {
    bool ret = true;
    if (!node)
        return false;

    name = node->GetPropVal(wxT("name"), wxT(""));
    if (name.IsEmpty())
        throw RCT3Exception(_("BAN tag lacks name attribute"));

    wxXmlNode* child = node->GetChildren();
    while(child) {
        if (child->GetName() == COMPILER_BONE) {
            cBoneAnimation bone;
            if (!bone.FromCompilerXml(child, path))
                ret = false;
            boneanimations.push_back(bone);
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ban tag."), child->GetName().c_str()));
        }
        child = child->GetNext();
    }
    return ret;
}

bool cAnimation::FromNode(wxXmlNode* node, const wxString& path, unsigned long version) {
    bool ret = true;
    wxString temp;
    if (!node)
        return false;
    if (!node->GetName().IsSameAs(RCT3XML_CANIMATION))
        return false;

    name = node->GetPropVal(wxT("name"), wxT(""));

    if (node->GetPropVal(wxT("orientation"), &temp)) {
        unsigned long l = 0;
        if (!temp.ToULong(&l)) {
            usedorientation = ORIENTATION_UNKNOWN;
            ret = false;
        } else {
            usedorientation = static_cast<c3DLoaderOrientation>(l);
        }
    } else {
        usedorientation = ORIENTATION_UNKNOWN;
    }

    wxXmlNode* child = node->GetChildren();
    while(child) {
        if (child->GetName() == RCT3XML_CBONEANIMATION) {
            cBoneAnimation ban;
            ban.FromNode(child, path, version);
            boneanimations.push_back(ban);
        }
        child = child->GetNext();
    }
    return ret;
}

wxXmlNode* cAnimation::GetNode(const wxString& path) {
    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_CANIMATION);
    wxXmlNode* lastchild = NULL;
    node->AddProperty(wxT("name"), name);
    unsigned long l = usedorientation;
    node->AddProperty(wxT("orientation"), wxString::Format(wxT("%lu"), l));

    for (cBoneAnimation::iterator it = boneanimations.begin(); it != boneanimations.end(); it++) {
        wxXmlNode* newchild = it->GetNode(path);
        if (lastchild)
            lastchild->SetNext(newchild);
        else
            node->SetChildren(newchild);
        lastchild = newchild;
    }

    return node;
}


///////////////////////////////////////////////////////////////
//
// cLOD
//
///////////////////////////////////////////////////////////////

bool cLOD::FromNode(wxXmlNode* node, const wxString& path, unsigned long version) {
    bool ret = true;
    wxString temp;
    if (!node)
        return false;
    if (!node->GetName().IsSameAs(RCT3XML_CLOD))
        return false;

    modelname = node->GetPropVal(wxT("modelname"), wxT(""));
    animated = node->GetPropVal(wxT("animated"), wxT("0")).IsSameAs(wxT("1"));
    temp = node->GetPropVal(wxT("distance"), wxT("0.0"));
    if (sscanf(temp.mb_str(wxConvLocal), "%f", &distance) != 1)
        ret = false;
    if (node->GetPropVal(wxT("unk2"), &temp)) {
        if (!temp.ToULong(&unk2)) {
            unk2 = 0;
            ret = false;
        }
    } else {
        unk2 = 0;
    }
    if (node->GetPropVal(wxT("unk4"), &temp)) {
        if (!temp.ToULong(&unk4)) {
            unk4 = 0;
            ret = false;
        }
    } else {
        unk4 = 0;
    }
    if (node->GetPropVal(wxT("unk14"), &temp)) {
        if (!temp.ToULong(&unk14)) {
            unk14 = 0;
            ret = false;
        }
    } else {
        unk14 = 0;
    }

    wxXmlNode* child = node->GetChildren();
    while(child) {
        if (child->GetName() == RCT3XML_CLOD_ANIMATION) {
            wxString anim = child->GetPropVal(wxT("name"), wxT("ERROR"));
            animations.Add(anim);
        }
        child = child->GetNext();
    }
    return ret;
}

wxXmlNode* cLOD::GetNode(const wxString& path) {
    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_CLOD);
    wxXmlNode* lastchild = NULL;
    node->AddProperty(wxT("modelname"), modelname);
    node->AddProperty(wxT("animated"), animated?wxT("1"):wxT("0"));
    node->AddProperty(wxT("distance"), wxString::Format(wxT("%8f"), distance));
    node->AddProperty(wxT("unk2"), wxString::Format(wxT("%lu"), unk2));
    node->AddProperty(wxT("unk4"), wxString::Format(wxT("%lu"), unk4));
    node->AddProperty(wxT("unk14"), wxString::Format(wxT("%lu"), unk14));

    for (cStringIterator it = animations.begin(); it != animations.end(); it++) {
        //wxXmlNode* newchild = new wxXmlNode(node, wxXML_ELEMENT_NODE, RCT3XML_CLOD_ANIMATION);
        wxXmlNode* newchild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_CLOD_ANIMATION);
        newchild->AddProperty(wxT("name"), *it);
        if (lastchild)
            lastchild->SetNext(newchild);
        else
            node->SetChildren(newchild);
        lastchild = newchild;
    }

    return node;
}


///////////////////////////////////////////////////////////////
//
// cSIVSettings
//
///////////////////////////////////////////////////////////////

bool cSIVSettings::FromNode(wxXmlNode* node, const wxString& path, unsigned long version) {
    bool ret = true;
    wxString temp;
    if (!node)
        return false;
    if (!node->GetName().IsSameAs(RCT3XML_CSIVSETTINGS))
        return false;

    if (node->GetPropVal(wxT("sivflags"), &temp)) {
        unsigned long l;
        if (!temp.ToULong(&l)) {
            sivflags = 0;
            ret = false;
        } else {
            sivflags = l;
        }
    } else {
        sivflags = 0;
    }
    temp = node->GetPropVal(wxT("sway"), wxT("0.0"));
    if (sscanf(temp.mb_str(wxConvLocal), "%f", &sway) != 1)
        ret = false;
    temp = node->GetPropVal(wxT("brightness"), wxT("0.0"));
    if (sscanf(temp.mb_str(wxConvLocal), "%f", &brightness) != 1)
        ret = false;
    temp = node->GetPropVal(wxT("unknown"), wxT("0.0"));
    if (sscanf(temp.mb_str(wxConvLocal), "%f", &unknown) != 1)
        ret = false;
    temp = node->GetPropVal(wxT("scale"), wxT("0.0"));
    if (sscanf(temp.mb_str(wxConvLocal), "%f", &scale) != 1)
        ret = false;
    if (node->GetPropVal(wxT("unk6"), &temp)) {
        if (!temp.ToULong(&unk6)) {
            unk6 = 0;
            ret = false;
        }
    } else {
        unk6 = 0;
    }
    if (node->GetPropVal(wxT("unk7"), &temp)) {
        if (!temp.ToULong(&unk7)) {
            unk7 = 0;
            ret = false;
        }
    } else {
        unk7 = 0;
    }
    if (node->GetPropVal(wxT("unk8"), &temp)) {
        if (!temp.ToULong(&unk8)) {
            unk8 = 0;
            ret = false;
        }
    } else {
        unk8 = 0;
    }
    if (node->GetPropVal(wxT("unk9"), &temp)) {
        if (!temp.ToULong(&unk9)) {
            unk9 = 0;
            ret = false;
        }
    } else {
        unk9 = 0;
    }
    if (node->GetPropVal(wxT("unk10"), &temp)) {
        if (!temp.ToULong(&unk10)) {
            unk10 = 0;
            ret = false;
        }
    } else {
        unk10 = 0;
    }
    if (node->GetPropVal(wxT("unk11"), &temp)) {
        if (!temp.ToULong(&unk11)) {
            unk11 = 0;
            ret = false;
        }
    } else {
        unk11 = 0;
    }

    return ret;
}

wxXmlNode* cSIVSettings::GetNode(const wxString& path) {
    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_CSIVSETTINGS);
    node->AddProperty(wxT("sivflags"), wxString::Format(wxT("%u"), sivflags));
    node->AddProperty(wxT("sway"), wxString::Format(wxT("%8f"), sway));
    node->AddProperty(wxT("brightness"), wxString::Format(wxT("%8f"), brightness));
    node->AddProperty(wxT("unknown"), wxString::Format(wxT("%8f"), unknown));
    node->AddProperty(wxT("scale"), wxString::Format(wxT("%8f"), scale));
    node->AddProperty(wxT("unk6"), wxString::Format(wxT("%lu"), unk6));
    node->AddProperty(wxT("unk7"), wxString::Format(wxT("%lu"), unk7));
    node->AddProperty(wxT("unk8"), wxString::Format(wxT("%lu"), unk8));
    node->AddProperty(wxT("unk9"), wxString::Format(wxT("%lu"), unk9));
    node->AddProperty(wxT("unk10"), wxString::Format(wxT("%lu"), unk10));
    node->AddProperty(wxT("unk11"), wxString::Format(wxT("%lu"), unk11));

    return node;
}


