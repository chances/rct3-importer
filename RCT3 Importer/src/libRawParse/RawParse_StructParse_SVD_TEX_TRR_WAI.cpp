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



#include "RawParse_cpp.h"

#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

#include "ManagerSVD.h"
#include "ManagerTEX.h"
#include "ManagerTKS.h"
#include "ManagerTRR.h"
#include "ManagerWAI.h"


#define RAWXML_TKS_SPEED            "speed"
#define RAWXML_TKS_BASIC            "basic"
#define RAWXML_TKS_BASIC_ENTRY      "entry"
#define RAWXML_TKS_BASIC_EXIT       "exit"
#define RAWXML_TKS_SPEED            "speed"
#define RAWXML_TKS_SPLINES          "splines"
#define RAWXML_TKS_SPLINES_CAR      "car"
#define RAWXML_TKS_SPLINES_JOIN     "join"
#define RAWXML_TKS_SPLINES_EXTRA    "extra"
#define RAWXML_TKS_SPLINES_WATER    "water"
#define RAWXML_TKS_ANIMATIONS       "animations"
#define RAWXML_TKS_OPTIONS          "options"
//#define RAWXML_UNKNOWNS "unknowns"
#define RAWXML_TKS_SOAKED           "soaked"
#define RAWXML_TKS_SOAKED_PATH      "path"
//#define RAWXML_RIDE_STATION_LIMIT   "rideStationLimit"
#define RAWXML_TKS_SOAKED_SPEED     "speedSplines"
//#define RAWXML_TKS_SPLINES          "splines"
#define RAWXML_TKS_WILD             "wild"

#define RAWXML_TRR_SECTION          "trackSection"
#define RAWXML_TRR_SECTION_UNIQUE   "trackSectionRemoveDuplicates"
#define RAWXML_TRR_TRAIN            "train"
#define RAWXML_TRR_PATH             "trackPath"
#define RAWXML_TRR_SPECIALS         "specials"
#define RAWXML_TRR_OPTIONS          "options"
#define RAWXML_TRR_STATION          "station"
#define RAWXML_TRR_LAUNCHED         "launched"
#define RAWXML_TRR_CHAIN            "chain"
#define RAWXML_TRR_CONSTANT         "constant"
#define RAWXML_TRR_COST             "cost"
#define RAWXML_TRR_SPLINES          "splines"
#define RAWXML_TRR_TOWER            "tower"
#define RAWXML_TRR_SOAKED           "soaked"
#define RAWXML_TRR_WILD             "wild"

#define RAWXML_TEX_TEXTURE          "texture"

#define RAWXML_WAI_PARAMETERS       "waiparameters"
#define RAWXML_WAI_UNKNOWNS         "waiunknowns"


