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

//#include <boost/algorithm/string.hpp>
//#include <boost/tokenizer.hpp>

#include "ManagerTEX.h"

#include "texcheck.h"

#define RAWXML_TEX_TEXTURE          "texture"

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
                throw MakeNodeException<RCT3Exception>(wxString::Format(_("Tag tex(%s)/texture misses data."), name.c_str()), child);

            cRawDatablock data = GetDataBlock(wxString::Format(_("tag tex(%s)/texture"), name.c_str()), datanode);
            if (data.datatype().IsEmpty()) {
                throw MakeNodeException<RCT3Exception>(wxString::Format(_("Could not determine data type in tag tex(%s)/texture."), name.c_str()), child);
            } else if (data.datatype().IsSameAs(wxT("processed"))) {
                unsigned long datadim = cTexture::GetDimension(texture.texture.format, data.datasize());
                if (!mip.dimension)
                    mip.dimension = datadim;
                if (mip.dimension != datadim)
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Dimension mismatch in tag tex(%s)/texture."), name.c_str()), child);
                mip.data = data.data();
            } else {
                wxGXImage img;
                img.read(data.data().get(), data.datasize(), std::string(data.datatype().mb_str(wxConvLocal)));
                if (!img.Ok())
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Error decoding image in tag tex(%s)/texture."), name.c_str()), child);

                img.flip();
                if (!mainimage.Ok())
                    mainimage = img;
                if (mip.dimension) {
                    img.Rescale(mip.dimension, mip.dimension);
                } else {
                    mip.dimension = img.GetWidth();
                }
                try {
                    checkRCT3Texture(img);
                } catch (RCT3TextureException& e) {
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Image in tag tex(%s)/texture is not fit for RCT3: "), name.c_str())+e.wxwhat(), child);
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
                        throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown format 0x%02lx in tag tex(%s)/texture."), texture.texture.format, name.c_str()), child);
                }
            }

            texture.texture.mips.insert(mip);
        } else if (child.element()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in tex(%s) tag."), child.wxname().c_str(), name.c_str()), child);
        }
        child.go_next();
    }

    if (mips) {
        if ((texture.texture.mips.size() > 1) && (mips != texture.texture.mips.size()))
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Mip count mismatch in tag tex(%s)."), name.c_str()), node);
        if ((texture.texture.mips.size() == 1) && (mips > 1)) {
            if (!mainimage.Ok())
                throw MakeNodeException<RCT3Exception>(wxString::Format(_("Cannot auto-generate mips in tag tex(%s)."), name.c_str()), node);
            for (unsigned long i = 1; i < mips; ++i) {
                mainimage.Rescale(mainimage.GetWidth() / 2, mainimage.GetHeight() / 2);
                if (mainimage.GetWidth() < 4) {
                    IssueNodeWarning(wxString::Format(_("Too many mips requested in tag tex(%s)."), name.c_str()), node);
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
                        throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown format 0x%02lx in tag tex(%s)."), texture.texture.format, name.c_str()), node);
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
