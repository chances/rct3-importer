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

#include "ManagerCHG.h"
#include "ManagerCID.h"
#include "ManagerFTX.h"

void cRawParser::ParseCHG(cXmlNode& node) {
    USE_PREFIX(node);
    cChangingRoom room;
    room.name = ParseString(node, wxT(RAWXML_CHG), wxT("name"), NULL, useprefix).ToAscii();
    room.attraction.name = ParseString(node, wxT(RAWXML_CHG), wxT("nametxt"), NULL, useprefix).ToAscii();
    room.attraction.description = ParseString(node, wxT(RAWXML_CHG), wxT("description"), NULL, useprefix).ToAscii();
    room.sid = ParseString(node, wxT(RAWXML_CHG), wxT("sid"), NULL, useprefix).ToAscii();
    room.spline = ParseString(node, wxT(RAWXML_CHG), wxT("roomspline"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding chg %s to %s."), wxString(room.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_ATTRACTION_BASE)) {
            USE_PREFIX(child);
            OPTION_PARSE(unsigned long, room.attraction.type, ParseUnsigned(child, wxT(RAWXML_ATTRACTION_BASE), wxT("type")));
            ParseStringOption(room.attraction.icon, child, wxT("icon"), NULL, useprefix);
            ParseStringOption(room.spline, child, wxT("attractionspline"), NULL, useprefix);
        } else if (child(RAWXML_ATTRACTION_UNKNOWNS)) {
            OPTION_PARSE(unsigned long, room.attraction.unk2, ParseUnsigned(child, wxT(RAWXML_ATTRACTION_UNKNOWNS), wxT("u2")));
            OPTION_PARSE(long, room.attraction.unk3, ParseSigned(child, wxT(RAWXML_ATTRACTION_UNKNOWNS), wxT("u3")));
            OPTION_PARSE(unsigned long, room.attraction.unk4, ParseUnsigned(child, wxT(RAWXML_ATTRACTION_UNKNOWNS), wxT("u4")));
            OPTION_PARSE(unsigned long, room.attraction.unk5, ParseUnsigned(child, wxT(RAWXML_ATTRACTION_UNKNOWNS), wxT("u5")));
            OPTION_PARSE(unsigned long, room.attraction.unk6, ParseUnsigned(child, wxT(RAWXML_ATTRACTION_UNKNOWNS), wxT("u6")));
            OPTION_PARSE(unsigned long, room.attraction.unk9, ParseUnsigned(child, wxT(RAWXML_ATTRACTION_UNKNOWNS), wxT("u9")));
            OPTION_PARSE(long, room.attraction.unk10, ParseSigned(child, wxT(RAWXML_ATTRACTION_UNKNOWNS), wxT("u10")));
            OPTION_PARSE(unsigned long, room.attraction.addonascn, ParseUnsigned(child, wxT(RAWXML_ATTRACTION_UNKNOWNS), wxT("u11")));
            OPTION_PARSE(unsigned long, room.attraction.unk12, ParseUnsigned(child, wxT(RAWXML_ATTRACTION_UNKNOWNS), wxT("u12")));
            OPTION_PARSE(unsigned long, room.attraction.unk13, ParseUnsigned(child, wxT(RAWXML_ATTRACTION_UNKNOWNS), wxT("u13")));
        } else if (child.element()) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in sta tag."), STRING_FOR_FORMAT(child.name())));
        }

        child.go_next();
    }

    ovlCHGManager* c_chg = m_ovl.GetManager<ovlCHGManager>();
    c_chg->AddRoom(room);
}

