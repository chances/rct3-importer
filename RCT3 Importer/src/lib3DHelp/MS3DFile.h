//------------
//Copyright © 2003 Mete Ciragan, chUmbaLum sOft
//
//This software is provided 'as-is', without any expressed or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
//
//Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
//
//1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software.
//   If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
//2. Altered versions must be plainly marked as such, and must not be misrepresented as being the original software.
//3. This notice may not be removed or altered from any distribution.
//
//------------
//You can find chUmbaLum sOft here: http://www.swissquake.ch/chumbalum-soft/

#ifndef _MS3DFILE_H
#define _MS3DFILE_H

#ifdef __WXDEBUG__
#include "wx_pch.h"
#endif

//#pragma once

//#include "main include file.h"
//#include <windef.h>
#include <vector>
#include <string>

#ifndef byte
typedef unsigned char byte;
#endif // byte

#ifndef word
typedef unsigned short word;
#endif // word

#ifndef UINT
typedef unsigned int UINT;
#endif

#ifndef DWORD
typedef unsigned long DWORD;
#endif

#ifndef WORD
typedef unsigned short WORD;
#endif

#ifndef LONG
typedef long LONG;
#endif

#ifndef MAKELONG
#define MAKELONG(a,b)   ((LONG)(((WORD)(a))|(((DWORD)((WORD)(b)))<<16)))
#endif

#ifndef LOWORD
#define LOWORD(l)       ((WORD)((DWORD)(l)))
#endif

#ifndef HIWORD
#define HIWORD(l)       ((WORD)(((DWORD)(l)>>16)&0xFFFF))
#endif

#ifndef FALSE
#define FALSE false
#endif

#ifndef TRUE
#define TRUE true
#endif

#if defined( _MSC_VER ) || defined( __BORLANDC__ )
#	include <pshpack1.h>
#	define PACK_STRUCT
#	define PATH_MAX _MAX_PATH
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#	include <limits.h>
#else
#	error you must byte-align these structures with the appropriate compiler directives
#endif

struct ms3d_header_t
{
    char    id[10];                                     // always "MS3D000000"
    int     version;                                    // 4
} PACK_STRUCT;

struct ms3d_vertex_t
{
    byte    flags;                                      // SELECTED | SELECTED2 | HIDDEN
    float   vertex[3];                                  //
    char    boneId;                                     // -1 = no bone
    byte    referenceCount;
} PACK_STRUCT;

struct ms3d_triangle_t
{
    word    flags;                                      // SELECTED | SELECTED2 | HIDDEN
    word    vertexIndices[3];                           //
    float   vertexNormals[3][3];                        //
    float   s[3];                                       //
    float   t[3];                                       //
    byte    smoothingGroup;                             // 1 - 32
    byte    groupIndex;                                 //
} PACK_STRUCT;

struct ms3d_edge_t
{
	word edgeIndices[2];
} PACK_STRUCT;

struct ms3d_group_t
{
    byte            flags;                              // SELECTED | HIDDEN
    char            name[32];                           //
    word            numtriangles;                       //
	std::vector<word> triangleIndices;					// the groups group the triangles
    char            materialIndex;                      // -1 = no material
} ;

struct ms3d_material_t
{
    char            name[32];                           //
    float           ambient[4];                         //
    float           diffuse[4];                         //
    float           specular[4];                        //
    float           emissive[4];                        //
    float           shininess;                          // 0.0f - 128.0f
    float           transparency;                       // 0.0f - 1.0f
    char            mode;                               // 0, 1, 2 is unused now
    char            texture[128];                        // texture.bmp
    char            alphamap[128];                       // alpha.bmp
} PACK_STRUCT;

struct ms3d_keyframe_rot_t
{
    float           time;                               // time in seconds
    float           rotation[3];                        // x, y, z angles
} PACK_STRUCT;

struct ms3d_keyframe_pos_t
{
    float           time;                               // time in seconds
    float           position[3];                        // local position
} PACK_STRUCT;

