/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef __libOVL_OVL_h__
#define __libOVL_OVL_h__

#include "CommonOVL.h"
#include "flexitexture.h"
#include "staticshape.h"
#include "boneshape.h"
#include "boneanim.h"
#include "sceneryvisual.h"

typedef struct
{
	float x;
	float y;
	float z;
} libOVL_Vector3D;

typedef struct
{
	int vertex[3];
	int texture_coordinates[3];
	int smoothing;
	int mtlid;
	int ab;
	int bc;
	int ca;
	libOVL_Vector3D face_normal;
} libOVL_Face;

typedef struct
{
	int timevalue;
	int vertexCount;
	int textureCoordinateCount;
	int faceCount;
	libOVL_Vector3D* vertices;
	libOVL_Vector3D* vertex_normals;
	libOVL_Vector3D* texture_coordinates;
	libOVL_Face* faces;
} libOVL_Mesh;



//These are the structures that are added from outside
typedef struct
{
	libOVL_Mesh *mesh;
	char* meshname;
	char* textureIntOVLName; //Internal name of the texture, without the :ftx
	char* textureStyle; //Texture rendering style. Present in RCT3.dgf
	VECTOR BoundingBox1;
	VECTOR BoundingBox2;
} libOVL_MeshInfo;

typedef struct
{
	unsigned char *texturedata; //parent pointer having other members as offset
	unsigned char *texturealpha;
	FlexiTextureStruct *fts;
	FlexiTextureInfoStruct *fti;
	COLOURQUAD *colors;
	char* textureIntOVLName; //Internal name of the texture, without the :ftx
} libOVL_FlexiTextureInfo;

struct EffectPoint
{
	char *Name;
	MATRIX Transform;
};

#ifdef BUILDING_DLL
std::vector <char *> References;
std::vector <EffectPoint *> EffectPoints;
std::vector <libOVL_MeshInfo *> MeshItems;
std::vector <libOVL_FlexiTextureInfo *> FlexiTextureItems;
#endif

LIBOVL_API void AddOVLInfo_Mesh(libOVL_Mesh* mesh, char* meshname, char* textureIntOVLName,char* textureStyle,VECTOR BoundingBox1,VECTOR BoundingBox2);
LIBOVL_API void DeleteOVLInfo_Meshes();
LIBOVL_API void AddOVLInfo_FlexiTexture(FlexiTextureStruct *fts, FlexiTextureInfoStruct *fti,RGBQUAD * Bitmapcolors, char* textureIntOVLName);
LIBOVL_API void DeleteOVLInfo_FlexiTextures();

LIBOVL_API void CreateSceneryOvl(char *filename);
LIBOVL_API void AddStaticShape(char *StaticShapeName, char **FTXNames, char **TXSNames, StaticShape1 *sh);
LIBOVL_API void AddBoneShape(char *BoneShapeName, char **FTXNames, char **TXSNames, BoneShape1 *sh);
LIBOVL_API void AddBoneAnim(char *BoneAnimName, BoneAnim *ban);
LIBOVL_API void AddSceneryItemVisual(char *SceneryItemVisualName, char **StaticShapeNames, char **BoneShapeNames, char **BoneAnimNames, SceneryItemVisual *svd);
LIBOVL_API void AddFlexiTexture(char *FlexiTextureName, FlexiTextureInfoStruct *fti);
LIBOVL_API void AddReference(char *reference);
LIBOVL_API void SaveSceneryOvl();
LIBOVL_API unsigned long CreateOvl(char *filename,char *InternalName);
LIBOVL_API void clearOVLData();
LIBOVL_API bool SaveOvl();


#endif
