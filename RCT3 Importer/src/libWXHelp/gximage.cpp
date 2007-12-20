/////////////////////////////////////////////////////////////////////////////
// Name:        gximage.cpp
// Purpose:     wxGXImage class
// Author:      Tobias Minich
// Modified by:
// Created:     2007-11-13
// RCS-ID:
// Copyright:   (c) Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx_pch.h"

#include "wx/wxprec.h"
#include "wx/defs.h"

#include <wx/filesys.h>
#include <wx/mstream.h>

#include "gximage.h"

#ifndef RGBQUAD
#include <windows.h>
#endif

Magick::Image* LoadImageFromFileSystem(const wxFSFile* file) {
    Magick::Image* ret = NULL;
    if (file) {
        wxInputStream* filestream = file->GetStream();
        filestream->SeekI(0, wxFromEnd);
        int len = filestream->TellI();
        char* buf = new char[len];
        filestream->SeekI(0);
        wxMemoryOutputStream* buffer = new wxMemoryOutputStream(buf, len);
        buffer->Write(*filestream);

        try {
            Magick::Blob blob;
            blob.updateNoCopy(buf, len);
            ret = new Magick::Image(blob);
        } catch (Magick::Exception e) {
            ret = NULL;
            wxLogDebug(wxT("Error in LoadImageFromFileSystem: %s"), e.what());
            throw;
        }

#ifdef __WXDEBUG__
        if (ret) {
            wxLogDebug(wxT("Trace, LoadImageFromFileSystem, valid, length %d, %dx%d, %s"), len, ret->columns(), ret->rows(),ret->magick().c_str());
        } else {
            wxLogDebug(wxT("Trace, LoadImageFromFileSystem, invalid, length %d"), len);
        }
#endif
        delete buffer;
        //delete[] buf;
        delete filestream;
    } else {
        wxLogDebug(wxT("Trace, LoadImageFromFileSystem, invalid wxFSFile"));
    }
    return ret;
}

#ifdef CACHE_GXIMAGE
#include <wx/filename.h>

class wxGXImageCacheEntry: public wxObject {
protected:
    Magick::Image* m_image;
    wxDateTime m_mtime;
public:
    wxGXImageCacheEntry() {
        m_image = NULL;
    }
    virtual ~wxGXImageCacheEntry() {
        if (m_image)
            delete m_image;
    }
    virtual Magick::Image* Get() = 0;
    bool Valid() {
        return m_image;
    }
};

class wxGXImageCacheEntryFile: public wxGXImageCacheEntry {
protected:
    wxFileName m_file;
public:
    wxGXImageCacheEntryFile(wxString filename):wxGXImageCacheEntry() {
        try {
            m_image = new Magick::Image(filename.fn_str());
            m_file = filename;
            wxLogDebug(wxT("Trace, wxGXImageCacheEntryFile new cache entry '%s'"), filename.c_str());
            if (m_image)
                m_mtime = m_file.GetModificationTime();
        } catch (Magick::Exception e) {
            if (m_image)
                delete m_image;
            m_image = NULL;
            throw;
        }
    }
    Magick::Image* Get() {
        if (m_image) {
            if (m_mtime != m_file.GetModificationTime()) {
                wxString lfile = m_file.GetFullPath();
                wxLogDebug(wxT("Trace, wxGXImageCacheEntryFile MTime change '%s'"), lfile.c_str());
                delete m_image;
                try {
                    m_image = new Magick::Image(lfile.fn_str());
                    if (m_image)
                        m_mtime = m_file.GetModificationTime();
                } catch (Magick::Exception e) {
                    if (m_image)
                        delete m_image;
                    m_image = NULL;
                    throw;
                }
            }
#ifdef __WXDEBUG__
            else
                wxLogDebug(wxT("Trace, wxGXImageCacheEntryFile cache hit '%s'"), m_file.GetFullPath().c_str());
#endif
            return m_image;
        } else {
            // Try again
            try {
                m_image = new Magick::Image(m_file.GetFullPath().fn_str());
                if (m_image)
                    m_mtime = m_file.GetModificationTime();
            } catch (Magick::Exception e) {
                if (m_image)
                    delete m_image;
                m_image = NULL;
                throw;
            }
            return m_image;
        }
    }
};

