/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef __libOVL_StyleOVL_h__
#define __libOVL_StyleOVL_h__

#include "CommonOVL.h"
#include "guiicon.h"
#include "icontexture.h"
#include "scenery.h"
#include "DisplayText.h"
#include "terraintype.h"
#include "stall.h"
#include "path.h"

#ifdef BUILDING_DLL
extern std::vector <Scenery *> SceneryItems;
#endif

extern unsigned long loadercount;
extern unsigned long symbolcount;
extern unsigned long symbolrefcount;
extern unsigned long LoaderPos;
extern unsigned long SymbolPos;
extern unsigned long SymbolRefPos;
extern char *GUIIcon;
extern char *extra[14];
extern char *extraparams[14];
extern char *supports[35];
void AddStringSize(const char *string);
void AddStringSize2(const char *string);
void AddLoader(unsigned long *data,int type,int SymbolsToResolve, const char *SymbolName, const char *Type,bool HasExtraData);
void AddLoader2(unsigned long *data,int type,int SymbolsToResolve,bool HasExtraData);
void AddRef(unsigned long *reference, char *SymbolName);
void AddRef2(unsigned long *reference, const char *SymbolName, const char *Type);
char *AddSymbol(const char *SymbolName, const char *Type);
char *AddString(const char *String);
char *FindString(const char *name, const char *type);

LIBOVL_API void AddStyleOVLInfo_SceneryItem(Scenery *scn);
LIBOVL_API void DeleteStyleOVLInfo_SceneryItems();
LIBOVL_API void AddStyleOVLInfo_IconTexture(IconTexture *ict);
LIBOVL_API void DeleteStyleOVLInfo_IconTextures();
LIBOVL_API void AddStyleOVLInfo_Icon(Icon *ico);
LIBOVL_API void DeleteStyleOVLInfo_Icons();
LIBOVL_API void AddStyleOVLInfo_TextString(Text *txt);
LIBOVL_API void DeleteStyleOVLInfo_TextStrings();
LIBOVL_API void AddStyleOVLInfo_Stalls(r3::StallStr *str);
LIBOVL_API void DeleteStyleOVLInfo_Stalls();
LIBOVL_API void AddStyleOVLInfo_Attractions(r3::AttractionStr *str);
LIBOVL_API void DeleteStyleOVLInfo_Attractions();

LIBOVL_API unsigned long CreateStyleOvl(char *filename, unsigned long ThemeCode);
LIBOVL_API bool saveStyleOVL();
LIBOVL_API void clearStyleOVLData();

#endif