void cRawParser::ParseCID(cXmlNode& node) {
    USE_PREFIX(node);
    cCarriedItem cid;
    cid.name = ParseString(node, wxT(RAWXML_CID), wxT("name"), NULL, useprefix).ToAscii();
    cid.nametxt = ParseString(node, wxT(RAWXML_CID), wxT("nametxt"), NULL, useprefix).ToAscii();
    cid.pluralnametxt = ParseString(node, wxT(RAWXML_CID), wxT("pluralnametxt"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding cid %s to %s."), wxString(cid.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_CID_SHAPE)) {
            USE_PREFIX(child);
            cid.shape.shaped = ParseUnsigned(child, wxT(RAWXML_CID_SHAPE), wxT("shaped"));
            if (cid.shape.shaped) {
                cid.shape.shape1 = ParseString(child, wxT(RAWXML_CID_SHAPE), wxT("shape1"), NULL, useprefix).ToAscii();
                cid.shape.shape2 = ParseString(child, wxT(RAWXML_CID_SHAPE), wxT("shape1"), NULL, useprefix).ToAscii();
            } else {
                cid.shape.MakeBillboard();
                cid.shape.fts = ParseString(child, wxT(RAWXML_CID_SHAPE), wxT("ftx"), NULL, useprefix).ToAscii();
            }
            OPTION_PARSE(float, cid.shape.unk9, ParseFloat(child, wxT(RAWXML_CID_SHAPE), wxT("distance")));
            OPTION_PARSE(unsigned long, cid.shape.defaultcolour, ParseUnsigned(child, wxT(RAWXML_CID_SHAPE), wxT("defaultcolour")));
            OPTION_PARSE(float, cid.shape.scalex, ParseFloat(child, wxT(RAWXML_CID_SHAPE), wxT("scalex")));
            OPTION_PARSE(float, cid.shape.scaley, ParseFloat(child, wxT(RAWXML_CID_SHAPE), wxT("scaley")));
        } else if (child(RAWXML_CID_MORE)) {
            USE_PREFIX(child);
            OPTION_PARSE(unsigned long, cid.addonpack, ParseUnsigned(child, wxT(RAWXML_CID_MORE), wxT("addonpack")));
            ParseStringOption(cid.icon, child, wxT("icon"), NULL, useprefix);
        } else if (child(RAWXML_CID_EXTRA)) {
            USE_PREFIX(child);
            wxString extra = ParseString(child, wxT(RAWXML_CID_EXTRA), wxT("name"), NULL, useprefix);
            cid.extras.push_back(std::string(extra.ToAscii()));
        } else if (child(RAWXML_CID_SETTINGS)) {
            OPTION_PARSE(unsigned long, cid.settings.flags, ParseUnsigned(child, wxT(RAWXML_CID_SETTINGS), wxT("flags")));
            OPTION_PARSE(long, cid.settings.ageclass, ParseSigned(child, wxT(RAWXML_CID_SETTINGS), wxT("ageclass")));
            OPTION_PARSE(unsigned long, cid.settings.type, ParseUnsigned(child, wxT(RAWXML_CID_SETTINGS), wxT("type")));
            OPTION_PARSE(float, cid.settings.hunger, ParseFloat(child, wxT(RAWXML_CID_SETTINGS), wxT("hunger")));
            OPTION_PARSE(float, cid.settings.thirst, ParseFloat(child, wxT(RAWXML_CID_SETTINGS), wxT("thirst")));
            OPTION_PARSE(float, cid.settings.lightprotectionfactor, ParseFloat(child, wxT(RAWXML_CID_SETTINGS), wxT("lightprotectionfactor")));
        } else if (child(RAWXML_CID_TRASH)) {
            USE_PREFIX(child);
            ParseStringOption(cid.trash.wrapper, child, wxT("wrapper"), NULL, useprefix);
            OPTION_PARSE(float, cid.trash.trash1, ParseFloat(child, wxT(RAWXML_CID_TRASH), wxT("trash1")));
            OPTION_PARSE(long, cid.trash.trash2, ParseSigned(child, wxT(RAWXML_CID_TRASH), wxT("trash2")));
        } else if (child(RAWXML_CID_SOAKED)) {
            ParseStringOption(cid.soaked.male_morphmesh_body, child, wxT("malebody"), NULL, false);
            ParseStringOption(cid.soaked.male_morphmesh_legs, child, wxT("malelegs"), NULL, false);
            ParseStringOption(cid.soaked.female_morphmesh_body, child, wxT("femalebody"), NULL, false);
            ParseStringOption(cid.soaked.female_morphmesh_legs, child, wxT("femalelegs"), NULL, false);
            OPTION_PARSE(unsigned long, cid.soaked.unk38, ParseUnsigned(child, wxT(RAWXML_CID_SOAKED), wxT("unknown")));
            ParseStringOption(cid.soaked.textureoption, child, wxT("textureoption"), NULL, false);
        } else if (child(RAWXML_CID_SIZEUNK)) {
            OPTION_PARSE(unsigned long, cid.size.unk17, ParseUnsigned(child, wxT(RAWXML_CID_SIZEUNK), wxT("u1")));
            OPTION_PARSE(unsigned long, cid.size.unk18, ParseUnsigned(child, wxT(RAWXML_CID_SIZEUNK), wxT("u2")));
            OPTION_PARSE(unsigned long, cid.size.unk19, ParseUnsigned(child, wxT(RAWXML_CID_SIZEUNK), wxT("u3")));
            OPTION_PARSE(unsigned long, cid.size.unk20, ParseUnsigned(child, wxT(RAWXML_CID_SIZEUNK), wxT("u4")));
        } else if (child(RAWXML_CID_UNK)) {
            OPTION_PARSE(unsigned long, cid.unknowns.unk1, ParseUnsigned(child, wxT(RAWXML_CID_UNK), wxT("u1")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk3, ParseUnsigned(child, wxT(RAWXML_CID_UNK), wxT("u3")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk26, ParseUnsigned(child, wxT(RAWXML_CID_UNK), wxT("u26")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk27, ParseUnsigned(child, wxT(RAWXML_CID_UNK), wxT("u27")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk28, ParseUnsigned(child, wxT(RAWXML_CID_UNK), wxT("u28")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk33, ParseUnsigned(child, wxT(RAWXML_CID_UNK), wxT("u33")));
        } else if (child.element()) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in cid tag."), STRING_FOR_FORMAT(child.name())));
        }
        child.go_next();
    }

    ovlCIDManager* c_cid = m_ovl.GetManager<ovlCIDManager>();
    c_cid->AddItem(cid);
}

void cRawParser::ParseFTX(cXmlNode& node) {
    USE_PREFIX(node);
    cFlexiTextureInfoStruct c_fti;

    wxString name = ParseString(node, wxT(RAWXML_FTX), wxT("name"), NULL, useprefix);
    c_fti.name = name.ToAscii();
    OPTION_PARSE(unsigned long, c_fti.dimension, ParseUnsigned(node, wxT(RAWXML_FTX), wxT("dimension")));
    OPTION_PARSE(unsigned long, c_fti.recolourable, ParseUnsigned(node, wxT(RAWXML_FTX), wxT("recolourable")));
    OPTION_PARSE(unsigned long, c_fti.fps, ParseUnsigned(node, wxT(RAWXML_FTX), wxT("fps")));
//    wxFileName texture = ParseString(node, RAWXML_TEX, wxT("texture"));
//    if (!texture.IsAbsolute())
//        texture.MakeAbsolute(m_input.GetPathWithSep());
    wxLogVerbose(wxString::Format(_("Adding ftx %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));
    counted_array_ptr<unsigned char> data;

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_FTX_ANIMATION)) {
            if (m_mode == MODE_BAKE) {
                child.go_next();
                continue;
            }
            unsigned long frame = ParseUnsigned(child, wxT(RAWXML_FTX_ANIMATION), wxT("index"));
            unsigned long rep = 1;
            OPTION_PARSE(unsigned long, rep, ParseUnsigned(child, wxT(RAWXML_FTX_ANIMATION), wxT("repeat")));
            for (unsigned long i = 0; i < rep; ++i)
                c_fti.animation.push_back(frame);
        } else if (child(RAWXML_FTX_FRAME)) {
            cFlexiTextureStruct c_fts;
            c_fts.recolourable = -1;
            OPTION_PARSE(unsigned long, c_fts.dimension, ParseUnsigned(child, wxT(RAWXML_FTX_FRAME), wxT("dimension")));
            OPTION_PARSE(unsigned long, c_fts.recolourable, ParseUnsigned(child, wxT(RAWXML_FTX_FRAME), wxT("recolourable")));
            unsigned char alphacutoff = 0;
            OPTION_PARSE(unsigned long, alphacutoff, ParseUnsigned(child, wxT(RAWXML_FTX_FRAME), wxT("alphacutoff")));
            if (c_fts.recolourable == -1)
                c_fts.recolourable = c_fti.recolourable;
            if (c_fts.dimension == 0)
                c_fts.dimension = c_fti.dimension;

            cXmlNode subchild(child.children());
            while (subchild) {
                DO_CONDITION_COMMENT(subchild);

                if (subchild(RAWXML_FTX_FRAME_IMAGE)) {
                    if ((c_fts.palette.get() || c_fts.texture.get()) && (m_mode != MODE_BAKE))
                        throw RCT3Exception(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe/image: multiple datasources."));

                    bool usealpha = false;
                    OPTION_PARSE(unsigned long, usealpha, ParseUnsigned(subchild, wxT(RAWXML_FTX_FRAME_IMAGE), wxT("usealpha")));

                    wxString t = UTF8STRINGWRAP(subchild.content());
                    bool filevar = MakeVariable(t);
                    wxFSFileName texture = t;
                     if (m_mode == MODE_BAKE) {
                        if (m_bake.Index(wxT(RAWXML_FTX)) == wxNOT_FOUND) {
                            if (!filevar) {
                                if (m_bake.Index(wxT(RAWBAKE_ABSOLUTE)) == wxNOT_FOUND) {
                                    texture.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                                }
                                subchild.content(texture.GetFullPath().mb_str(wxConvUTF8));
                            }
                            subchild.go_next();
                            continue;
                        }
                    }
//                    if (!texture.IsFileReadable())
//                        throw RCT3Exception(_("ftx/ftxframe tag: File not readable: ") + texture.GetFullPath());
                    try {
                        wxGXImage img(texture.GetFullPath(), false);
                        if (!img.Ok())
                            throw RCT3Exception(_("ftx/ftxframe tag: Error reading image file: ") + texture.GetFullPath());

                        if (c_fts.dimension) {
                            if ((img.GetWidth() != c_fts.dimension) || (img.GetHeight() != c_fts.dimension)) {
                                img.Rescale(c_fts.dimension, c_fts.dimension);
                            }
                        } else {
                            int width = img.GetWidth();
                            int height = img.GetHeight();
                            if (width != height) {
                                throw Magick::Exception("No dimension set and texture is not square");
                            }
                            if ((1 << local_log2(width)) != width) {
                                throw Magick::Exception("Texture's width/height is not a power of 2");
                            }
                            c_fts.dimension = width;
                        }

                        if (!c_fti.dimension)
                            c_fti.dimension = c_fts.dimension;

                        img.flip();
                        c_fts.palette = counted_array_ptr<unsigned char>(new unsigned char[256 * sizeof(COLOURQUAD)]);
                        memset(c_fts.palette.get(), 0, 256 * sizeof(COLOURQUAD));
                        c_fts.texture = counted_array_ptr<unsigned char>(new unsigned char[c_fts.dimension * c_fts.dimension]);
                        if (usealpha)
                            c_fts.alpha = counted_array_ptr<unsigned char>(new unsigned char[c_fts.dimension * c_fts.dimension]);

                        if (c_fts.recolourable && (img.type() != Magick::PaletteType)) {
                            memcpy(c_fts.palette.get(), cFlexiTexture::GetRGBPalette(), 256 * sizeof(COLOURQUAD));
                            img.GetAs8bitForced(c_fts.texture.get(), c_fts.palette.get(), true);
                        } else {
                            img.GetAs8bit(c_fts.texture.get(), c_fts.palette.get());
                        }

                        if (usealpha) {
                            img.GetAlpha(c_fts.alpha.get());
                        }

                        for (unsigned int j = 0; j < 256; j++)
                            reinterpret_cast<COLOURQUAD*>(c_fts.palette.get())[j].alpha = alphacutoff;

                        if (m_mode == MODE_BAKE) {
                            unsigned long outsize = 4 * ((c_fts.dimension * c_fts.dimension > 256 * sizeof(COLOURQUAD))?c_fts.dimension * c_fts.dimension:256 * sizeof(COLOURQUAD));
                            unsigned long outlen = outsize;
                            counted_array_ptr<char> outbuf(new char[outsize]) ;
                            int bret = base64_encode(c_fts.palette.get(), 256 * sizeof(COLOURQUAD), reinterpret_cast<unsigned char*>(outbuf.get()), &outlen);
                            switch (bret) {
                                case CRYPT_OK:
                                    break;
                                case CRYPT_BUFFER_OVERFLOW:
                                    throw RCT3Exception(wxString(_("Buffer overflow baking palette '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                                default:
                                    throw RCT3Exception(wxString(_("Unknown base64 error baking palette '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                            }
                            subchild.name(RAWXML_FTX_FRAME_PDATA);
                            subchild.delProp("usealpha");
                            //delete subchild->GetChildren();
                            subchild.content(outbuf.get());

                            outlen = outsize;
                            bret = base64_encode(c_fts.texture.get(), c_fts.dimension * c_fts.dimension, reinterpret_cast<unsigned char*>(outbuf.get()), &outlen);
                            switch (bret) {
                                case CRYPT_OK:
                                    break;
                                case CRYPT_BUFFER_OVERFLOW:
                                    throw RCT3Exception(wxString(_("Buffer overflow baking texture '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                                default:
                                    throw RCT3Exception(wxString(_("Unknown base64 error baking texture '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                            }
                            cXmlNode newnode(RAWXML_FTX_FRAME_TDATA);
                            CopyBaseAttributes(subchild, newnode);
                            newnode.content(outbuf.get());
                            subchild = subchild.insertNodeAfter(newnode);

                            if (usealpha) {
                                outlen = outsize;
                                bret = base64_encode(c_fts.alpha.get(), c_fts.dimension * c_fts.dimension, reinterpret_cast<unsigned char*>(outbuf.get()), &outlen);
                                switch (bret) {
                                    case CRYPT_OK:
                                        break;
                                    case CRYPT_BUFFER_OVERFLOW:
                                        throw RCT3Exception(wxString(_("Buffer overflow baking alpha '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                                    default:
                                        throw RCT3Exception(wxString(_("Unknown base64 error baking alpha '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                                }
                                cXmlNode newnode(RAWXML_FTX_FRAME_ADATA);
                                CopyBaseAttributes(subchild, newnode);
                                newnode.content(outbuf.get());
                                subchild = subchild.insertNodeAfter(newnode);
                            }
                            child.prop("dimension", wxString::Format(wxT("%ld"), c_fts.dimension));
                        }

                    } catch (Magick::Exception& e) {
                        throw RCT3Exception(wxString::Format(_("ftx/ftxframe tag: GarphicsMagik exception: %s"), e.what()));
                    }

                } else if (subchild(RAWXML_FTX_FRAME_ALPHA)) {
                    if (c_fts.alpha.get() && (m_mode != MODE_BAKE))
                        throw RCT3Exception(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe/alpha: multiple datasources."));

                    wxString t = UTF8STRINGWRAP(subchild.content());
                    bool filevar = MakeVariable(t);
                    wxFSFileName texture = t;
                    if (!texture.IsAbsolute())
                        texture.MakeAbsolute(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                    if (m_mode == MODE_BAKE) {
                        if (m_bake.Index(wxT(RAWXML_FTX)) == wxNOT_FOUND) {
                            if (!filevar) {
                                if (m_bake.Index(wxT(RAWBAKE_ABSOLUTE)) == wxNOT_FOUND) {
                                    texture.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                                }
                                subchild.content(texture.GetFullPath().mb_str(wxConvUTF8));
                            }
                            subchild.go_next();
                            continue;
                        }
                    }
//                    if (!texture.IsFileReadable())
//                        throw RCT3Exception(_("ftx/ftxframe tag: File not readable: ") + texture.GetFullPath());

                    try {
                        wxGXImage img(texture.GetFullPath(), false);
                        if (!img.Ok())
                            throw RCT3Exception(_("ftx/ftxframe tag: Error reading image file: ") + texture.GetFullPath());

                        if (c_fts.dimension) {
                            if ((img.GetWidth() != c_fts.dimension) || (img.GetHeight() != c_fts.dimension)) {
                                img.Rescale(c_fts.dimension, c_fts.dimension);
                            }
                        } else {
                            int width = img.GetWidth();
                            int height = img.GetHeight();
                            if (width != height) {
                                throw Magick::Exception("No dimension set and texture is not square");
                            }
                            if ((1 << local_log2(width)) != width) {
                                throw Magick::Exception("Texture's width/height is not a power of 2");
                            }
                            c_fts.dimension = width;
                        }

                        if (!c_fti.dimension)
                            c_fti.dimension = c_fts.dimension;

                        img.flip();
                        c_fts.alpha = counted_array_ptr<unsigned char>(new unsigned char[c_fts.dimension * c_fts.dimension]);
                        img.GetGrayscale(c_fts.alpha.get());

                        if (m_mode == MODE_BAKE) {
                            unsigned long outsize = 4 * c_fts.dimension * c_fts.dimension;
                            unsigned long outlen = outsize;
                            counted_array_ptr<char> outbuf(new char[outsize]) ;
                            int bret = base64_encode(c_fts.alpha.get(), c_fts.dimension * c_fts.dimension, reinterpret_cast<unsigned char*>(outbuf.get()), &outlen);
                            switch (bret) {
                                case CRYPT_OK:
                                    break;
                                case CRYPT_BUFFER_OVERFLOW:
                                    throw RCT3Exception(wxString(_("Buffer overflow baking alpha '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                                default:
                                    throw RCT3Exception(wxString(_("Unknown base64 error baking alpha '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                            }
                            subchild.name(RAWXML_FTX_FRAME_ADATA);
                            subchild.content(outbuf.get());
                            child.prop("dimension", wxString::Format(wxT("%ld"), c_fts.dimension));
                        }

                    } catch (Magick::Exception& e) {
                        throw RCT3Exception(wxString::Format(_("ftx/ftxframe tag: GarphicsMagik exception: %s"), e.what()));
                    }
                } else if (subchild(RAWXML_FTX_FRAME_PDATA)) {
                    if (c_fts.palette.get() && (m_mode != MODE_BAKE))
                        throw RCT3Exception(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe/palettedata: multiple datasources."));

                    BAKE_SKIP(subchild);

                    wxString tex = UTF8STRINGWRAP(subchild.content());
                    MakeVariable(tex);
                    c_fts.palette = counted_array_ptr<unsigned char>(new unsigned char[256 * sizeof(COLOURQUAD)]);
                    unsigned long outlen = 256 * sizeof(COLOURQUAD);
#ifdef UNICODE
                    int bret = base64_decode(reinterpret_cast<const unsigned char*>(tex.ToAscii().data()), tex.Length(), c_fts.palette.get(), &outlen);
#else
                    int bret = base64_decode(reinterpret_cast<const unsigned char*>(tex.ToAscii()), tex.Length(), c_fts.palette.get(), &outlen);
#endif
                    switch (bret) {
                        case CRYPT_OK:
                            break;
                        case CRYPT_INVALID_PACKET:
                            throw RCT3Exception(wxString(_("Decoding error in ftx/ftxframe/palettedata tag ")) + name);
                        case CRYPT_BUFFER_OVERFLOW:
                            throw RCT3Exception(wxString(_("Buffer overflow decoding ftx/ftxframe/palettedata tag ")) + name);
                        default:
                            throw RCT3Exception(wxString(_("Unknown base64 error decoding ftx/ftxframe/palettedata tag ")) + name);
                    }
                    if (outlen != 256 * sizeof(COLOURQUAD))
                        throw RCT3Exception(wxString::Format(_("Datasize mismatch (%ld/%d) error in ftx('%s')/ftxframe/palettedata tag."), outlen, 256 * sizeof(COLOURQUAD),name.c_str()));
                } else if (subchild(RAWXML_FTX_FRAME_TDATA)) {
                    if (c_fts.texture.get() && (m_mode != MODE_BAKE))
                        throw RCT3Exception(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe/texturedata: multiple datasources."));
                    if (m_mode == MODE_BAKE) {
                        subchild.go_next();
                        continue;
                    }

                    if (!c_fts.dimension)
                        throw RCT3Exception(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe: dimension missing for decoding texturedata."));

                    if (!c_fti.dimension)
                        c_fti.dimension = c_fts.dimension;

                    wxString tex = UTF8STRINGWRAP(subchild.content());
                    MakeVariable(tex);
                    c_fts.texture = counted_array_ptr<unsigned char>(new unsigned char[c_fts.dimension * c_fts.dimension]);
                    unsigned long outlen = c_fts.dimension * c_fts.dimension;
#ifdef UNICODE
                    int bret = base64_decode(reinterpret_cast<const unsigned char*>(tex.ToAscii().data()), tex.Length(), c_fts.texture.get(), &outlen);
#else
                    int bret = base64_decode(reinterpret_cast<const unsigned char*>(tex.ToAscii()), tex.Length(), c_fts.texture.get(), &outlen);
#endif
                    switch (bret) {
                        case CRYPT_OK:
                            break;
                        case CRYPT_INVALID_PACKET:
                            throw RCT3Exception(wxString(_("Decoding error in ftx/ftxframe/texturedata tag ")) + name);
                        case CRYPT_BUFFER_OVERFLOW:
                            throw RCT3Exception(wxString(_("Buffer overflow decoding ftx/ftxframe/texturedata tag ")) + name);
                        default:
                            throw RCT3Exception(wxString(_("Unknown base64 error decoding ftx/ftxframe/texturedata tag ")) + name);
                    }
                    if (outlen != c_fts.dimension * c_fts.dimension)
                        throw RCT3Exception(wxString(_("Datasize mismatch error in ftx/ftxframe/texturedata tag ")) + name);
                } else if (subchild(RAWXML_FTX_FRAME_ADATA)) {
                    if (c_fts.alpha.get() && (m_mode != MODE_BAKE))
                        throw RCT3Exception(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe/alphadata: multiple datasources."));

                    if (m_mode == MODE_BAKE) {
                        subchild.go_next();
                        continue;
                    }

                    if (!c_fts.dimension)
                        throw RCT3Exception(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe: dimension missing for decoding alphadata."));

                    wxString tex = UTF8STRINGWRAP(subchild.content());
                    MakeVariable(tex);
                    c_fts.alpha = counted_array_ptr<unsigned char>(new unsigned char[c_fts.dimension * c_fts.dimension]);
                    unsigned long outlen = c_fts.dimension * c_fts.dimension;
#ifdef UNICODE
                    int bret = base64_decode(reinterpret_cast<const unsigned char*>(tex.ToAscii().data()), tex.Length(), c_fts.alpha.get(), &outlen);
#else
                    int bret = base64_decode(reinterpret_cast<const unsigned char*>(tex.ToAscii()), tex.Length(), c_fts.alpha.get(), &outlen);
#endif
                    switch (bret) {
                        case CRYPT_OK:
                            break;
                        case CRYPT_INVALID_PACKET:
                            throw RCT3Exception(wxString(_("Decoding error in ftx/ftxframe/alphadata tag ")) + name);
                        case CRYPT_BUFFER_OVERFLOW:
                            throw RCT3Exception(wxString(_("Buffer overflow decoding ftx/ftxframe/alphadata tag ")) + name);
                        default:
                            throw RCT3Exception(wxString(_("Unknown base64 error decoding ftx/ftxframe/alphadata tag ")) + name);
                    }
                    if (outlen != c_fts.dimension * c_fts.dimension)
                        throw RCT3Exception(wxString(_("Datasize mismatch error in ftx/ftxframe/alphadata tag ")) + name);
                } else if (subchild.element()) {
                    throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ftx/ftxframe tag."), STRING_FOR_FORMAT(child.name())));
                }

                subchild.go_next();
            }

            if (m_mode != MODE_BAKE) {
                if (!c_fts.palette.get())
                    throw RCT3Exception(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe: palette data missing."));
                if (!c_fts.texture.get())
                    throw RCT3Exception(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe: texture data missing."));
                c_fti.frames.push_back(c_fts);
            }
        } else if (child.element()) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ftx tag."), STRING_FOR_FORMAT(child.name())));
        }
        child.go_next();

    }

    if (m_mode != MODE_BAKE) {
        if (!c_fti.animation.size())
            c_fti.animation.push_back(0);

        ovlFTXManager* c_ftx = m_ovl.GetManager<ovlFTXManager>();
        c_ftx->AddTexture(c_fti);
    }
}