class wxGXImageCacheEntryFileSystem: public wxGXImageCacheEntry {
protected:
    wxString m_file;
public:
    wxGXImageCacheEntryFileSystem(wxString filename):wxGXImageCacheEntry() {
        try {
            m_file = filename;
            wxLogDebug(wxT("Trace, wxGXImageCacheEntryFileSystem new cache entry '%s'"), m_file.c_str());
            wxFileSystem fs;
            wxFSFile* file = fs.OpenFile(filename, wxFS_READ|wxFS_SEEKABLE);
            if (file) {
                m_image = LoadImageFromFileSystem(file);
                if (m_image)
                    m_mtime = file->GetModificationTime();
            }
        } catch (Magick::Exception e) {
            if (m_image)
                delete m_image;
            m_image = NULL;
            throw;
        }
    }
    Magick::Image* Get() {
        if (m_image) {
            wxFileSystem fs;
            wxFSFile* file = fs.OpenFile(m_file, wxFS_READ|wxFS_SEEKABLE);
            if (file) {
                if (m_mtime != file->GetModificationTime()) {
                    wxLogDebug(wxT("Trace, wxGXImageCacheEntryFileSystem MTime change '%s'"), m_file.c_str());
                    delete m_image;
                    try {
                        m_image = LoadImageFromFileSystem(file);
                        if (m_image)
                            m_mtime = file->GetModificationTime();
                    } catch (Magick::Exception e) {
                        if (m_image)
                            delete m_image;
                        m_image = NULL;
                        throw;
                    }
                }
#ifdef __WXDEBUG__
                else
                    wxLogDebug(wxT("Trace, wxGXImageCacheEntryFileSystem cache hit '%s'"), m_file.c_str());
#endif
            }
#ifdef __WXDEBUG__
            else
                wxLogDebug(wxT("Error, wxGXImageCacheEntryFileSystem mtime failed, used cached version '%s'"), m_file.c_str());
#endif
            return m_image;
        } else {
            // Try again
            try {
                wxFileSystem fs;
                wxFSFile* file = fs.OpenFile(m_file, wxFS_READ|wxFS_SEEKABLE);
                if (file) {
                    m_image = LoadImageFromFileSystem(file);
                    if (m_image)
                        m_mtime = file->GetModificationTime();
                }
            } catch (Magick::Exception e) {
                if (m_image)
                    delete m_image;
                m_image = NULL;
                throw;
            }
            return m_image;
        }
    }
};

wxGXImageCache wxGXImage::g_cache;
#endif

/*
 * wxGXImage
 */
#if wxUSE_IMAGE
wxImage wxGXImage::GetImage(int nw, int nh) {
    wxImage img(wxNullImage);
    if (m_valid) {
        try {
            Magick::Image temp = m_image;
            if (nw < 0)
                nw = temp.columns();
            if (nh < 0)
                nh = temp.rows();
            if ((nw != temp.columns()) || (nh != temp.rows())) {
                temp.sample(Magick::Geometry(nw, nh));
            }

            unsigned char *data = static_cast<unsigned char*> (malloc(nw*nh*3));
            temp.write(0, 0, nw, nh, "RGB", Magick::CharPixel, data);
            img = wxImage(nw, nh);
            img.SetData(data);

            if (temp.matte()) {
                unsigned char *alphadata = static_cast<unsigned char*> (malloc(nw*nh));
                temp.write(0, 0, nw, nh, "A", Magick::CharPixel, alphadata);
                for (int i = 0; i < nw*nh; ++i)
                    alphadata[i] = 255 - alphadata[i];
                img.SetAlpha(alphadata);
            }
        } catch (Magick::Exception e) {
            m_error = e.what();
            img = wxImage(wxNullImage);
            wxLogDebug(wxT("Error in wxGXImage::GetImage: %s"), e.what());
        }
    }
#ifdef __WXDEBUG__
    if (img.HasAlpha()) {
        wxLogDebug(wxT("Trace, wxGXImage::GetImage, alpha, %dx%d"), img.GetWidth(), img.GetHeight());
    } else {
        wxLogDebug(wxT("Trace, wxGXImage::GetImage, no alpha, %dx%d"), img.GetWidth(), img.GetHeight());
    }
#endif
    return img;
}

void wxGXImage::FromImage(const wxImage& image) {
    if (image.Ok()) {
        try {
            m_valid = true;
            m_image.read(image.GetWidth(), image.GetHeight(), "RGB", Magick::CharPixel, image.GetData());
            m_cache = false;
            if (image.HasAlpha()) {
                unsigned char* alpha = image.GetAlpha();
                m_image.matte(true);
                Magick::PixelPacket* pixels = m_image.getPixels(0, 0, m_image.columns(), m_image.rows());
                for (int y = 0; y < m_image.rows(); ++y) {
                    for (int x = 0; x < m_image.columns(); ++x) {
                        pixels[(y*m_image.columns())+x].opacity = 255-alpha[(y*m_image.columns())+x];
                    }
                }
                m_image.syncPixels();
            }
        } catch (Magick::Exception e) {
            m_valid = false;
            m_error = e.what();
            wxLogDebug(wxT("Error in wxGXImage::FromImage: %s"), e.what());
        }
    } else {
        m_valid = false;
        m_image = Magick::Image();
    }
}
#endif //wxUSE_IMAGE

