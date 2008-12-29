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

#include <Magick++.h>

#include <wx/control.h>

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
    explicit wxGXImage(unsigned int width, unsigned int height, bool docache = false) {
        Init(docache);
        m_image = Magick::Image(Magick::Geometry(width, height), Magick::Color(0, 0, 0));
        m_valid = true;
    }

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
    wxGXImage(unsigned int width, unsigned int height) {
        Init();
        m_image = Magick::Image(Magick::Geometry(width, height), Magick::Color(0, 0, 0));
        m_valid = true;
    }
    wxGXImage(const wxString& filename) {
        Init();
        FromFileSystem(filename);
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

    void FromData(int width, int height, const char* channelmap, const void* data);
    void GetData(const char* channelmap, void* data, bool swapalpha = false);
    void FromPaletteData(int width, int height, const void* palette, const void* data);

    bool SaveFile(const wxString& name, int type);
    bool SaveFile(const wxString& name, const wxString& magick);
    bool SaveFile(const wxString& name);

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
    void SetAlpha(const void* data);
    void InvertAlpha();
	void FudgeAlpha(float f);
    void GetGrayscale(unsigned char* data) const;
    void GetAs8bit(unsigned char* data, unsigned char* palette) const;
    void GetAs8bitForced(unsigned char* data, unsigned char* palette, bool special = false) const;

    // GraphicsMagick++ functions
    // FloodFill doesn't work for some reason -.-
    void colorFuzz(double _fuzz) { m_image.colorFuzz(_fuzz); }
    double colorFuzz() const { return m_image.colorFuzz(); }
/*
    void floodFillTexture(unsigned int x, unsigned int y, const wxGXImage& texture) {
        m_image.floodFillTexture(x, y, texture.m_image);
    }
*/
    inline void floodFillOpacity(long x, long y, unsigned int _opacity, Magick::PaintMethod _method = Magick::PointMethod) {
		m_image.floodFillOpacity(x, y, _opacity, _method);
	}
	inline bool matte() const {return m_image.matte();}
	inline void matte(bool _matte) {m_image.matte(_matte);}
	inline void opacity(unsigned int _opacity) {m_image.opacity(_opacity);}
	inline void composite(const Magick::Image& compositeImage_, int xOffset_, int yOffset_, Magick::CompositeOperator compose_ = Magick::InCompositeOp) {
		m_image.composite(compositeImage_, xOffset_, yOffset_, compose_);
	}
	inline void composite(const wxGXImage& compositeImage_, int xOffset_, int yOffset_, Magick::CompositeOperator compose_ = Magick::InCompositeOp) {
		composite(compositeImage_.m_image, xOffset_, yOffset_, compose_);
	}

    wxString magick() {return wxString(m_image.magick().c_str(), wxConvLocal);}
    inline void flip() {m_image.flip();}
    inline void flop() {m_image.flop();}
    void crop(unsigned int x, unsigned int y, unsigned int width, unsigned int height) { crop(wxString::Format("%ux%u+%u+%u", width, height, x, y)); }
    void crop(const char* geometry) {m_image.crop(geometry);}
    void crop(const wxString& geometry) {crop(static_cast<const char*>(geometry.utf8_str()));}
    Magick::ImageType type() {return m_image.type();}
    void read(const Magick::Blob& blob) {
#ifdef CACHE_GXIMAGE
        m_cache = false;
#endif
        m_valid = true;
        try {
            m_image.read(blob);
        } catch(...) {
            m_valid = false;
            throw;
        }
    }
    void read(const Magick::Blob& blob, std::string magick) {
#ifdef CACHE_GXIMAGE
        m_cache = false;
#endif
        m_valid = true;
        try {
            m_image.magick(magick);
            m_image.read(blob);
        } catch(...) {
            m_valid = false;
            throw;
        }
    }
    void read(const void* blob, unsigned long size, std::string magick = "") {
        if (magick == "")
            read(Magick::Blob(blob, size));
        else
            read(Magick::Blob(blob, size), magick);
    }
    void size(unsigned int width, unsigned int height) { size(wxString::Format("%ux%u", width, height)); }
    void size(const char* geometry) {m_image.size(geometry);}
    void size(const wxString& geometry) {size(static_cast<const char*>(geometry.utf8_str()));}
	inline Magick::ImageType type() const {return m_image.type();}
	inline void type(Magick::ImageType _type) {m_image.type(_type);}
    void write(Magick::Blob& blob) {m_image.write(&blob);}
    void write(Magick::Blob& blob, std::string magick) {m_image.write(&blob, magick);}

    // Operators
    wxGXImage& operator+= (const wxGXImage& rhs) {
        m_image.composite(rhs.m_image, (m_image.columns() - rhs.m_image.columns())/2, (m_image.rows() - rhs.m_image.rows())/2, Magick::OverCompositeOp);
        return *this;
    }
    wxGXImage operator+ (const wxGXImage& rhs) const {
        wxGXImage ret = *this;
        ret += rhs;
        return ret;
    }
    wxGXImage& operator*= (const wxGXImage& rhs) {
        for (unsigned int x = 0; x < m_image.columns(); x += rhs.m_image.columns()) {
            for (unsigned int y = 0; y < m_image.rows(); y += rhs.m_image.rows()) {
                m_image.composite(rhs.m_image, x, y, Magick::OverCompositeOp);
            }
        }
        return *this;
    }
    wxGXImage operator* (const wxGXImage& rhs) const {
        wxGXImage ret = *this;
        ret *= rhs;
        return ret;
    }

    static bool CheckGraphicsMagick(const wxString& apppath);

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
    void FromDxtCompressed(int width, int height, const void* buffer, const int dxt);
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
