///////////////////////////////////////////////////////////////////////////////
//
// libXmlCpp
// A light C++ wrapper for libxml2, libxslt and libexslt
// Copyright (C) 2008 Tobias Minch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, <http://www.gnu.org/licenses/>.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////


#ifndef CXMLCONFIG_H_INCLUDED
#define CXMLCONFIG_H_INCLUDED

#include <libxml/xmlversion.h>

/** @file cxmlconfig.h
 *  \brief Configuration file for libxmlcpp
 *  This file allows you to comfigure certain options. Do not touch the ifdef's.
 **/

/// Use xslt transforms.
/**
 * You need to link libxslt if you use this. A lot of validators depend on this to
 * work with all features.
 **/
#define XMLCPP_USE_XSLT 1
#define XMLCPP_USE_EXSLT 1

// Special functions for wxWidgets support to prevent unwanted overload conversions
//#define XMLCPP_USE_WXWIDGETS 1

#if LIBXML_VERSION >= 20632
#define XMLCPP_USE_SCHEMATRON_PATCHED_LIBXML 1
#endif

#ifdef XMLCPP_USE_XSLT
// These all depend on libxslt support

/// Support for rng (xml RelaxNG) in the rnv-based RelaxNG validator
#define XMLCPP_USE_ISO_SCHEMATRON 1

#ifdef XMLCPP_USE_ISO_SCHEMATRON
/// Support for inlined Schematron
#define XMLCPP_USE_INLINED_SCHEMATRON 1
#endif

/// Support for rng (xml RelaxNG) in the rnv-based RelaxNG validator
#define XMLCPP_USE_RNV_RNG 1

#ifdef XMLCPP_USE_RNV_RNG
// These extend RelaxNG via RNV and do not work without it
/// Support short rng format (http://www.kohsuke.org/relaxng/shorthand/ShortRNG.html)
#define XMLCPP_USE_RNV_SHORTRNG 1
/// Support examplotron (http://examplotron.org/)
#define XMLCPP_USE_RNV_EXAMPLOTRON 1

#endif

#endif

#ifdef XMLCPP_SLIM
#ifdef XMLCPP_USE_RNV_RNG
#undef XMLCPP_USE_RNV_RNG
#endif

#ifdef XMLCPP_USE_RNV_SHORTRNG
#undef XMLCPP_USE_RNV_SHORTRNG
#endif

#ifdef XMLCPP_USE_RNV_EXAMPLOTRON
#undef XMLCPP_USE_RNV_EXAMPLOTRON
#endif

#endif

#endif // CXMLCONFIG_H_INCLUDED