void wxGXImage::FromFile(const wxString& filename) {
    if (wxFileExists(filename)) {
#ifdef CACHE_GXIMAGE
        if (m_cache) {
            wxGXImageCache::iterator it = g_cache.find(filename);
            Magick::Image *im = NULL;
            if (it != g_cache.end()) {
                im = it->second->Get();
            } else {
                wxGXImageCacheEntry* nw = new wxGXImageCacheEntryFile(filename);
                if (nw->Valid()) {
                    g_cache[filename] = nw;
                    im = nw->Get();
                }
            }
            if (im) {
                m_image = *im;
                m_valid = true;
            } else {
                m_image = Magick::Image();
                m_valid = false;
            }
        } else {
            wxLogDebug(wxT("Trace, wxGXImage::FromFile, Uncached '%s'"), filename.fn_str());
#endif
        try {
            m_valid = true;
            m_image.read(filename.fn_str());
        } catch (Magick::Exception e) {
            m_valid = false;
            m_error = e.what();
            wxLogDebug(wxT("Error in wxGXImage::FromFile: %s"), e.what());
        }
#ifdef CACHE_GXIMAGE
        }
#endif
    } else {
        m_valid = false;
    }
}

void wxGXImage::FromFileSystem(const wxString& filename) {
#ifdef CACHE_GXIMAGE
    if (m_cache) {
        wxGXImageCache::iterator it = g_cache.find(filename);
        Magick::Image *im = NULL;
        if (it != g_cache.end()) {
            im = it->second->Get();
        } else {
            wxGXImageCacheEntry* nw = new wxGXImageCacheEntryFileSystem(filename);
            if (nw->Valid()) {
                g_cache[filename] = nw;
                im = nw->Get();
            }
        }
        if (im) {
            m_image = *im;
            m_valid = true;
        } else {
            m_image = Magick::Image();
            m_valid = false;
        }
    } else {
        wxLogDebug(wxT("Trace, wxGXImage::FromFileSystem, Uncached '%s'"), filename.fn_str());
#endif
    wxFileSystem fs;
    wxFSFile* file = fs.OpenFile(filename, wxFS_READ);
    if (file) {
        try {
            Magick::Image *im = LoadImageFromFileSystem(file);
            if (im) {
                m_image = *im;
                m_valid = true;
            } else {
                m_image = Magick::Image();
                m_valid = false;
            }
        } catch (Magick::Exception e) {
            m_valid = false;
            m_error = e.what();
            wxLogDebug(wxT("Error in wxGXImage::FromFile: %s"), e.what());
        }
    } else {
        wxLogDebug(wxT("Trace, wxGXImage::FromFileSystem, invalid wxFSFile"));
        m_valid = false;
    }
#ifdef CACHE_GXIMAGE
    }
#endif
/*
        wxInputStream* filestream = file->GetStream();
        filestream->SeekI(0, wxFromEnd);
        int len = filestream->TellI();
        char* buf = new char[len];
        filestream->SeekI(0);
        wxMemoryOutputStream* buffer = new wxMemoryOutputStream(buf, len);
        buffer->Write(*filestream);

        try {
            m_valid = true;
            Magick::Blob blob;
            blob.updateNoCopy(buf, len);
            m_image.read(blob);
        } catch (Magick::Exception e) {
            m_valid = false;
            m_error = e.what();
            wxLogDebug(wxT("Error in wxGXImage::FromFileSystem: %s"), e.what());
        }

#ifdef __WXDEBUG__
        if (m_valid) {
            wxLogDebug(wxT("Trace, wxGXImage::FromFileSystem, valid, length %d, %dx%d, %s"), len, m_image.columns(), m_image.rows(), m_image.magick().c_str());
            //m_image.write("I:\\test.png");
        } else {
            wxLogDebug(wxT("Trace, wxGXImage::FromFileSystem, invalid, length %d"), len);
        }
#endif
        delete buffer;
        //delete[] buf;
        delete filestream;
*/
}