void cRawParser::ParseSVD(cXmlNode& node) {
    USE_PREFIX(node);
    cSceneryItemVisual svd;
    wxString name = ParseString(node, wxT(RAWXML_SVD), wxT("name"), NULL, useprefix);
    svd.name = name.ToAscii();
    OPTION_PARSE(unsigned long, svd.sivflags, ParseFloat(node, wxT(RAWXML_SVD), wxT("flags")));
    OPTION_PARSE(float, svd.sway, ParseFloat(node, wxT(RAWXML_SVD), wxT("sway")));
    OPTION_PARSE(float, svd.brightness, ParseFloat(node, wxT(RAWXML_SVD), wxT("brightness")));
    OPTION_PARSE(float, svd.scale, ParseFloat(node, wxT(RAWXML_SVD), wxT("scale")));
    wxLogVerbose(wxString::Format(_("Adding svd %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_UNKNOWNS)) {
            OPTION_PARSE(float, svd.unk4, ParseFloat(child, wxT(RAWXML_UNKNOWNS), wxT("u4")));
            OPTION_PARSE(unsigned long, svd.unk6, ParseUnsigned(child, wxT(RAWXML_UNKNOWNS), wxT("u6")));
            OPTION_PARSE(unsigned long, svd.unk7, ParseUnsigned(child, wxT(RAWXML_UNKNOWNS), wxT("u7")));
            OPTION_PARSE(unsigned long, svd.unk8, ParseUnsigned(child, wxT(RAWXML_UNKNOWNS), wxT("u8")));
            OPTION_PARSE(unsigned long, svd.unk9, ParseUnsigned(child, wxT(RAWXML_UNKNOWNS), wxT("u9")));
            OPTION_PARSE(unsigned long, svd.unk10, ParseUnsigned(child, wxT(RAWXML_UNKNOWNS), wxT("u10")));
            OPTION_PARSE(unsigned long, svd.unk11, ParseUnsigned(child, wxT(RAWXML_UNKNOWNS), wxT("u11")));
        } else if (child(RAWXML_SVD_LOD)) {
            USE_PREFIX(child);
            cSceneryItemVisualLOD lod;
            wxString lodname = ParseString(child, wxT(RAWXML_SVD_LOD), wxT("name"), NULL);
            lod.name = lodname.ToAscii();
            lod.meshtype = ParseUnsigned(child, wxT(RAWXML_SVD_LOD), wxT("type"));
            lod.distance = ParseFloat(child, wxT(RAWXML_SVD_LOD), wxT("distance"));
            ParseStringOption(lod.staticshape, child, wxT("staticshape"), NULL, useprefix);
            ParseStringOption(lod.boneshape, child, wxT("boneshape"), NULL, useprefix);
            ParseStringOption(lod.fts, child, wxT("ftx"), NULL, useprefix);
            ParseStringOption(lod.txs, child, wxT("txs"), NULL);

            cXmlNode subchild(child.children());
            while (subchild) {
                DO_CONDITION_COMMENT(subchild);

                if (subchild(RAWXML_UNKNOWNS)) {
                    OPTION_PARSE(unsigned long, lod.unk2, ParseUnsigned(child, wxT(RAWXML_UNKNOWNS), wxT("u2")));
                    OPTION_PARSE(unsigned long, lod.unk4, ParseUnsigned(child, wxT(RAWXML_UNKNOWNS), wxT("u4")));
                    OPTION_PARSE(float, lod.unk7, ParseFloat(child, wxT(RAWXML_UNKNOWNS), wxT("u7")));
                    OPTION_PARSE(float, lod.unk8, ParseFloat(child, wxT(RAWXML_UNKNOWNS), wxT("u8")));
                    OPTION_PARSE(float, lod.unk9, ParseFloat(child, wxT(RAWXML_UNKNOWNS), wxT("u9")));
                    OPTION_PARSE(float, lod.unk10, ParseFloat(child, wxT(RAWXML_UNKNOWNS), wxT("u10")));
                    OPTION_PARSE(float, lod.unk11, ParseFloat(child, wxT(RAWXML_UNKNOWNS), wxT("u11")));
                    OPTION_PARSE(float, lod.unk12, ParseFloat(child, wxT(RAWXML_UNKNOWNS), wxT("u12")));
                    OPTION_PARSE(unsigned long, lod.unk14, ParseUnsigned(child, wxT(RAWXML_UNKNOWNS), wxT("u14")));
                } else if (subchild(RAWXML_SVD_LOD_ANIMATION)) {
                    USE_PREFIX(subchild);
                    wxString anim = UTF8STRINGWRAP(subchild.content());
                    MakeVariable(anim);
                    if (useprefix)
                        lod.animations.push_back(m_prefix + std::string(anim.ToAscii()));
                    else
                        lod.animations.push_back(std::string(anim.ToAscii()));
                } else if (subchild.element()) {
                    throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown tag '%s' in svd(%s)/svdlod(%s)."), STRING_FOR_FORMAT(subchild.name()), name.c_str(), lodname.c_str()), subchild));
                }

                subchild.go_next();
            }

            svd.lods.push_back(lod);
        } else if (child.element()) {
            throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown tag '%s' in svd tag '%s'."), STRING_FOR_FORMAT(child.name()), name.c_str()), child));
        }

        child.go_next();
    }

    ovlSVDManager* c_svd = m_ovl.GetManager<ovlSVDManager>();
    c_svd->AddSVD(svd);
}

void cRawParser::ParseTEX(cXmlNode& node) {
    USE_PREFIX(node);
    wxString name = ParseString(node, wxT(RAWXML_TEX), wxT("name"), NULL, useprefix);
//    wxFileName texture = ParseString(node, RAWXML_TEX, wxT("texture"));
//    if (!texture.IsAbsolute())
//        texture.MakeAbsolute(m_input.GetPathWithSep());
    wxLogVerbose(wxString::Format(_("Adding tex %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    cTextureStruct texture;
    texture.texture.name = name.ToAscii();
    OPTION_PARSE(unsigned long, texture.texture.format, ParseUnsigned(node, wxT(RAWXML_TEX), wxT("format")));
    wxGXImage mainimage;
    unsigned long mips = 0;
    OPTION_PARSE(unsigned long, mips, ParseUnsigned(node, wxT(RAWXML_TEX), wxT("mips")));
    ParseStringOption(texture.texturestyle, node, wxT("txs"), NULL);

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_TEX_TEXTURE)) {
            cTextureMIP mip;
            OPTION_PARSE(unsigned long, mip.dimension, ParseUnsigned(child, wxT(RAWXML_TEX_TEXTURE), wxT("dimension")));

            cXmlNode datanode(child.children());
            while (datanode && (!datanode(RAWXML_DATA)))
                datanode.go_next();

            if (!datanode)
                throw RCT3Exception(FinishNodeError(wxString::Format(_("Tag tex(%s)/texture misses data."), name.c_str()), child));

            cRawDatablock data = GetDataBlock(wxString::Format(_("tag tex(%s)/texture"), name.c_str()), datanode);
            if (data.datatype().IsEmpty()) {
                throw RCT3Exception(FinishNodeError(wxString::Format(_("Could not determine data type in tag tex(%s)/texture."), name.c_str()), child));
            } else if (data.datatype().IsSameAs(wxT("processed"))) {
                unsigned long datadim = cTexture::GetDimension(texture.texture.format, data.datasize());
                if (!mip.dimension)
                    mip.dimension = datadim;
                if (mip.dimension != datadim)
                    throw RCT3Exception(FinishNodeError(wxString::Format(_("Dimension mismatch in tag tex(%s)/texture."), name.c_str()), child));
                mip.data = data.data();
            } else {
                wxGXImage img;
                img.read(data.data().get(), data.datasize(), std::string(data.datatype().mb_str(wxConvLocal)));
                if (!img.Ok())
                    throw RCT3Exception(FinishNodeError(wxString::Format(_("Error decoding image in tag tex(%s)/texture."), name.c_str()), child));
                img.flip();
                if (!mainimage.Ok())
                    mainimage = img;
                if (mip.dimension) {
                    img.Rescale(mip.dimension, mip.dimension);
                } else {
                    mip.dimension = img.GetWidth();
                }
                mip.data = boost::shared_array<unsigned char>(new unsigned char[mip.CalcSize(cTexture::GetBlockSize(texture.texture.format))]);
                switch (texture.texture.format) {
                    case cTexture::FORMAT_A8R8G8B8:
                        img.GetData("RGBA", mip.data.get(), true);
                        break;
                    case cTexture::FORMAT_DXT1:
                        img.DxtCompress(mip.data.get(), wxDXT1);
                        break;
                    case cTexture::FORMAT_DXT3:
                        img.DxtCompress(mip.data.get(), wxDXT3);
                        break;
                    case cTexture::FORMAT_DXT5:
                        img.DxtCompress(mip.data.get(), wxDXT5);
                        break;
                    default:
                        throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown format 0x%02lx in tag tex(%s)/texture."), texture.texture.format, name.c_str()), child));
                }
            }

            texture.texture.mips.insert(mip);
        } else if (child.element()) {
            throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown tag '%s' in tex(%s) tag."), STRING_FOR_FORMAT(child.name()), name.c_str()), child));
        }
        child.go_next();
    }

    if (mips) {
        if ((texture.texture.mips.size() > 1) && (mips != texture.texture.mips.size()))
            throw RCT3Exception(FinishNodeError(wxString::Format(_("Mip count mismatch in tag tex(%s)."), name.c_str()), node));
        if ((texture.texture.mips.size() == 1) && (mips > 1)) {
            if (!mainimage.Ok())
                throw RCT3Exception(FinishNodeError(wxString::Format(_("Cannot auto-generate mips in tag tex(%s)."), name.c_str()), node));
            for (unsigned long i = 1; i < mips; ++i) {
                mainimage.Rescale(mainimage.GetWidth() / 2, mainimage.GetHeight() / 2);
                if (mainimage.GetWidth() < 4) {
                    wxLogWarning(FinishNodeError(wxString::Format(_("Too many mips requested in tag tex(%s)."), name.c_str()), node));
                    break;
                }
                cTextureMIP mip;
                mip.dimension = mainimage.GetWidth();
                mip.data = boost::shared_array<unsigned char>(new unsigned char[mip.CalcSize(cTexture::GetBlockSize(texture.texture.format))]);
                switch (texture.texture.format) {
                    case cTexture::FORMAT_A8R8G8B8:
                        mainimage.GetData("RGBA", mip.data.get(), true);
                        break;
                    case cTexture::FORMAT_DXT1:
                        mainimage.DxtCompress(mip.data.get(), wxDXT1);
                        break;
                    case cTexture::FORMAT_DXT3:
                        mainimage.DxtCompress(mip.data.get(), wxDXT3);
                        break;
                    case cTexture::FORMAT_DXT5:
                        mainimage.DxtCompress(mip.data.get(), wxDXT5);
                        break;
                    default:
                        throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown format 0x%02lx in tag tex(%s)."), texture.texture.format, name.c_str()), node));
                }
                texture.texture.mips.insert(mip);
            }
        }
    }

/*
    counted_array_ptr<unsigned char> data;
    unsigned long dimension = 0;
    unsigned long datasize = 0;

    wxXmlNode* child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_TEX_FILE) {
            if (data.get() && (m_mode != MODE_BAKE))
                throw RCT3Exception(wxString(wxT("tex tag '"))+name+wxT("': multiple datasources."));

            wxString t = child->GetNodeContent();
            bool filevar = MakeVariable(t);
            wxFSFileName texture = t;
            if (!texture.IsAbsolute())
                texture.MakeAbsolute(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));

            if (m_mode == MODE_BAKE) {
                if (m_bake.Index(RAWXML_TEX) == wxNOT_FOUND) {
                    if (!filevar) {
                        if (m_bake.Index(RAWBAKE_ABSOLUTE) == wxNOT_FOUND) {
                            texture.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                        }
                        delete child->GetChildren();
                        child->SetChildren(new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), texture.GetFullPath()));
                    }
                    child = child->GetNext();
                    continue;
                }
            }

//            if (!texture.IsFileReadable())
//                throw RCT3Exception(_("tex tag: File not readable: ") + texture.GetFullPath());

            try {
                wxGXImage img(texture.GetFullPath(), false);
                if (!img.Ok())
                    throw RCT3Exception(_("tex tag: Error reading image file: ") + texture.GetFullPath());

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
                datasize = img.GetDxtBufferSize(wxDXT3);
                dimension = width;
                data = counted_array_ptr<unsigned char>(new unsigned char[datasize]);
                img.DxtCompress(data.get(), wxDXT3);

                if (m_mode == MODE_BAKE) {
                    counted_array_ptr<char> buf(new char[datasize * 4]);
                    unsigned long outlen = datasize * 4;
                    int bret = base64_encode(data.get(), datasize, reinterpret_cast<unsigned char*>(buf.get()), &outlen);
                    switch (bret) {
                        case CRYPT_OK:
                            break;
                        case CRYPT_BUFFER_OVERFLOW:
                            throw RCT3Exception(wxString(_("Buffer overflow baking texture '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                        default:
                            throw RCT3Exception(wxString(_("Unknown base64 error baking texture '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                    }
                    child->SetName(RAWXML_TEX_DATA);
                    child->AddProperty(wxT("dimension"), wxString::Format(wxT("%u"), img.GetWidth()));
                    delete child->GetChildren();
                    child->SetChildren(new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), wxString(buf.get(), wxConvLocal)));
                }

            } catch (Magick::Exception& e) {
                throw RCT3Exception(wxString::Format(_("tex tag: GarphicsMagik exception: %s"), e.what()));
            }
        } else if (child->GetName() == RAWXML_TEX_DATA) {
            if (m_mode == MODE_BAKE) {
                child = child->GetNext();
                continue;
            }
            if (data.get())
                throw RCT3Exception(wxString(wxT("tex tag '"))+name+wxT("': multiple datasources."));
            dimension = ParseUnsigned(child, RAWXML_TEX_DATA, wxT("dimension"));
            datasize = squish::GetStorageRequirements(dimension, dimension, wxDXT3);
            unsigned long outlen = datasize;

            data = counted_array_ptr<unsigned char>(new unsigned char[datasize]);
            wxString tex = child->GetNodeContent();
            MakeVariable(tex);
            int bret = base64_decode(reinterpret_cast<const unsigned char*>(static_cast<const char*>(tex.ToAscii())), tex.Length(), data.get(), &outlen);
            switch (bret) {
                case CRYPT_OK:
                    break;
                case CRYPT_INVALID_PACKET:
                    throw RCT3Exception(wxString(_("Decoding error in tex tag ")) + name);
                case CRYPT_BUFFER_OVERFLOW:
                    throw RCT3Exception(wxString(_("Buffer overflow decoding tex tag ")) + name);
                default:
                    throw RCT3Exception(wxString(_("Unknown base64 error decoding tex tag ")) + name);
            }
            if (outlen != datasize)
                throw RCT3Exception(wxString(_("Datasize mismatch error in tex tag ")) + name);

        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in tex tag."), child->GetName().c_str()));
        }
        child = child->GetNext();
    }
*/

    if (m_mode != MODE_BAKE) {
        if (!texture.texture.mips.size())
            throw RCT3Exception(wxString(wxT("tex tag '"))+name+wxT("': no data."));
        ovlTEXManager* c_tex = m_ovl.GetManager<ovlTEXManager>();
        c_tex->AddTexture(texture);
        //c_tex->AddTexture(std::string(name.ToAscii()), dimension, datasize, reinterpret_cast<unsigned long*>(data.get()));
    }
}

#define ParseEntryExit(node, strct, tag) \
    OPTION_PARSE(unsigned long, strct.curve, ParseUnsigned(node, tag, wxT("curve"))); \
    OPTION_PARSE(unsigned long, strct.flags, ParseUnsigned(node, tag, wxT("flags"))); \
    OPTION_PARSE(unsigned long, strct.unknown, ParseUnsigned(node, tag, wxT("unknown"))); \
    OPTION_PARSE(unsigned long, strct.slope, ParseUnsigned(node, tag, wxT("slope"))); \
    OPTION_PARSE(unsigned long, strct.bank, ParseUnsigned(node, tag, wxT("bank"))); \
    ParseStringOption(strct.group, child, wxT("group"), NULL); \

#define ParseSplinePair(node, strct, tag) \
    strct.left = ParseString(node, tag, wxT("left"), NULL, useprefix).ToAscii(); \
    strct.right = ParseString(node, tag, wxT("right"), NULL, useprefix).ToAscii();

void cRawParser::ParseTKS(cXmlNode& node) {
    USE_PREFIX(node);
    cTrackSection section;
    section.name = ParseString(node, wxT(RAWXML_TKS), wxT("name"), NULL, useprefix).ToAscii();
    section.internalname = ParseString(node, wxT(RAWXML_TKS), wxT("internalname"), NULL, false).ToAscii();
    OPTION_PARSE(unsigned long, section.version, ParseUnsigned(node, RAWXML_TKS, wxT("version")));
    section.sid = ParseString(node, wxT(RAWXML_TKS), wxT("sid"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding tks %s to %s."), wxString(section.name.c_str()).c_str(), m_output.GetFullPath().c_str()));

    foreach (const cXmlNode& child, node.children()) {
        USE_PREFIX(child);
        DO_CONDITION_COMMENT_FOR(child);

        if (child(RAWXML_TKS_SPEED)) {
            cTrackSectionSpeed speed;
            speed.unk01 = ParseFloat(child, RAWXML_TKS_SPEED, wxT("u1"));
            speed.unk02 = ParseFloat(child, RAWXML_TKS_SPEED, wxT("u2"));
            speed.unk03 = ParseFloat(child, RAWXML_TKS_SPEED, wxT("u3"));
            OPTION_PARSE(float, speed.unk04, ParseFloat(child, RAWXML_TKS_SPEED, wxT("u4")));
            OPTION_PARSE(float, speed.unk05, ParseFloat(child, RAWXML_TKS_SPEED, wxT("u5")));
            section.speeds.push_back(speed);
        } else if (child(RAWXML_TKS_BASIC)) {
            OPTION_PARSE(unsigned long, section.basic.special, ParseUnsigned(child, RAWXML_TKS_BASIC, wxT("special")));
            OPTION_PARSE(unsigned long, section.basic.direction, ParseUnsigned(child, RAWXML_TKS_BASIC, wxT("direction")));

            foreach(const cXmlNode& subchild, child.children()) {
                USE_PREFIX(subchild);
                DO_CONDITION_COMMENT_FOR(subchild);

                if (subchild(RAWXML_TKS_BASIC_ENTRY)) {
                    ParseEntryExit(subchild, section.basic.entry, RAWXML_TKS_BASIC_ENTRY);
                } else if (subchild(RAWXML_TKS_BASIC_EXIT)) {
                    ParseEntryExit(subchild, section.basic.exit, RAWXML_TKS_BASIC_EXIT);
                } else if (subchild.element()) {
                    throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown tag '%s' in tks/basic tag."), STRING_FOR_FORMAT(subchild.name())), subchild));
                }
            }
        } else if (child(RAWXML_TKS_SPLINES)) {
            foreach(const cXmlNode& subchild, child.children()) {
                USE_PREFIX(subchild);
                DO_CONDITION_COMMENT_FOR(subchild);

                if (subchild(RAWXML_TKS_SPLINES_CAR)) {
                    ParseSplinePair(subchild, section.splines.car, RAWXML_TKS_SPLINES_CAR);
                } else if (subchild(RAWXML_TKS_SPLINES_JOIN)) {
                    ParseSplinePair(subchild, section.splines.join, RAWXML_TKS_SPLINES_JOIN);
                } else if (subchild(RAWXML_TKS_SPLINES_EXTRA)) {
                    ParseSplinePair(subchild, section.splines.extra, RAWXML_TKS_SPLINES_EXTRA);
                } else if (subchild(RAWXML_TKS_SPLINES_WATER)) {
                    ParseSplinePair(subchild, section.splines.water, RAWXML_TKS_SPLINES_WATER);
                } else if (subchild.element()) {
                    throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown tag '%s' in tks/splines tag."), STRING_FOR_FORMAT(subchild.name())), subchild));
                }
            }
        } else if (child(RAWXML_TKS_ANIMATIONS)) {
            OPTION_PARSE(long, section.animations.stopped, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("stopped")));
            OPTION_PARSE(long, section.animations.starting, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("starting")));
            OPTION_PARSE(long, section.animations.running, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("running")));
            OPTION_PARSE(long, section.animations.stopping, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("stopping")));
            OPTION_PARSE(long, section.animations.hold_after_train_stop, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("holdAfterTrainStop")));
            OPTION_PARSE(long, section.animations.hold_loop, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("holdLoop")));
            OPTION_PARSE(long, section.animations.hold_before_release, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("holdBeforeRelease")));
            OPTION_PARSE(long, section.animations.hold_leaving, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("holdLeaving")));
            OPTION_PARSE(long, section.animations.hold_after_train_left, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("holdAfterTrainLeft")));
            OPTION_PARSE(long, section.animations.pre_station_leave, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("preStationLeave")));
            OPTION_PARSE(long, section.animations.rotating_tower_idle, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("rotatingTowerIdle")));
        } else if (child(RAWXML_UNKNOWNS)) {
            OPTION_PARSE(unsigned long, section.unknowns.unk15, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u15")));
            OPTION_PARSE(unsigned long, section.unknowns.unk16, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u16")));
            OPTION_PARSE(unsigned long, section.unknowns.unk17, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u17")));
            OPTION_PARSE(unsigned long, section.unknowns.unk22, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u22")));
            OPTION_PARSE(unsigned long, section.unknowns.unk23, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u23")));
            OPTION_PARSE(unsigned long, section.unknowns.unk24, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u24")));
            OPTION_PARSE(unsigned long, section.unknowns.unk45, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u45")));
            OPTION_PARSE(unsigned long, section.unknowns.unk47, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u47")));
            OPTION_PARSE(unsigned long, section.unknowns.unk53, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u53")));
            OPTION_PARSE(unsigned long, section.unknowns.unk68, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u68")));
            OPTION_PARSE(unsigned long, section.unknowns.unk69, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u69")));
            OPTION_PARSE(float, section.unknowns.unk77, ParseFloat(child, RAWXML_UNKNOWNS, wxT("u77")));
            OPTION_PARSE(float, section.unknowns.unk78, ParseFloat(child, RAWXML_UNKNOWNS, wxT("u78")));
            OPTION_PARSE(float, section.unknowns.unk79, ParseFloat(child, RAWXML_UNKNOWNS, wxT("u79")));
        } else if (child(RAWXML_TKS_OPTIONS)) {
            OPTION_PARSE(unsigned long, section.options.tower_ride_base_flag, ParseUnsigned(child, RAWXML_TKS_OPTIONS, wxT("towerRideBase")));
            OPTION_PARSE(float, section.options.tower_unkf01, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("tower")));
            OPTION_PARSE(float, section.options.water_splash01, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("waterSplash01")));
            OPTION_PARSE(float, section.options.water_splash02, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("waterSplash02")));
            OPTION_PARSE(float, section.options.reverser_val, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("reverser")));
            OPTION_PARSE(float, section.options.elevator_top_val, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("elevatorTop")));
            OPTION_PARSE(float, section.options.rapids01, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("rapids01")));
            OPTION_PARSE(float, section.options.rapids02, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("rapids02")));
            OPTION_PARSE(float, section.options.rapids03, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("rapids03")));
            OPTION_PARSE(float, section.options.whirlpool01, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("whirlpool01")));
            OPTION_PARSE(float, section.options.whirlpool02, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("whirlpool02")));
            ParseStringOption(section.options.chair_lift_station_end, child, wxT("chairLiftStationEnd"), NULL);
        } else if (child(RAWXML_TKS_SOAKED)) {
            ParseStringOption(section.soaked.loop_path, child, wxT("loopSpline"), NULL, useprefix);
            OPTION_PARSE(float, section.soaked.tower_unkf2, ParseFloat(child, RAWXML_TKS_SOAKED, wxT("tower")));
            OPTION_PARSE(float, section.soaked.aquarium_val, ParseFloat(child, RAWXML_TKS_SOAKED, wxT("aquarium")));
            ParseStringOption(section.soaked.auto_group, child, wxT("autoGroup"), NULL);
            OPTION_PARSE(long, section.soaked.giant_flume_val, ParseSigned(child, RAWXML_TKS_SOAKED, wxT("giantFlume")));
            OPTION_PARSE(unsigned long, section.soaked.entry_wide_flag, ParseUnsigned(child, RAWXML_TKS_SOAKED, wxT("entryWide")));
            OPTION_PARSE(unsigned long, section.soaked.exit_wide_flag, ParseUnsigned(child, RAWXML_TKS_SOAKED, wxT("exitWide")));
            OPTION_PARSE(float, section.soaked.speed_spline_val, ParseFloat(child, RAWXML_TKS_SOAKED, wxT("speedSpline")));
            OPTION_PARSE(float, section.soaked.slide_end_to_lifthill_val, ParseFloat(child, RAWXML_TKS_SOAKED, wxT("slideEndToLifthill")));
            OPTION_PARSE(unsigned long, section.soaked.soaked_options, ParseUnsigned(child, RAWXML_TKS_SOAKED, wxT("flags")));

            foreach(const cXmlNode& subchild, child.children()) {
                USE_PREFIX(subchild);
                DO_CONDITION_COMMENT_FOR(subchild);

                if (subchild(RAWXML_TKS_SOAKED_PATH)) {
                    string path;
                    ParseStringOption(path, subchild, wxT("spline"), NULL, useprefix);
                    section.soaked.paths.push_back(path);
                } else if (subchild(RAWXML_RIDE_STATION_LIMIT)) {
                    cRideStationLimit limit;
                    ParseRideStationLimit(subchild, limit);
                    section.soaked.ride_station_limits.push_back(limit);
                } else if (subchild(RAWXML_TKS_SOAKED_SPEED)) {
                    cTrackSectionSpeedSplines speeds;
                    speeds.speed = ParseFloat(subchild, RAWXML_TKS_SOAKED_SPEED, wxT("speed"));

                    foreach(const cXmlNode& speedchild, child.children()) {
                        USE_PREFIX(speedchild);
                        DO_CONDITION_COMMENT_FOR(speedchild);

                        if (speedchild(RAWXML_TKS_SPLINES)) {
                            ParseSplinePair(speedchild, speeds.splines, RAWXML_TKS_SPLINES);
                        } else if (speedchild.element()) {
                            throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown tag '%s' in tks/soked/speedSplines tag."), STRING_FOR_FORMAT(speedchild.name())), speedchild));
                        }
                    }
                    section.soaked.speeds.push_back(speeds);
                } else if (subchild("groupIsAtEntry")) {
                    string gr(subchild());
                    boost::trim(gr);
                    section.soaked.groups_is_at_entry.push_back(gr);
                } else if (subchild("groupIsAtExit")) {
                    string gr(subchild());
                    boost::trim(gr);
                    section.soaked.groups_is_at_exit.push_back(gr);
                } else if (subchild("groupRequireAtEntry")) {
                    string gr(subchild());
                    boost::trim(gr);
                    section.soaked.groups_must_have_at_entry.push_back(gr);
                } else if (subchild("groupRequireAtExit")) {
                    string gr(subchild());
                    boost::trim(gr);
                    section.soaked.groups_must_have_at_exit.push_back(gr);
                } else if (subchild("groupRefuseAtEntry")) {
                    string gr(subchild());
                    boost::trim(gr);
                    section.soaked.groups_must_not_be_at_entry.push_back(gr);
                } else if (subchild("groupRefuseAtExit")) {
                    string gr(subchild());
                    boost::trim(gr);
                    section.soaked.groups_must_not_be_at_exit.push_back(gr);
                } else if (subchild.element()) {
                    throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown tag '%s' in tks/soaked tag."), STRING_FOR_FORMAT(subchild.name())), subchild));
                }
            }

        } else if (child(RAWXML_TKS_WILD)) {
            OPTION_PARSE(unsigned long, section.wild.splitter_half, ParseUnsigned(child, RAWXML_TKS_WILD, wxT("splitterHalf")));
            ParseStringOption(section.wild.splitter_joined_other_ref, child, wxT("splitterJoint"), NULL, useprefix);
            OPTION_PARSE(unsigned long, section.wild.rotator_type, ParseUnsigned(child, RAWXML_TKS_WILD, wxT("rotatorType")));
            OPTION_PARSE(float, section.wild.animal_house, ParseFloat(child, RAWXML_TKS_WILD, wxT("animalHouse")));
            ParseStringOption(section.wild.alternate_text_lookup, child, wxT("alteranteTextLookup"), NULL);
            OPTION_PARSE(float, section.wild.tower_cap01, ParseFloat(child, RAWXML_TKS_WILD, wxT("towerCap01")));
            OPTION_PARSE(float, section.wild.tower_cap02, ParseFloat(child, RAWXML_TKS_WILD, wxT("towerCap02")));
        } else if (child.element()) {
            throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown tag '%s' in tks tag."), STRING_FOR_FORMAT(child.name())), child));
        }
    }

    ovlTKSManager* c_tks = m_ovl.GetManager<ovlTKSManager>();
    c_tks->AddSection(section);
    wxLogVerbose(wxString::Format(_("Adding tks %s to %s. -- Done"), wxString(section.name.c_str()).c_str(), m_output.GetFullPath().c_str()));
}

