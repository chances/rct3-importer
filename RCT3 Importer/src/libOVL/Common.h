/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef __libOVL_Common_h__
#define __libOVL_Common_h__

#include "vertex.h"

#define WIN32_MEAN_AND_LEAN
#include "win_predefine.h"
//#include <windows.h> //predefine already includes that
//#include <d3d9types.h>
#include <stdio.h>
#pragma warning (disable:4702 4701)
#include <queue>
#include <vector>
#pragma warning (default:4702 4701)

// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the LIBOVL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// LIBOVL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifndef LIBOVL_STATIC
#ifdef LIBOVL_EXPORTS
#define LIBOVL_API extern "C" __declspec(dllexport)
#else
#define LIBOVL_API extern "C" __declspec(dllimport)
#endif
#else
#define LIBOVL_API
#endif

extern FILE *f;

unsigned long log2(unsigned long y);

#endif