void wxGXImage::GetAlpha(unsigned char* data) const {
    wxLogDebug(wxT("TRACE wxGXImage::GetAlpha"));
    if (m_image.matte()) {
        const_cast<Magick::Image*>(&m_image)->write(0, 0, m_image.columns(), m_image.rows(), "A", Magick::CharPixel, data);
        for (unsigned int i = 0; i < m_image.columns()*m_image.rows(); ++i)
            data[i] = 255 - data[i];
    } else {
        memset(data, 0, m_image.columns()*m_image.rows());
    }
}

void wxGXImage::GetGrayscale(unsigned char* data) const {
    wxLogDebug(wxT("TRACE wxGXImage::GetGrayscale"));
    Magick::Image temp = m_image;
    temp.type(Magick::GrayscaleType);
    const Magick::PixelPacket* src = temp.getConstPixels(0, 0, m_image.columns(), m_image.rows());

    for (unsigned int r = 0; r < m_image.columns() * m_image.rows(); ++r) {
        data[r] = src[r].red;
    }
}

void wxGXImage::GetAs8bit(unsigned char* data, unsigned char* palette) const {
    wxLogDebug(wxT("TRACE wxGXImage::GetAs8bit"));
    Magick::Image temp = m_image;
    RGBQUAD* pal = reinterpret_cast<RGBQUAD*>(palette);

    if (temp.matte())
        temp.matte(false);

    if ((temp.type() != Magick::PaletteType) || (temp.totalColors() != 256)) {
        temp.quantizeColors(256);
        temp.quantizeDither(false);
        temp.quantize();
        temp.type(Magick::PaletteType);
    }

    for (unsigned int i = 0; i < temp.colorMapSize(); ++i) {
        Magick::Color col = temp.colorMap(i);
        pal[i].rgbRed = col.redQuantum();
        pal[i].rgbGreen = col.greenQuantum();
        pal[i].rgbBlue = col.blueQuantum();
    }

    temp.getConstPixels(0, 0, temp.columns(), temp.rows());
    const Magick::IndexPacket* indices = temp.getConstIndexes();
    memcpy(data, indices, temp.columns() * temp.rows());
}

void wxGXImage::GetAs8bitForced(unsigned char* data, unsigned char* palette, bool special) const {
    wxLogDebug(wxT("TRACE wxGXImage::GetAs8bitForced"));
    RGBQUAD* pal = reinterpret_cast<RGBQUAD*>(palette);
    const Magick::PixelPacket* src = m_image.getConstPixels(0, 0, m_image.columns(), m_image.rows());

    unsigned int paldist, palmax;
    int r, d1, d2, d3;
    unsigned int p;
    for (r = 0; r < m_image.columns() * m_image.rows(); ++r) {
        palmax = UINT_MAX;
        for (p = (special)?1:0; p < 256; p++) {
            d1 = static_cast<int>(src[r].red) - static_cast<int>(pal[p].rgbRed);
            d2 = static_cast<int>(src[r].green) - static_cast<int>(pal[p].rgbGreen);
            d3 = static_cast<int>(src[r].blue) - static_cast<int>(pal[p].rgbBlue);
            paldist = d1 * d1 + d2 * d2 + d3 * d3;
            if (paldist < palmax) {
                palmax = paldist;
                data[r] = p;
            }
        }
    }
}

#ifdef USE_SQUISH
void wxGXImage::DxtCompress(void* buffer, const int dxt) {
    wxLogDebug(wxT("TRACE wxGXImage::DxtCompress"));
    int datasize = 4*m_image.rows()*m_image.columns();
    squish::u8* data = new squish::u8[datasize];
    Magick::Image temp = m_image;
    temp.matte(true);
    temp.write(0, 0, m_image.columns(), m_image.rows(), "RGBA", Magick::CharPixel, data);
    for (int i = 3; i < datasize; i+=4)
        data[i] = 255 - data[i];
#ifdef __WXDEBUG__
    int ofs = (m_image.rows()*m_image.columns()) / 3;
    ofs *= 4;
    wxLogDebug(wxT("Trace, DxtCompress %hhx %hhx %hhx %hhx"), data[ofs+0], data[ofs+1], data[ofs+2], data[ofs+3]);
#endif
    squish::CompressImage(data, m_image.columns(), m_image.rows(), buffer, dxt | m_dxtCompressionMethod);
    delete[] data;
}
#endif

#ifdef CACHE_GXIMAGE
void wxGXImage::ClearCache() {
    for (wxGXImageCache::iterator it = g_cache.begin(); it != g_cache.end(); ++it)
        delete it->second;
    g_cache.clear();
}
#endif
