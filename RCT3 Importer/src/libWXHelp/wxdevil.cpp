/////////////////////////////////////////////////////////////////////////////
// Name:        wxdevil.cpp
// Purpose:     Mutex to make devIL thread safe & related helper functions
// Author:      Tobias Minich
// Modified by:
// Created:     Oct 28 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx_pch.h"

#include "wxdevil.h"

wxMutex wxILMutex(wxMUTEX_DEFAULT);

wxSize getBitmapSize(const char *filename) {
    ILuint bmp;
    ILenum Error;
    wxSize res;

    {
        wxMutexLocker lock(wxILMutex);
        ILuint old = ilGetInteger(IL_ACTIVE_IMAGE);
        bmp = ilGenImage();
        ilBindImage(bmp);

        if (!ilLoadImage(filename)) {
            res = wxDefaultSize;
            // Flush error buffer
            while ((Error = ilGetError())) {}
        } else {
            res = wxSize(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
        }
        ilDeleteImage(bmp);
        ilBindImage(old);
    }
    return res;
}

bool getBitmapInfo(const char *filename, ILinfo& info) {
    ILuint bmp;
    ILenum Error;
    bool res = true;

    {
        wxMutexLocker lock(wxILMutex);
        ILuint old = ilGetInteger(IL_ACTIVE_IMAGE);
        bmp = ilGenImage();
        ilBindImage(bmp);

        if (!ilLoadImage(filename)) {
            // Flush error buffer
            while ((Error = ilGetError())) {}
            res = false;
        } else {
            iluGetImageInfo(&info);
        }
        ilDeleteImage(bmp);
        ilBindImage(old);
    }
    return res;
}

bool ilInfoHasAlpha(const ILinfo& info) {
    switch (info.Format) {
        case IL_COLOR_INDEX: {
            return false; // devIL currently doesn't support fetching alpha from indexed bitmaps
/*
            switch (info.PalType) {
                case IL_PAL_RGBA32:
                case IL_PAL_BGRA32:
                    return true;
                default:
                    return false;
            }
*/
        }
        case IL_RGBA:
        case IL_LUMINANCE_ALPHA:
        case IL_BGRA:
            return true;
        default:
            return false;
    }
}
