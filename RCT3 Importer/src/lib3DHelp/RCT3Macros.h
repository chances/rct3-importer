///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// Helper Macros
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

#ifndef RCT3MACROS_H_INCLUDED
#define RCT3MACROS_H_INCLUDED

#define COMPILER_WRONGTAG(c) (!c->GetName().IsEmpty() && !c->GetName().IsSameAs(wxT("comment")))

#define DEF_RCT3_VECIT(cl) \
public: \
    typedef std::vector<cl>::iterator iterator; \
    typedef std::vector<cl>::const_iterator const_iterator; \
    typedef std::vector<cl> vec;

#define DEF_RCT3_RAW(ty, cl) \
private: \
    ty m_raw; \
public: \
	const ty& raw() const { \
	    return m_raw; \
	}; \
	void raw(const ty& value) { \
        m_raw = value; \
	}; \
	explicit cl(const ty& value) { \
        Init(); \
        raw(value); \
	};

#define DEF_RCT3_PROPERTY(ty, val, pr) \
public: \
	const ty& pr() const { \
	    return m_raw.val; \
	}; \
	void pr(const ty& value) { \
        m_raw.val = value; \
	};

#define DEF_RCT3_PROPERTY_NC(ty, val, pr) \
public: \
	ty& pr ## _nc(bool dirty = false) { \
	    return m_raw.val; \
	}; \

#define DEF_RCT3_PROPERTY_F(ty, val, pr) \
public: \
	void pr ## _read(FILE* f) { \
        fread(&m_raw.val, sizeof(ty), 1, f); \
	}; \
	void pr ## _write(FILE* f) { \
        fwrite(&m_raw.val, sizeof(ty), 1, f); \
	};

#define DEF_RCT3_D_PROPERTY(ty, val, pr) \
public: \
	const ty& pr() const { \
	    return val; \
	}; \
	void pr(const ty& value) { \
        val = value; \
	};

#define DEF_RCT3_D_PROPERTY_NC(ty, val, pr) \
public: \
	ty& pr ## _nc(bool dirty = false) { \
	    return val; \
	}; \

#define DEF_RCT3_D_PROPERTY_F(ty, val, pr) \
public: \
	void pr ## _read(FILE* f) { \
        fread(&val, sizeof(ty), 1, f); \
	}; \
	void pr ## _write(FILE* f) { \
        fwrite(&val, sizeof(ty), 1, f); \
	};

#endif
