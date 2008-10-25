/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef __ICONTEXTURE_H__
#define __ICONTEXTURE_H__

#include <string>

struct BmpTbl
{
	unsigned long unk; ///< always 0 on disk files.
	unsigned long count; ///< Number of stored textures.
};

//before FlicStruct, put pointer to FlicStruct, loader points to FlicStruct
struct FlicStruct
{
	unsigned long *FlicDataPtr; //always 0 on disk files
	unsigned long unk1; //always 1
	float unk2; //always 1.0
};
//Present only in importer
struct Flic
{
	FlicStruct *fl;
	FlicStruct fl2;
};

struct FlicHeader
{
	unsigned long Format;
	unsigned long Width;
	unsigned long Height;
	unsigned long Mipcount;
};

struct FlicMipHeader
{
	unsigned long MWidth;
	unsigned long MHeight;
	unsigned long Pitch;
	unsigned long Blocks;
};


struct IconTexture
{
	std::string name;
	std::string filename;
	FlicHeader fh;
	FlicMipHeader fmh;
	unsigned char *data;
	IconTexture(): data(NULL) {}
};

struct TextureStruct2;

struct TextureStruct
{
	unsigned long unk1; ///< always 0x70007
	unsigned long unk2; ///< always 0x70007
	unsigned long unk3; ///< always 0x70007
	unsigned long unk4; ///< always 0x70007
	unsigned long unk5; ///< always 0x70007
	unsigned long unk6; ///< always 0x70007
	unsigned long unk7; ///< always 0x70007
	unsigned long unk8; ///< always 0x70007
	unsigned long unk9; ///< Usually 1.
                        /**< Seems to be a count, either for the TextureStruct.Flic array or for the array inside TextureStruct2.
                          */
	unsigned long unk10; ///< Usually 8.
	                     /**< Other values:
	                      *     - 4 (if unk9 and unk12 are 0)
	                      *     - 12 (if unk9 and unk12 are 2)
	                      */
	unsigned long unk11; ///< Usually 0x10.
	                     /**< Other values:
	                      *     - 0x00 (GUIRendererBitmap:txs, UIRendererColour:txs, GUIRendererZMask:txs, SystemFont:txs, VisibleFootprint:txs)
	                      *     - 0x0D
	                      */
	unsigned long *TextureData; ///< Symbol reference for a TXS (texture style).
                                /**< Always 0 on disk files, use GUIIcon:txs as a symbol resolve for icons.\n
                                 * Common symrefs:
                                 *    - GUIIcon:txs. For icon textures.
                                 *    - PathGround:txs. For paths.
                                 *    - TerrainBlending:txs. For terrain.
                                 *    - TerrainCliff:txs. For cliffs.
                                 *
                                 * Other symrefs (probably not useful for custom stuff):
                                 *    - EnvMap:txs
                                 *    - GUIRendererBitmap:txs. unk9 and unk12 (low) are 0.
                                 *    - GUIRendererColour:txs. unk9 and unk12 (low) are 0.
                                 *    - GUIRendererZMask:txs. unk9 and unk12 (low) are 0.
                                 *    - GUISkin:txs
                                 *    - GUISkinTile:txs
                                 *    - GUISolidColour:txs. unk9 and unk12 (low) are 0.
                                 *    - GUISolidColourOpaque:txs. unk9 and unk12 (low) are 0.
                                 *    - HorizonShadowUV:txs
                                 *    - LaserTS:txs
                                 *    - LaserCapTS:txs
                                 *    - PoolPreviewGrid:txs
                                 *    - PoolPreviewGrid2:txs. unk9 and unk12 (low) are 2.
                                 *    - Rope:txs
                                 *    - ShapeStandard:txs
                                 *    - SolidColourFont:txs
                                 *    - SystemFont:txs
                                 *    - TerrainCircleBack:txs
                                 *    - TerrainCircleFront:txs
                                 *    - TerrainCliffTransparent:txs
                                 *    - TerrainContour:txs. unk9 and unk12 (low) are 2.
                                 *    - TerrainDebug:txs. unk9 and unk12 (low) are 0.
                                 *    - TerrainDetail:txs
                                 *    - TerrainDetailAndLightmap:txs. unk9 and unk12 (low) are 0.
                                 *    - TerrainFakeUp:txs
                                 *    - TerrainFog:txs
                                 *    - TerrainGrid:txs
                                 *    - TerrainGrid2StageDummy:txs. unk9 and unk12 (low) are 0.
                                 *    - TerrainHighlight:txs
                                 *    - TerrainHole:txs
                                 *    - TerrainPosition:txs
                                 *    - TerrainPositionNoZ:txs
                                 *    - TerrainTransparent:txs
                                 *    - TerrainZRender:txs
                                 *    - VisibleFootprint:txs
                                 *    - Underground:txs
                                 *    - WaypointHighlight:txs
                                 *    - ZFillAlpha:txs
                                 */
	unsigned long unk12; ///< Usually 1.
	                     /**< loword is a count like unk9 (see there).
                          * hiword is addonpack and in this case determines the number of unknown longs at the end of
                          * the structure.
                          */
	FlicStruct **Flic; ///< always points to pointer before flic data
                       /**< Is an array with either unk9 or unk12 members.
                        * Not relocated if those are 0.
                        */
	TextureStruct2 *ts2;
};

struct TextureStruct2
{
	TextureStruct *Texture; ///< points back to the TextureStruct
	FlicStruct *Flic; ///< points to the FlicStruct for this item.
	                  /**< This is actually seems to be an array with either TextureStruct.unk9 or TextureStruct.unk12 (loword)
	                   * items.
	                   */
};

struct TextureStruct2Ext
{
	TextureStruct *Texture; ///< points back to the TextureStruct
	FlicStruct *Flic; ///< points to the FlicStruct for this item.
	                  /**< This is actually seems to be an array with either TextureStruct.unk9 or TextureStruct.unk12 (loword)
	                   * items.
	                   */
	FlicStruct *Flic2; ///< points to the second FlicStruct for this item.
};

struct Tex
{
	TextureStruct t1;
	TextureStruct2 t2;
};

#endif