bool TrackSectionSort(const cTrackedRideSection& a, const cTrackedRideSection& b) {
    return stricmp(a.name.c_str(), b.name.c_str())<0;
}

bool TrackSectionSame(const cTrackedRideSection& a, const cTrackedRideSection& b) {
    return !stricmp(a.name.c_str(), b.name.c_str());
}

void cRawParser::ParseTRR(cXmlNode& node) {
    USE_PREFIX(node);
    cTrackedRide ride;
    ride.name = ParseString(node, wxT(RAWXML_TRR), wxT("name"), NULL, useprefix).ToAscii();
    ride.attraction.name = ParseString(node, wxT(RAWXML_TRR), wxT("nametxt"), NULL, useprefix).ToAscii();
    ride.attraction.description = ParseString(node, wxT(RAWXML_TRR), wxT("description"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding trr %s to %s."), wxString(ride.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    foreach (const cXmlNode& child, node.children()) {
        DO_CONDITION_COMMENT_FOR(child);

        if (child(RAWXML_ATTRACTION)) {
            ParseAttraction(child, ride.attraction);
        } else if (child(RAWXML_RIDE)) {
            ParseRide(child, ride.ride);
        } else if (child(RAWXML_TRR_SECTION)) {
            cTrackedRideSection section;
            unsigned long split = 1;
            string sect(child());
            boost::trim(sect);
            OPTION_PARSE(unsigned long, section.cost, ParseUnsigned(child, RAWXML_TRR_SECTION, wxT("cost")));
            OPTION_PARSE(unsigned long, split, ParseUnsigned(child, RAWXML_TRR_SECTION, wxT("split")));

            if (split) {
                boost::char_separator<char> sep(" ,");
                typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
                tokenizer tok(sect, sep);
                foreach(const string& token, tok) {
                    section.name = token;
                    ride.sections.push_back(section);
                }
            } else {
                section.name = sect;
                ride.sections.push_back(section);
            }
        } else if (child(RAWXML_TRR_SECTION_UNIQUE)) {
            sort(ride.sections.begin(), ride.sections.end(), TrackSectionSort);
            unique(ride.sections.begin(), ride.sections.end(), TrackSectionSame);
        } else if (child(RAWXML_TRR_TRAIN)) {
            ride.trains.push_back(boost::trim_copy(child()));
        } else if (child(RAWXML_TRR_PATH)) {
            ride.trackpaths.push_back(boost::trim_copy(child()));
        } else if (child(RAWXML_TRR_SPECIALS)) {
            ParseStringOption(ride.specials.cable_lift, child, wxT("cableLift"), NULL);
            OPTION_PARSE(float, ride.specials.cable_lift_unk1, ParseFloat(child, RAWXML_TRR_SPECIALS, wxT("cableLift_u1")));
            OPTION_PARSE(float, ride.specials.cable_lift_unk2, ParseFloat(child, RAWXML_TRR_SPECIALS, wxT("cableLift_u2")));
            ParseStringOption(ride.specials.lift_car, child, wxT("liftCar"), NULL);
            OPTION_PARSE(float, ride.specials.unk28, ParseFloat(child, RAWXML_TRR_SPECIALS, wxT("u28")));
            OPTION_PARSE(float, ride.specials.unk29, ParseFloat(child, RAWXML_TRR_SPECIALS, wxT("u29")));
        } else if (child(RAWXML_TRR_OPTIONS)) {
            OPTION_PARSE(unsigned long, ride.options.only_on_water, ParseUnsigned(child, RAWXML_TRR_OPTIONS, wxT("onlyOnWater")));
            OPTION_PARSE(unsigned long, ride.options.blocks_possible, ParseUnsigned(child, RAWXML_TRR_OPTIONS, wxT("blocksPossible")));
            OPTION_PARSE(unsigned long, ride.options.car_rotation, ParseUnsigned(child, RAWXML_TRR_OPTIONS, wxT("carRotation")));
            OPTION_PARSE(unsigned long, ride.options.loop_not_allowed, ParseUnsigned(child, RAWXML_TRR_OPTIONS, wxT("loopNotAllowed")));
            OPTION_PARSE(unsigned long, ride.options.deconstruct_everywhere, ParseUnsigned(child, RAWXML_TRR_OPTIONS, wxT("deconstructEverywhere")));
        } else if (child(RAWXML_TRR_STATION)) {
            ParseStringOption(ride.station.name, child, wxT("name"), NULL);
            OPTION_PARSE(unsigned long, ride.station.platform_height_over_track, ParseUnsigned(child, RAWXML_TRR_STATION, wxT("platformHeightOverTrack")));
            OPTION_PARSE(unsigned long, ride.station.start_preset, ParseUnsigned(child, RAWXML_TRR_STATION, wxT("startPreset")));
            OPTION_PARSE(unsigned long, ride.station.start_possibilities, ParseUnsigned(child, RAWXML_TRR_STATION, wxT("startPossibilities")));
            OPTION_PARSE(float, ride.station.station_roll_speed, ParseFloat(child, RAWXML_TRR_STATION, wxT("stationRollSpeed")));
            OPTION_PARSE(unsigned long, ride.station.modus_flags, ParseUnsigned(child, RAWXML_TRR_STATION, wxT("modusFlags")));
        } else if (child(RAWXML_TRR_LAUNCHED)) {
            OPTION_PARSE(float, ride.launched.preset, ParseFloat(child, RAWXML_TRR_LAUNCHED, wxT("preset")));
            OPTION_PARSE(float, ride.launched.min, ParseFloat(child, RAWXML_TRR_LAUNCHED, wxT("min")));
            OPTION_PARSE(float, ride.launched.max, ParseFloat(child, RAWXML_TRR_LAUNCHED, wxT("max")));
            OPTION_PARSE(float, ride.launched.step, ParseFloat(child, RAWXML_TRR_LAUNCHED, wxT("step")));
        } else if (child(RAWXML_TRR_CHAIN)) {
            OPTION_PARSE(float, ride.chain.preset, ParseFloat(child, RAWXML_TRR_CHAIN, wxT("preset")));
            OPTION_PARSE(float, ride.chain.min, ParseFloat(child, RAWXML_TRR_CHAIN, wxT("min")));
            OPTION_PARSE(float, ride.chain.max, ParseFloat(child, RAWXML_TRR_CHAIN, wxT("max")));
            OPTION_PARSE(float, ride.chain.step, ParseFloat(child, RAWXML_TRR_CHAIN, wxT("step")));
            OPTION_PARSE(float, ride.chain.lock, ParseFloat(child, RAWXML_TRR_CHAIN, wxT("lock")));
        } else if (child(RAWXML_TRR_CONSTANT)) {
            OPTION_PARSE(float, ride.constant.preset, ParseFloat(child, RAWXML_TRR_CONSTANT, wxT("preset")));
            OPTION_PARSE(float, ride.constant.min, ParseFloat(child, RAWXML_TRR_CONSTANT, wxT("min")));
            OPTION_PARSE(float, ride.constant.max, ParseFloat(child, RAWXML_TRR_CONSTANT, wxT("max")));
            OPTION_PARSE(float, ride.constant.step, ParseFloat(child, RAWXML_TRR_CONSTANT, wxT("step")));
            OPTION_PARSE(float, ride.constant.up_down_variation, ParseFloat(child, RAWXML_TRR_CONSTANT, wxT("upDownVariation")));
        } else if (child(RAWXML_TRR_COST)) {
            OPTION_PARSE(unsigned long, ride.cost.upkeep_per_train, ParseUnsigned(child, RAWXML_TRR_COST, wxT("upkeepPerTrain")));
            OPTION_PARSE(unsigned long, ride.cost.upkeep_per_car, ParseUnsigned(child, RAWXML_TRR_COST, wxT("upkeepPerCar")));
            OPTION_PARSE(unsigned long, ride.cost.upkeep_per_station, ParseUnsigned(child, RAWXML_TRR_COST, wxT("upkeepPerStation")));
            OPTION_PARSE(unsigned long, ride.cost.ride_cost_preview1, ParseUnsigned(child, RAWXML_TRR_COST, wxT("costPreview1")));
            OPTION_PARSE(unsigned long, ride.cost.ride_cost_preview2, ParseUnsigned(child, RAWXML_TRR_COST, wxT("costPreview2")));
            OPTION_PARSE(unsigned long, ride.cost.ride_cost_preview3, ParseUnsigned(child, RAWXML_TRR_COST, wxT("costPreview3")));
            OPTION_PARSE(unsigned long, ride.cost.cost_per_4h_height, ParseUnsigned(child, RAWXML_TRR_COST, wxT("costPer4h")));
        } else if (child(RAWXML_TRR_SPLINES)) {
            OPTION_PARSE(unsigned long, ride.splines.auto_complete, ParseUnsigned(child, RAWXML_TRR_SPLINES, wxT("autoComplete")));
            ParseStringOption(ride.splines.track, child, wxT("track"), NULL);
            ParseStringOption(ride.splines.track_big, child, wxT("trackBig"), NULL);
            ParseStringOption(ride.splines.car, child, wxT("car"), NULL);
            ParseStringOption(ride.splines.car_swing, child, wxT("carSwing"), NULL);
            OPTION_PARSE(float, ride.splines.free_space_profile_height, ParseFloat(child, RAWXML_TRR_SPLINES, wxT("profileHight")));
        } else if (child(RAWXML_TRR_TOWER)) {
            OPTION_PARSE(float, ride.tower.top_duration, ParseFloat(child, RAWXML_TRR_TOWER, wxT("topDuration")));
            OPTION_PARSE(float, ride.tower.top_distance, ParseFloat(child, RAWXML_TRR_TOWER, wxT("topDistance")));
            ParseStringOption(ride.tower.top, child, wxT("top"), NULL);
            ParseStringOption(ride.tower.mid, child, wxT("mid"), NULL);
            ParseStringOption(ride.tower.other_top, child, wxT("otherTop"), NULL);
            ParseStringOption(ride.tower.other_mid, child, wxT("otherMid"), NULL);
            ParseStringOption(ride.tower.other_top_flipped, child, wxT("otherTopFlipped"), NULL);
            ParseStringOption(ride.tower.other_mid_flipped, child, wxT("otherMidFlipped"), NULL);
        } else if (child(RAWXML_UNKNOWNS)) {
            OPTION_PARSE(float, ride.unknowns.unk31, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u31")));
            OPTION_PARSE(float, ride.unknowns.unk32, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u32")));
            OPTION_PARSE(float, ride.unknowns.unk33, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u33")));
            OPTION_PARSE(float, ride.unknowns.unk34, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u34")));
            OPTION_PARSE(unsigned long, ride.unknowns.unk38, ParseUnsigned(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u38")));
            OPTION_PARSE(float, ride.unknowns.unk43, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u43")));
            OPTION_PARSE(float, ride.unknowns.unk45, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u45")));
            OPTION_PARSE(float, ride.unknowns.unk48, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u48")));
            OPTION_PARSE(float, ride.unknowns.unk49, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u49")));
            OPTION_PARSE(float, ride.unknowns.unk50, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u50")));
            OPTION_PARSE(float, ride.unknowns.unk51, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u51")));
            OPTION_PARSE(unsigned long, ride.unknowns.unk57, ParseUnsigned(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u57")));
            OPTION_PARSE(unsigned long, ride.unknowns.unk58, ParseUnsigned(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u58")));
            OPTION_PARSE(unsigned long, ride.unknowns.unk59, ParseUnsigned(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u59")));
            OPTION_PARSE(unsigned long, ride.unknowns.unk60, ParseUnsigned(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u60")));
            OPTION_PARSE(unsigned long, ride.unknowns.unk61, ParseUnsigned(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u61")));
            OPTION_PARSE(unsigned long, ride.unknowns.unk69, ParseUnsigned(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u69")));
            OPTION_PARSE(float, ride.unknowns.unk95, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u95")));
        } else if (child(RAWXML_TRR_SOAKED)) {
            OPTION_PARSE(unsigned long, ride.soaked.unk80, ParseUnsigned(child, RAWXML_TRR_SOAKED, wxT("u80")));
            OPTION_PARSE(unsigned long, ride.soaked.unk81, ParseUnsigned(child, RAWXML_TRR_SOAKED, wxT("u81")));
            OPTION_PARSE(float, ride.soaked.unk82, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u82")));
            OPTION_PARSE(unsigned long, ride.soaked.unk85, ParseUnsigned(child, RAWXML_TRR_SOAKED, wxT("u85")));
            OPTION_PARSE(unsigned long, ride.soaked.unk86, ParseUnsigned(child, RAWXML_TRR_SOAKED, wxT("u86")));
            OPTION_PARSE(unsigned long, ride.soaked.unk87, ParseUnsigned(child, RAWXML_TRR_SOAKED, wxT("u87")));
            OPTION_PARSE(long, ride.soaked.unk88, ParseSigned(child, RAWXML_TRR_SOAKED, wxT("u88")));
            OPTION_PARSE(float, ride.soaked.unk89, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u89")));
            OPTION_PARSE(float, ride.soaked.unk90, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u90")));
            OPTION_PARSE(float, ride.soaked.unk93, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u93")));
            OPTION_PARSE(float, ride.soaked.unk94, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u94")));
            OPTION_PARSE(unsigned long, ride.soaked.unk95, ParseUnsigned(child, RAWXML_TRR_SOAKED, wxT("u95")));
            OPTION_PARSE(long, ride.soaked.unk96, ParseSigned(child, RAWXML_TRR_SOAKED, wxT("u96")));
            OPTION_PARSE(unsigned long, ride.soaked.unk97, ParseUnsigned(child, RAWXML_TRR_SOAKED, wxT("u97")));
            OPTION_PARSE(unsigned long, ride.soaked.short_struct, ParseUnsigned(child, RAWXML_TRR_SOAKED, wxT("shortStruct")));
            OPTION_PARSE(float, ride.soaked.unk99, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u99")));
            OPTION_PARSE(float, ride.soaked.unk100, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u100")));
            OPTION_PARSE(float, ride.soaked.unk101, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u101")));
            OPTION_PARSE(float, ride.soaked.unk102, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u102")));
            OPTION_PARSE(float, ride.soaked.unk103, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u103")));
        } else if (child(RAWXML_TRR_WILD)) {
            ParseStringOption(ride.wild.splitter, child, wxT("splitter"), NULL);
            OPTION_PARSE(unsigned long, ride.wild.robo_flag, ParseUnsigned(child, RAWXML_TRR_WILD, wxT("boneControl")));
            OPTION_PARSE(unsigned long, ride.wild.spinner_control, ParseUnsigned(child, RAWXML_TRR_WILD, wxT("spinnerControl")));
            OPTION_PARSE(unsigned long, ride.wild.unk107, ParseUnsigned(child, RAWXML_TRR_WILD, wxT("u107")));
            OPTION_PARSE(unsigned long, ride.wild.unk108, ParseUnsigned(child, RAWXML_TRR_WILD, wxT("u108")));
            OPTION_PARSE(unsigned long, ride.wild.split_flag, ParseUnsigned(child, RAWXML_TRR_WILD, wxT("splitFlag")));
            ParseStringOption(ride.wild.internalname, child, wxT("name"), NULL);
        } else if (child.element()) {
            throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown tag '%s' in trr tag."), STRING_FOR_FORMAT(child.name())), child));
        }
    }

    ovlTRRManager* c_trr = m_ovl.GetManager<ovlTRRManager>();
    c_trr->AddRide(ride);
    wxLogVerbose(wxString::Format(_("Adding trr %s to %s. -- Done"), wxString(ride.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));
}

void cRawParser::ParseWAI(cXmlNode& node) {
    USE_PREFIX(node);
    cWildAnimalItem wai;
    wxString name = ParseString(node, wxT(RAWXML_WAI), wxT("name"), NULL, useprefix);
    wai.name = name.ToAscii();
    wai.shortname = ParseString(node, wxT(RAWXML_WAI), wxT("shortname"), NULL).ToAscii();
    wai.nametxt = ParseString(node, wxT(RAWXML_WAI), wxT("nametxt"), NULL, useprefix).ToAscii();
    wai.description = ParseString(node, wxT(RAWXML_WAI), wxT("description"), NULL, useprefix).ToAscii();
    wai.icon = ParseString(node, wxT(RAWXML_WAI), wxT("icon"), NULL, useprefix).ToAscii();
    wai.staticshape = ParseString(node, wxT(RAWXML_WAI), wxT("staticshape"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding wai %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_WAI_PARAMETERS)) {
            OPTION_PARSE(float, wai.distance, ParseFloat(child, wxT(RAWXML_WAI_PARAMETERS), wxT("distance")));
            OPTION_PARSE(long, wai.cost, ParseSigned(child, wxT(RAWXML_WAI_PARAMETERS), wxT("cost")));
            OPTION_PARSE(long, wai.refund, ParseSigned(child, wxT(RAWXML_WAI_PARAMETERS), wxT("refund")));
            OPTION_PARSE(float, wai.invdurability, ParseFloat(child, wxT(RAWXML_WAI_PARAMETERS), wxT("inversedurability")));
            OPTION_PARSE(float, wai.x_size, ParseFloat(child, wxT(RAWXML_WAI_PARAMETERS), wxT("xsize")));
            OPTION_PARSE(float, wai.y_size, ParseFloat(child, wxT(RAWXML_WAI_PARAMETERS), wxT("ysize")));
            OPTION_PARSE(float, wai.z_size, ParseFloat(child, wxT(RAWXML_WAI_PARAMETERS), wxT("zsize")));
            OPTION_PARSE(unsigned long, wai.flags, ParseUnsigned(child, wxT(RAWXML_WAI_PARAMETERS), wxT("flags")));
            OPTION_PARSE(float, wai.weight, ParseFloat(child, wxT(RAWXML_WAI_PARAMETERS), wxT("weight")));
        } else if (child(RAWXML_WAI_UNKNOWNS)) {
            OPTION_PARSE(unsigned long, wai.unk1, ParseUnsigned(child, wxT(RAWXML_WAI_UNKNOWNS), wxT("u1")));
            OPTION_PARSE(unsigned long, wai.unk2, ParseUnsigned(child, wxT(RAWXML_WAI_UNKNOWNS), wxT("u2")));
            OPTION_PARSE(unsigned long, wai.unk3, ParseUnsigned(child, wxT(RAWXML_WAI_UNKNOWNS), wxT("u3")));
            OPTION_PARSE(unsigned long, wai.unk4, ParseUnsigned(child, wxT(RAWXML_WAI_UNKNOWNS), wxT("u4")));
            OPTION_PARSE(unsigned long, wai.unk11, ParseUnsigned(child, wxT(RAWXML_WAI_UNKNOWNS), wxT("u11")));
            OPTION_PARSE(float, wai.unk17, ParseFloat(child, wxT(RAWXML_WAI_UNKNOWNS), wxT("u17")));
            OPTION_PARSE(float, wai.unk18, ParseFloat(child, wxT(RAWXML_WAI_UNKNOWNS), wxT("u18")));
            OPTION_PARSE(float, wai.unk19, ParseFloat(child, wxT(RAWXML_WAI_UNKNOWNS), wxT("u19")));
            OPTION_PARSE(float, wai.unk20, ParseFloat(child, wxT(RAWXML_WAI_UNKNOWNS), wxT("u20")));
        } else if (child.element()) {
            throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown tag '%s' in wai tag '%s'."), STRING_FOR_FORMAT(child.name()), name.c_str()), child));
        }

        child.go_next();
    }

    ovlWAIManager* c_wai = m_ovl.GetManager<ovlWAIManager>();
    c_wai->AddItem(wai);
}

