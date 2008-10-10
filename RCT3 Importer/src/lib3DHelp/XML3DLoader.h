///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// XML Loader
// Copyright (C) 2006 Tobias Minch
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

#ifndef XML3DLOADER_H_INCLUDED
#define XML3DLOADER_H_INCLUDED

#include "3DLoader.h"

#include "cXmlClasses.h"

class cXML3DLoader: public c3DLoader {
private:
    c3DLoaderOrientation m_ori;
    void parseVertex(r3::VERTEX2& v, xmlcpp::cXmlNode n, c3DMesh& mesh);
    void parseAnimation(c3DAnimation& a, xmlcpp::cXmlNode n, float _max);
public:
    cXML3DLoader(const wxChar *filename);
    virtual int getType() {return C3DLOADER_XML;};
    virtual c3DLoaderOrientation getOrientation() {return m_ori;};
};

#endif // ASE3DLOADER_H_INCLUDED
