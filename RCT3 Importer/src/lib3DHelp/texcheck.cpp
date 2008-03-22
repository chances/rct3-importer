///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// Texture checker & exception class
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

#include "texcheck.h"

#include <Magick++.h>
#include <algorithm>

#include "confhelp.h"
#include "gximage.h"
#include "lib3Dconfig.h"
#include "RCT3Structs.h"
#include "rct3log.h"

unsigned int checkRCT3Texture(const wxString& texture) {
    wxGXImage img;
    try {
        img.FromFile(texture);
    } catch (Magick::Exception& e) {
        throw RCT3TextureException(wxString::Format(_("File open error: %s"), e.what()));
    }
    if (READ_RCT3_TEXTURE() != RCT3_TEXTURE_ERROR_OUT)
        return std::max(img.GetWidth(), img.GetHeight());
    try {
        if (img.GetWidth() != img.GetHeight())
            throw RCT3TextureException(_("The texture is not square."));
        if (img.GetWidth() > 1024)
            throw RCT3TextureException(_("The texture is too big. RCT3 can only use textures up to 1024x1024."));
        if ((1 << local_log2(img.GetWidth())) != img.GetWidth())
            throw RCT3TextureException(_("The width/height of the texture is not a power of 2."));
    } catch (Magick::Exception& e) {
        throw RCT3TextureException(wxString::Format(_("Image Error during check: %s"), e.what()));
    }
    return img.GetWidth();
}
