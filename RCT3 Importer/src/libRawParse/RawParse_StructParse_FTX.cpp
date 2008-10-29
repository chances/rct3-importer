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

#include "ManagerFTX.h"

#include "base64.h"
#include "gximage.h"
#include "RCT3Structs.h"

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
            if (c_fts.recolourable == static_cast<unsigned long>(-1))
                c_fts.recolourable = c_fti.recolourable;
            if (c_fts.dimension == 0)
                c_fts.dimension = c_fti.dimension;

            cXmlNode subchild(child.children());
            while (subchild) {
                DO_CONDITION_COMMENT(subchild);

                if (subchild(RAWXML_FTX_FRAME_IMAGE)) {
                    if ((c_fts.palette.get() || c_fts.texture.get()) && (m_mode != MODE_BAKE))
                        throw MakeNodeException<RCT3Exception>(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe/image: multiple datasources"), subchild);

                    bool usealpha = false;
                    OPTION_PARSE(unsigned long, usealpha, ParseUnsigned(subchild, wxT(RAWXML_FTX_FRAME_IMAGE), wxT("usealpha")));

                    wxString t = subchild.wxcontent();
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
                            throw MakeNodeException<RCT3Exception>(_("ftx/ftxframe tag: Error reading image file: ") + texture.GetFullPath(), subchild);

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
                                    throw MakeNodeException<RCT3Exception>(wxString(_("Buffer overflow baking palette '")) + texture.GetFullPath() + _("' in tex tag ") + name, subchild);
                                default:
                                    throw MakeNodeException<RCT3Exception>(wxString(_("Unknown base64 error baking palette '")) + texture.GetFullPath() + _("' in tex tag ") + name, subchild);
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
                                    throw MakeNodeException<RCT3Exception>(wxString(_("Buffer overflow baking texture '")) + texture.GetFullPath() + _("' in tex tag ") + name, subchild);
                                default:
                                    throw MakeNodeException<RCT3Exception>(wxString(_("Unknown base64 error baking texture '")) + texture.GetFullPath() + _("' in tex tag ") + name, subchild);
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
                                        throw MakeNodeException<RCT3Exception>(wxString(_("Buffer overflow baking alpha '")) + texture.GetFullPath() + _("' in tex tag ") + name, subchild);
                                    default:
                                        throw MakeNodeException<RCT3Exception>(wxString(_("Unknown base64 error baking alpha '")) + texture.GetFullPath() + _("' in tex tag ") + name, subchild);
                                }
                                cXmlNode newnode(RAWXML_FTX_FRAME_ADATA);
                                CopyBaseAttributes(subchild, newnode);
                                newnode.content(outbuf.get());
                                subchild = subchild.insertNodeAfter(newnode);
                            }
                            child.prop("dimension", wxString::Format(wxT("%ld"), c_fts.dimension));
                        }

                    } catch (Magick::Exception& e) {
                        throw MakeNodeException<RCT3Exception>(wxString::Format(_("ftx/ftxframe tag: GarphicsMagik exception: %s"), e.what()), subchild);
                    }

                } else if (subchild(RAWXML_FTX_FRAME_ALPHA)) {
                    if (c_fts.alpha.get() && (m_mode != MODE_BAKE))
                        throw MakeNodeException<RCT3Exception>(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe/alpha: multiple datasources"), subchild);

                    wxString t = subchild.wxcontent();
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
                            throw MakeNodeException<RCT3Exception>(_("ftx/ftxframe tag: Error reading image file: ") + texture.GetFullPath(), subchild);

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
                                    throw MakeNodeException<RCT3Exception>(wxString(_("Buffer overflow baking alpha '")) + texture.GetFullPath() + _("' in tex tag ") + name, subchild);
                                default:
                                    throw MakeNodeException<RCT3Exception>(wxString(_("Unknown base64 error baking alpha '")) + texture.GetFullPath() + _("' in tex tag ") + name, subchild);
                            }
                            subchild.name(RAWXML_FTX_FRAME_ADATA);
                            subchild.content(outbuf.get());
                            child.prop("dimension", wxString::Format(wxT("%ld"), c_fts.dimension));
                        }

                    } catch (Magick::Exception& e) {
                        throw MakeNodeException<RCT3Exception>(wxString::Format(_("ftx/ftxframe tag: GarphicsMagik exception: %s"), e.what()), subchild);
                    }
                } else if (subchild(RAWXML_FTX_FRAME_PDATA)) {
                    if (c_fts.palette.get() && (m_mode != MODE_BAKE))
                        throw MakeNodeException<RCT3Exception>(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe/palettedata: multiple datasources"), subchild);

                    BAKE_SKIP(subchild);

                    wxString tex = subchild.wxcontent();
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
                            throw MakeNodeException<RCT3Exception>(wxString(_("Decoding error in ftx/ftxframe/palettedata tag ")) + name, subchild);
                        case CRYPT_BUFFER_OVERFLOW:
                            throw MakeNodeException<RCT3Exception>(wxString(_("Buffer overflow decoding ftx/ftxframe/palettedata tag ")) + name, subchild);
                        default:
                            throw MakeNodeException<RCT3Exception>(wxString(_("Unknown base64 error decoding ftx/ftxframe/palettedata tag ")) + name, subchild);
                    }
                    if (outlen != 256 * sizeof(COLOURQUAD))
                        throw MakeNodeException<RCT3Exception>(wxString::Format(_("Datasize mismatch (%ld/%d) error in ftx('%s')/ftxframe/palettedata tag"), outlen, 256 * sizeof(COLOURQUAD),name.c_str()), subchild);
                } else if (subchild(RAWXML_FTX_FRAME_TDATA)) {
                    if (c_fts.texture.get() && (m_mode != MODE_BAKE))
                        throw MakeNodeException<RCT3Exception>(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe/texturedata: multiple datasources"), subchild);
                    if (m_mode == MODE_BAKE) {
                        subchild.go_next();
                        continue;
                    }

                    if (!c_fts.dimension)
                        throw MakeNodeException<RCT3Exception>(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe: dimension missing for decoding texturedata"), subchild);

                    if (!c_fti.dimension)
                        c_fti.dimension = c_fts.dimension;

                    wxString tex = subchild.wxcontent();
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
                            throw MakeNodeException<RCT3Exception>(wxString(_("Decoding error in ftx/ftxframe/texturedata tag ")) + name, subchild);
                        case CRYPT_BUFFER_OVERFLOW:
                            throw MakeNodeException<RCT3Exception>(wxString(_("Buffer overflow decoding ftx/ftxframe/texturedata tag ")) + name, subchild);
                        default:
                            throw MakeNodeException<RCT3Exception>(wxString(_("Unknown base64 error decoding ftx/ftxframe/texturedata tag ")) + name, subchild);
                    }
                    if (outlen != c_fts.dimension * c_fts.dimension)
                        throw MakeNodeException<RCT3Exception>(wxString(_("Datasize mismatch error in ftx/ftxframe/texturedata tag ")) + name, subchild);
                } else if (subchild(RAWXML_FTX_FRAME_ADATA)) {
                    if (c_fts.alpha.get() && (m_mode != MODE_BAKE))
                        throw MakeNodeException<RCT3Exception>(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe/alphadata: multiple datasources"), subchild);

                    if (m_mode == MODE_BAKE) {
                        subchild.go_next();
                        continue;
                    }

                    if (!c_fts.dimension)
                        throw MakeNodeException<RCT3Exception>(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe: dimension missing for decoding alphadata"), subchild);

                    wxString tex = subchild.wxcontent();
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
                            throw MakeNodeException<RCT3Exception>(wxString(_("Decoding error in ftx/ftxframe/alphadata tag ")) + name, subchild);
                        case CRYPT_BUFFER_OVERFLOW:
                            throw MakeNodeException<RCT3Exception>(wxString(_("Buffer overflow decoding ftx/ftxframe/alphadata tag ")) + name, subchild);
                        default:
                            throw MakeNodeException<RCT3Exception>(wxString(_("Unknown base64 error decoding ftx/ftxframe/alphadata tag ")) + name, subchild);
                    }
                    if (outlen != c_fts.dimension * c_fts.dimension)
                        throw MakeNodeException<RCT3Exception>(wxString(_("Datasize mismatch error in ftx/ftxframe/alphadata tag ")) + name, subchild);
                } else if (subchild.element()) {
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in ftx/ftxframe tag"), subchild.wxname().c_str()), subchild);
                }

                subchild.go_next();
            }

            if (m_mode != MODE_BAKE) {
                if (!c_fts.palette.get())
                    throw MakeNodeException<RCT3Exception>(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe: palette data missing"), child);
                if (!c_fts.texture.get())
                    throw MakeNodeException<RCT3Exception>(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe: texture data missing"), child);
                c_fti.frames.push_back(c_fts);
            }
        } else if (child.element()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in ftx tag"), child.wxname().c_str()), child);
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