typedef struct
{
    byte            flags;                              // SELECTED | DIRTY
    char            name[32];                           //
    char            parentName[32];                     //
    float           rotation[3];                        // local reference matrix
    float           position[3];

    word            numKeyFramesRot;                    //
    word            numKeyFramesTrans;                  //

	std::vector<ms3d_keyframe_rot_t> keyFramesRot;      // local animation matrices
    std::vector<ms3d_keyframe_pos_t> keyFramesTrans;  // local animation matrices
} ms3d_joint_t;

typedef struct
{
	int index;											// index of group, material or joint
	int commentLength;									// length of comment (terminating '\0' is not saved), "MC" has comment length of 2 (not 3)
	std::string comment;        						// comment
} ms3d_comment_t;

struct ms3d_vertex_ex_t
{
	char boneIds[3];									// index of joint or -1, if -1, then that weight is ignored
	byte weights[3];									// vertex weight ranging from 0 - 255, last weight is computed by 1.0 - sum(all weights)
	// weight[0] is the weight for boneId in ms3d_vertex_t
	// weight[1] is the weight for boneIds[0]
	// weight[2] is the weight for boneIds[1]
	// 1.0f - weight[0] - weight[1] - weight[2] is the weight for boneIds[2]
} PACK_STRUCT;

#if defined( _MSC_VER ) || defined( __BORLANDC__ )
#include <poppack.h>
#endif

class CMS3DFile
{
public:
	CMS3DFile();
	virtual ~CMS3DFile();

public:
	DWORD LoadFromFile(const char* lpszFileName);
	void Clear();

	DWORD GetNumVertices();
	void GetVertexAt(int nIndex, ms3d_vertex_t **ppVertex);
	DWORD GetNumTriangles();
	void GetTriangleAt(int nIndex, ms3d_triangle_t **ppTriangle);
	DWORD GetNumEdges();
	void GetEdgeAt(int nIndex, ms3d_edge_t **ppEdge);
	DWORD GetNumGroups();
	void GetGroupAt(int nIndex, ms3d_group_t **ppGroup);
	DWORD GetNumMaterials();
	void GetMaterialAt(int nIndex, ms3d_material_t **ppMaterial);
	DWORD GetNumJoints();
	void GetJointAt(int nIndex, ms3d_joint_t **ppJoint);
	int FindJointByName(const char* lpszName);
	void GetVertexExAt(int nIndex, ms3d_vertex_ex_t **ppVertex);

	const std::vector<ms3d_comment_t>& GetGroupComments() const { return m_arrGroupComments; }
	const std::vector<ms3d_comment_t>& GetMaterialComments() const { return m_arrMaterialComments; }
	const std::vector<ms3d_comment_t>& GetJointComments() const { return m_arrJointComments; }
	const std::vector<ms3d_comment_t>& GetModelComments() const { return m_arrModelComments; }



	float GetAnimationFPS();
	float GetCurrentTimeM();
	int GetTotalFrames();


private:

	std::vector<ms3d_vertex_t> m_arrVertices;
	std::vector<ms3d_triangle_t> m_arrTriangles;
	std::vector<ms3d_edge_t> m_arrEdges;
	std::vector<ms3d_group_t> m_arrGroups;
	std::vector<ms3d_material_t> m_arrMaterials;
	float m_fAnimationFPS;
	float m_fCurrentTime;
	int m_iTotalFrames;
	std::vector<ms3d_joint_t> m_arrJoints;
	int m_subVersion;
	std::vector<ms3d_comment_t> m_arrGroupComments;
	std::vector<ms3d_comment_t> m_arrMaterialComments;
	std::vector<ms3d_comment_t> m_arrJointComments;
	std::vector<ms3d_comment_t> m_arrModelComments;
	std::vector<ms3d_vertex_ex_t> m_arrVerticesEx;

private:
	CMS3DFile(const CMS3DFile& rhs);
	CMS3DFile& operator=(const CMS3DFile& rhs);
};

#endif
