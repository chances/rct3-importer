///////////////////////////////////////////////////////////////////////////////
//
// devIL helper stuff
// Copyright (C) 2006 Tobias Minch, Jonathan Wilson
//
// Part of rct3tool
// Copyright 2005 Jonathan Wilson
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

#include "win_predefine.h"
#include "wx_pch.h"

#include <IL/il.h>
#include <IL/ilu.h>

#include "wxdevil.h"

#include "wxdlgInfo.h"

#include "global.h"
#include "resource.h"
#include "ilhelper.h"

char *il_bgr = "BGR";
char *il_bgra = "BGRA";
char *il_colour_index = "Colour Index";
char *il_luminance = "Luminance";
char *il_luminance_alpha = "Luminance Alpha";
char *il_rgb = "RGB";
char *il_rgba = "RGBA";
char *il_byte = "Byte";
char *il_unsigned_byte = "Unsigned Byte";
char *il_short = "Short";
char *il_unsigned_short = "Unsigned Short";
char *il_int = "Int";
char *il_unsigned_int = "Unsigned Int";
char *il_float = "Float";
char *il_double = "Double";
char *il_pal_none = "None";
char *il_pal_rgb24 = "RGB24";
char *il_pal_rgb32 = "RGB32";
char *il_pal_rgba32 = "RGBA32";
char *il_pal_bgr24 = "BGR24";
char *il_pal_bgr32 = "BGR32";
char *il_pal_bgra32 = "BGRA32";
char *il_origin_lower_left = "Lower Left";
char *il_origin_upper_left = "Upper Left";
char *il_unknown = "Unknown";

char *getILString(ILenum x) {
    switch (x) {
    case IL_COLOUR_INDEX:
        return il_colour_index;
    case IL_RGB:
        return il_rgb;
    case IL_RGBA:
        return il_rgba;
    case IL_BGR:
        return il_bgr;
    case IL_BGRA:
        return il_bgra;
    case IL_LUMINANCE:
        return il_luminance;
    case IL_LUMINANCE_ALPHA:
        return il_luminance_alpha;
    case IL_BYTE:
        return il_byte;
    case IL_UNSIGNED_BYTE:
        return il_unsigned_byte;
    case IL_SHORT:
        return il_short;
    case IL_UNSIGNED_SHORT:
        return il_unsigned_short;
    case IL_INT:
        return il_int;
    case IL_UNSIGNED_INT:
        return il_unsigned_int;
    case IL_FLOAT:
        return il_float;
    case IL_DOUBLE:
        return il_double;
    case IL_PAL_NONE:
        return il_pal_none;
    case IL_PAL_RGB24:
        return il_pal_rgb24;
    case IL_PAL_RGB32:
        return il_pal_rgb32;
    case IL_PAL_RGBA32:
        return il_pal_rgba32;
    case IL_PAL_BGR24:
        return il_pal_bgr24;
    case IL_PAL_BGR32:
        return il_pal_bgr32;
    case IL_PAL_BGRA32:
        return il_pal_bgra32;
    case IL_ORIGIN_LOWER_LEFT:
        return il_origin_lower_left;
    case IL_ORIGIN_UPPER_LEFT:
        return il_origin_upper_left;
    default:
        return il_unknown;
    }
}

void showBitmapInfo(wxWindow *parent, const char *filename) {
    ILuint bmp;
    ILenum Error;
    wxString res;

    res.Printf("File: %s\n", filename);
    {
        wxMutexLocker lock(wxILMutex);
        ILuint old = ilGetInteger(IL_ACTIVE_IMAGE);
        bmp = ilGenImage();
        ilBindImage(bmp);

        if (!ilLoadImage(filename)) {
            res += wxString::Format("File open error. devIL messages:\n");
            while ((Error = ilGetError())) {
                res += wxString::Format("  %s\n", iluErrorString(Error));
            }
        } else {
            ILinfo inf;
            iluGetImageInfo(&inf);
            res += wxString::Format("Size: %dx%d\n", inf.Width, inf.Height);
            res += wxString::Format("Origin: %s\n", getILString(inf.Origin));
            res += wxString::Format("Format: %s\n", getILString(inf.Format));
            res += wxString::Format("Type: %s\n", getILString(inf.Type));
            res += wxString::Format("Bits per Pixel: %d\n", ilGetInteger(IL_IMAGE_BITS_PER_PIXEL));
            res += wxString::Format("Colours: %d\n", iluColoursUsed());
            res += wxString::Format("Palette Type: %s\n", getILString(inf.PalType));
            res += wxString::Format("Palette Bytes per Pixel: %d\n", ilGetInteger(IL_PALETTE_BPP));
            res += wxString::Format("Palette Colours: %d\n", ilGetInteger(IL_PALETTE_NUM_COLS));
        }
        ilDeleteImage(bmp);
        ilBindImage(old);
    }

    dlgInfo *dlg = new dlgInfo(parent);
    dlg->SetText(res);
    dlg->ShowModal();

}

