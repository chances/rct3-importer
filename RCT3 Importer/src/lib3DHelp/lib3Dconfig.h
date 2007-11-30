///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// Defines for lib3DHelp relevant cxConfig paths
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

#ifndef LIB3DCONFIG_H_INCLUDED
#define LIB3DCONFIG_H_INCLUDED

#define READ_RCT3_ORIENTATION()     wxcRead<long>(wxT("RCT3/Orientation"), ORIENTATION_LEFT_YUP)
#define WRITE_RCT3_ORIENTATION(v)   wxcWrite<long>(wxT("RCT3/Orientation"), v)

#define READ_RCT3_TEXTURE()     wxcRead<long>(wxT("RCT3/TextureScaling"), RCT3_TEXTURE_ERROR_OUT)
#define WRITE_RCT3_TEXTURE(v)   wxcWrite<long>(wxT("RCT3/TextureScaling"), v)

#define READ_RCT3_EXPERTMODE()     wxcRead<bool>(wxT("RCT3/ExpertMode"), false)
#define WRITE_RCT3_EXPERTMODE(v)   wxcWrite<bool>(wxT("RCT3/ExpertMode"), v)

#endif
