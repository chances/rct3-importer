/*
    This file is part of the Virtual Object System of
    the Interreality project (http://interreality.org).

    Copyright (C) 2003 Peter Amstutz

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA

    Peter Amstutz <tetron@interreality.org>
*/

#ifndef _LIBASE_H_
#define _LIBASE_H_

#if defined(_WIN32) && defined(_MSC_VER)
# ifdef LIBASE_EXPORTS
#  define LIBASE_API __declspec(dllexport)
# else
#  define LIBASE_API __declspec(dllimport)
# endif
#else
# define LIBASE_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

typedef struct
{
    float r;
    float g;
    float b;
} ASE_ColorRGB;

typedef struct
{
    float x;
    float y;
    float z;
} ASE_Vector3D;

typedef struct
{
    char* name;
    int inherit_pos[3];
    int inherit_rot[3];
    int inherit_scl[3];
    float matrix[3][3];
    ASE_Vector3D pos;
    ASE_Vector3D rotaxis;
    float rotangle;
    ASE_Vector3D scale;
    ASE_Vector3D scaleaxis;
    float scaleangle;
} ASE_Transform;

typedef struct
{
    int vertex[3];
    int texture_coordinates[3];
    int smoothing;
    int mtlid;
    int ab;
    int bc;
    int ca;
    ASE_Vector3D face_normal;
} ASE_Face;

typedef struct
{
    int timevalue;
    int vertexCount;
    int textureCoordinateCount;
    int faceCount;
    ASE_Vector3D* vertices;
    ASE_Vector3D* vertex_normals;
    ASE_Vector3D* texture_coordinates;
    ASE_Face* faces;
} ASE_Mesh;

typedef struct
{
    char *name;
    ASE_Transform transform;
    ASE_Mesh mesh;
    int motionblur;
    int castShadow;
    int receiveShadow;
    int material;
} ASE_GeomObject;

typedef struct
{
    int timevalue;
    ASE_ColorRGB color;
    float intensity;
    float aspect;
    float tdist;
    float mapBias;
    float mapRange;
    int mapSize;
    float rayBias;
} ASE_LightSettings;

typedef struct
{
    char* name;
    char* type;
    ASE_Transform transform;
    char* shadows;
    int uselight;
    char* spotshape;
    int useglobal;
    int absMapBias;
    int overshoot;
    ASE_LightSettings settings;
} ASE_Light;

typedef struct
{
    char* name;
    char* mapclass;
    int subno;
    float amount;
    char* image_path;
    char* type;
    float u_offset;
    float v_offset;
    float u_tiling;
    float v_tiling;
    float angle;
    float blur;
    float blur_offset;
    float noise_amt;
    float noise_size;
    int noise_level;
    float noise_phase;
    char* bitmap_filter;
} ASE_MaterialMap;

typedef struct ASE_Material
{
    char* name;
    char* matclass;
    ASE_ColorRGB ambient;
    ASE_ColorRGB diffuse;
    ASE_ColorRGB specular;
    float shine;
    float shinestrength;
    float transparency;
    float wiresize;
    char* shading;
    float xp_falloff;
    float selfIllum;
    char* falloff;
    char* xp_type;
    ASE_MaterialMap diffuseMaterialMap;
    int numsubmaterials;
    struct ASE_Material* submaterials;
} ASE_Material;

typedef struct
{
    int timevalue;
    float near_val;
    float far_val;
    float fov;
    float tdist;
} ASE_CameraSettings;

typedef struct
{
    char* name;
    char* type;
    ASE_Transform camera;
    ASE_Transform target;
    ASE_CameraSettings settings;
} ASE_Camera;

typedef struct
{
    int firstFrame;
    int lastFrame;
    int frameSpeed;
    int ticksPerFrame;
    ASE_ColorRGB background_light;
    ASE_ColorRGB ambient_light;

    int objectCount;
    int lightCount;
    int materialCount;
    int cameraCount;

    ASE_GeomObject* objs;
    ASE_Light* lights;
    ASE_Material* materials;
    ASE_Camera* cameras;
} ASE_Scene;

LIBASE_API ASE_Scene* ASE_loadFilename(char* filename);

LIBASE_API ASE_Scene* ASE_loadFile(FILE* file);

LIBASE_API ASE_Scene* ASE_loadBuffer(char* buffer, int size);

LIBASE_API void ASE_freeScene(ASE_Scene* scene);

#ifdef __cplusplus
};
#endif

#endif
