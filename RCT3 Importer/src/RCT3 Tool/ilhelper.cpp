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

#include <Magick++.h>

//#include "wxgmagick.h"

#include "wxdlgInfo.h"

#include "global.h"
#include "resource.h"
#include "ilhelper.h"

wxString getGXTypeString(Magick::ImageType type) {
    switch (type) {
        case Magick::UndefinedType:
            return _("Unset value");
        case Magick::BilevelType:
            return _("Monochrome image");
        case Magick::GrayscaleType:
            return _("Grayscale image");
        case Magick::GrayscaleMatteType:
            return _("Grayscale image with opacity (alpha)");
        case Magick::PaletteType:
            return _("Indexed color (palette) image");
        case Magick::PaletteMatteType:
            return _("Indexed color (palette) image with opacity (alpha)");
        case Magick::TrueColorType:
            return _("Truecolor image");
        case Magick::TrueColorMatteType:
            return _("Truecolor image with opacity (alpha)");
        case Magick::ColorSeparationType:
            return _("Cyan/Yellow/Magenta/Black (CYMK) image");
        default:
            return _("Unknown value");
    }
}

wxString getGXColorspaceString(Magick::ColorspaceType type) {
    switch (type) {
        case Magick::UndefinedColorspace:
            return _("Unset value");
        case Magick::RGBColorspace:
            return _("RGB");
        case Magick::GRAYColorspace:
            return _("Grayscale colorspace");
        case Magick::TransparentColorspace:
            return _("Transparent. The Transparent color space behaves uniquely in that it preserves the matte channel of the image if it exists.");
        case Magick::OHTAColorspace:
            return _("OHTA");
        case Magick::XYZColorspace:
            return _("XYZ");
        case Magick::YCbCrColorspace:
            return _("YCbCr");
        case Magick::YCCColorspace:
            return _("YCC");
        case Magick::YIQColorspace:
            return _("YIQ");
        case Magick::YPbPrColorspace:
            return _("YPbPr");
        case Magick::YUVColorspace:
            return _("YUV. Y-signal, U-signal, and V-signal colorspace. YUV is most widely used to encode color for use in television transmission.");
        case Magick::CMYKColorspace:
            return _("CMYK. Cyan-Magenta-Yellow-Black colorspace. CYMK is a subtractive color system used by printers and photographers for the rendering of colors with ink or emulsion, normally on a white surface.");
        case Magick::sRGBColorspace:
            return _("sRGB");
        default:
            return _("Unknown value");
     }
}

void showBitmapInfo(wxWindow *parent, const char *filename) {
    wxString res;

    res.Printf("File: %s\n", filename);
    try {
        Magick::Image img(filename);
        img.colorMapSize(256);
        res += wxString::Format(_("Size: %dx%d\n"), img.columns(), img.rows());
        res += wxString::Format(_("Format: %s (%s)\n"), img.magick().c_str(), img.format().c_str());
        res += _("Image Type: ") + getGXTypeString(img.type()) + wxT("\n");
        res += _("Colorspace: ") + getGXColorspaceString(img.colorSpace()) + wxT("\n");
        res += wxString::Format(_("Colors: %d\n"), img.totalColors());
        res += _("Palette Colorspace: ") + getGXColorspaceString(img.quantizeColorSpace()) + wxT("\n");
        res += wxString::Format(_("Palette Colors: %d\n"), img.quantizeColors());
        res += _("Assigned Palatte Colors: ");
        try {
            res += wxString::Format(_("%d\n"), img.colorMapSize());
        } catch (Magick::Exception e) {
            res += _("(No Palette)\n");
        }
/*
        std::list<Magick::CoderInfo> coderList;
        Magick::coderInfoList( &coderList,           // Reference to output list
                     Magick::CoderInfo::AnyMatch, // Match readable formats
                     Magick::CoderInfo::TrueMatch,  // Don't care about writable formats
                     Magick::CoderInfo::AnyMatch); // Don't care about multi-frame support
        std::list<Magick::CoderInfo>::iterator entry = coderList.begin();
        while( entry != coderList.end() )
        {
            res += wxString(entry->name()) + wxT(": (") + wxString(entry->description()) + wxT(")\n");
            entry ++;
        }
*/
    } catch (Magick::Exception e) {
        res += _("File open error. Message:\n");
        res += e.what();
    }

    dlgInfo *dlg = new dlgInfo(parent);
    dlg->SetText(res);
    dlg->ShowModal();
    dlg->Destroy();
}

