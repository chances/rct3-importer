/////////////////////////////////////////////////////////////////////////////
// Name:        gximage.h
// Purpose:     wxGXImage class
// Author:      Tobias Minich
// Modified by:
// Created:     2007-11-13
// RCS-ID:
// Copyright:   (c) Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef GXIMAGE_H_INCLUDED
#define GXIMAGE_H_INCLUDED

#include "wx_pch.h"

#include <Magick++.h>

#include "wx/control.h"

#include "wx/icon.h"
#include "wx/bitmap.h"
#include "wx/image.h"

#ifdef USE_SQUISH
#include <squish.h>
enum
{
    wxDXT1       = squish::kDxt1,
    wxDXT3       = squish::kDxt3,
    wxDXT5       = squish::kDxt5
};
#define wxDXT_COMPRESSION_METHOD squish::kColourIterativeClusterFit
#endif

#ifdef CACHE_GXIMAGE
#include <wx/hashmap.h>
class wxGXImageCacheEntry;
WX_DECLARE_STRING_HASH_MAP(wxGXImageCacheEntry*, wxGXImageCache);
#endif

class wxGXImage : public wxObject {
#ifdef CACHE_GXIMAGE
friend class wxGXImageCacheEntry;
#endif
public:
#ifdef CACHE_GXIMAGE
    explicit wxGXImage(bool docache = true) {Init(docache);}
    wxGXImage(const wxString& filename, bool docache = true) {
        Init(docache);
        FromFileSystem(filename);
    }
 #if wxUSE_IMAGE
    wxGXImage(const wxImage& image) {
        Init(false);
        FromImage(image);
    }
    wxGXImage(const wxBitmap& bmp) {
        Init(false);
        FromBitmap(bmp);
    }
 #endif
#else
    wxGXImage() {Init();}
    wxGXImage(const wxString& filename) {
        Init();
        FromFile(filename);
    }
 #if wxUSE_IMAGE
    wxGXImage(const wxImage& image) {
        Init();
        FromImage(image);
    }
    wxGXImage(const wxBitmap& bmp) {
        Init();
        FromBitmap(bmp);
    }
 #endif
#endif

    virtual ~wxGXImage() {
        //ilDeleteImage(m_ilimage);
    }

    void FromFile(const wxString& filename);

#if wxUSE_IMAGE
    void FromImage(const wxImage& image);

    wxImage GetImage(int nw = -1, int nh = -1);

    void FromBitmap( const wxBitmap& bmp ) {
        FromImage(bmp.ConvertToImage());
    }

    // Icon interface for compatibility with wxStaticBitmap.
    void FromIcon( const wxIcon& icon ) {
        wxBitmap bmp;
        bmp.CopyFromIcon( icon );
        FromBitmap( bmp );
    }

    wxIcon GetIcon() {
        wxIcon icon;
        icon.CopyFromBitmap( wxBitmap(GetImage()) );
        return icon;
    }
#endif
    void FromFileSystem(const wxString& filename);

    bool Ok() const {
        return m_valid;
    }

    const wxString& GetLastError() const {
        return m_error;
    }

    const unsigned int GetWidth() const {
        return m_image.columns();
    }
    const unsigned int GetHeight() const {
        return m_image.rows();
    }
    const bool HasAlpha() const {
        return m_image.matte();
    }

    wxGXImage& Rescale(unsigned int width, unsigned int height) {
        m_image.sample(Magick::Geometry(width, height));
        return *this;
    }

    void GetAlpha(unsigned char* data) const;
    void GetGrayscale(unsigned char* data) const;
    void GetAs8bit(unsigned char* data, unsigned char* palette) const;
    void GetAs8bitForced(unsigned char* data, unsigned char* palette, bool special = false) const;

    // GraphicsMagick++ functions
    void flip() {m_image.flip();}
    void flop() {m_image.flop();}
    Magick::ImageType type() {return m_image.type();}

#ifdef USE_SQUISH
    const int GetDxtBufferSize(const int dxt) const {
        return squish::GetStorageRequirements(m_image.columns(), m_image.rows(), dxt);
    }
    const int dxtCompressionMethod() const {
        return m_dxtCompressionMethod;
    }
    void dxtCompressionMethod(int method) {
        m_dxtCompressionMethod = method;
    }
    void DxtCompress(void* buffer, const int dxt);
#endif

#ifdef CACHE_GXIMAGE
    const bool cache() const {
        return m_cache;
    }
    void cache(bool docache) {
        m_cache = docache;
    }

    static void ClearCache();
#endif
protected:
    Magick::Image m_image;
    wxString m_error;
    bool m_valid;
#ifdef USE_SQUISH
    int m_dxtCompressionMethod;
#endif
#ifdef CACHE_GXIMAGE
    bool m_cache;
    static wxGXImageCache g_cache;
#endif

#ifdef CACHE_GXIMAGE
    void Init(bool docache) {
        m_cache = docache;
#else
    void Init() {
#endif
        m_valid = false;
        m_error = wxT("");
#ifdef USE_SQUISH
        m_dxtCompressionMethod = wxDXT_COMPRESSION_METHOD;
#endif
    }
};

#endif // GXIMAGE_H_INCLUDED
