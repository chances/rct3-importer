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

//Modified for current ms3d spec by Tobias Minich (belgabor@gmx.de)

#include "MS3DFile.h"
#include <set>
#include <stdio.h>

CMS3DFile::CMS3DFile()
:	m_fAnimationFPS(24.0f),
	m_fCurrentTime(0.0f),
	m_iTotalFrames(0),
	m_subVersion(0)
{
}

CMS3DFile::~CMS3DFile()
{
}

DWORD CMS3DFile::LoadFromFile(LPCTSTR lpszFileName)
{
	FILE *fp = fopen(lpszFileName, "rb");
	if (!fp)
		return FALSE;

	DWORD i;
	ms3d_header_t header;
	fread(&header, 1, sizeof(ms3d_header_t), fp);

	if (strncmp(header.id, "MS3D000000", 10) != 0) {
	    fclose(fp);
		return FALSE;
	}

	if (header.version != 4) {
	    fclose(fp);
		return FALSE;
	}

	// vertices
	word nNumVertices;
	fread(&nNumVertices, 1, sizeof(word), fp);
	m_arrVertices.clear();
	m_arrVertices.resize(nNumVertices);

	fread(&m_arrVertices[0], nNumVertices, sizeof(ms3d_vertex_t), fp);

	// triangles
	word nNumTriangles;
	fread(&nNumTriangles, 1, sizeof(word), fp);
	m_arrTriangles.resize(nNumTriangles);
	fread(&m_arrTriangles[0], nNumTriangles, sizeof(ms3d_triangle_t), fp);

	// edges
	std::set<DWORD> setEdgePair;
	for (i = 0; i < m_arrTriangles.size(); i++)
	{
		WORD a, b;
		a = m_arrTriangles[i].vertexIndices[0];
		b = m_arrTriangles[i].vertexIndices[1];
		if (a > b)
			std::swap(a, b);
		if (setEdgePair.find(MAKELONG(a, b)) == setEdgePair.end())
			setEdgePair.insert(MAKELONG(a, b));

		a = m_arrTriangles[i].vertexIndices[1];
		b = m_arrTriangles[i].vertexIndices[2];
		if (a > b)
			std::swap(a, b);
		if (setEdgePair.find(MAKELONG(a, b)) == setEdgePair.end())
			setEdgePair.insert(MAKELONG(a, b));

		a = m_arrTriangles[i].vertexIndices[2];
		b = m_arrTriangles[i].vertexIndices[0];
		if (a > b)
			std::swap(a, b);
		if (setEdgePair.find(MAKELONG(a, b)) == setEdgePair.end())
			setEdgePair.insert(MAKELONG(a, b));
	}

	for(std::set<DWORD>::iterator it = setEdgePair.begin(); it != setEdgePair.end(); ++it)
	{
		DWORD EdgePair = *it;
		ms3d_edge_t Edge;
		Edge.edgeIndices[0] = LOWORD(EdgePair);
		Edge.edgeIndices[1] = HIWORD(EdgePair);
		m_arrEdges.push_back(Edge);
	}

	// groups
	word nNumGroups;
	fread(&nNumGroups, 1, sizeof(word), fp);
	m_arrGroups.resize(nNumGroups);
	for (i = 0; i < nNumGroups; i++)
	{
		fread(&m_arrGroups[i].flags, 1, sizeof(byte), fp);
		fread(&m_arrGroups[i].name, 32, sizeof(char), fp);
		fread(&m_arrGroups[i].numtriangles, 1, sizeof(word), fp);
		m_arrGroups[i].triangleIndices.resize(m_arrGroups[i].numtriangles);
		fread(&m_arrGroups[i].triangleIndices[0], m_arrGroups[i].numtriangles, sizeof(word), fp);
		fread(&m_arrGroups[i].materialIndex, 1, sizeof(char), fp);
	}

	// materials
	word nNumMaterials;
	fread(&nNumMaterials, 1, sizeof(word), fp);
	m_arrMaterials.resize(nNumMaterials);
	fread(&m_arrMaterials[0], nNumMaterials, sizeof(ms3d_material_t), fp);

	fread(&m_fAnimationFPS, 1, sizeof(float), fp);
	fread(&m_fCurrentTime, 1, sizeof(float), fp);
	fread(&m_iTotalFrames, 1, sizeof(int), fp);

	// joints
	word nNumJoints;
	fread(&nNumJoints, 1, sizeof(word), fp);
	m_arrJoints.resize(nNumJoints);
	for (i = 0; i < nNumJoints; i++)
	{
		fread(&m_arrJoints[i].flags, 1, sizeof(byte), fp);
		fread(&m_arrJoints[i].name, 32, sizeof(char), fp);
		fread(&m_arrJoints[i].parentName, 32, sizeof(char), fp);
		fread(&m_arrJoints[i].rotation, 3, sizeof(float), fp);
		fread(&m_arrJoints[i].position, 3, sizeof(float), fp);
		fread(&m_arrJoints[i].numKeyFramesRot, 1, sizeof(word), fp);
		fread(&m_arrJoints[i].numKeyFramesTrans, 1, sizeof(word), fp);
		m_arrJoints[i].keyFramesRot.resize(m_arrJoints[i].numKeyFramesRot);
		m_arrJoints[i].keyFramesTrans.resize(m_arrJoints[i].numKeyFramesTrans);
		fread(&m_arrJoints[i].keyFramesRot[0], m_arrJoints[i].numKeyFramesRot, sizeof(ms3d_keyframe_rot_t), fp);
		fread(&m_arrJoints[i].keyFramesTrans[0], m_arrJoints[i].numKeyFramesTrans, sizeof(ms3d_keyframe_pos_t), fp);
	}

	if (!feof(fp)) {
	    fread(&m_subVersion, 1, sizeof(m_subVersion), fp);
	    if (m_subVersion != 1) {
            fclose(fp);
            return 2;
	    }

        int nNumGroupComments;
        fread(&nNumGroupComments, 1, sizeof(int), fp);
        m_arrGroupComments.resize(nNumGroupComments);
        for (i = 0; i < nNumGroupComments; i++)
        {
            fread(&m_arrGroupComments[i].index, 1, sizeof(int), fp);
            fread(&m_arrGroupComments[i].commentLength, 1, sizeof(int), fp);
            char *tmp = new char[m_arrGroupComments[i].commentLength+1];
            fread(tmp, m_arrGroupComments[i].commentLength, sizeof(char), fp);
            tmp[m_arrGroupComments[i].commentLength] = 0;
            m_arrGroupComments[i].comment = tmp;
            delete[] tmp;
        }

        int nNumMaterialComments;
        fread(&nNumMaterialComments, 1, sizeof(int), fp);
        m_arrMaterialComments.resize(nNumMaterialComments);
        for (i = 0; i < nNumMaterialComments; i++)
        {
            fread(&m_arrMaterialComments[i].index, 1, sizeof(int), fp);
            fread(&m_arrMaterialComments[i].commentLength, 1, sizeof(int), fp);
            char *tmp = new char[m_arrMaterialComments[i].commentLength+1];
            fread(tmp, m_arrMaterialComments[i].commentLength, sizeof(char), fp);
            tmp[m_arrMaterialComments[i].commentLength] = 0;
            m_arrMaterialComments[i].comment = tmp;
            delete[] tmp;
        }

        int nNumJointComments;
        fread(&nNumJointComments, 1, sizeof(int), fp);
        m_arrJointComments.resize(nNumJointComments);
        for (i = 0; i < nNumJointComments; i++)
        {
            fread(&m_arrJointComments[i].index, 1, sizeof(int), fp);
            fread(&m_arrJointComments[i].commentLength, 1, sizeof(int), fp);
            char *tmp = new char[m_arrJointComments[i].commentLength+1];
            fread(tmp, m_arrJointComments[i].commentLength, sizeof(char), fp);
            tmp[m_arrJointComments[i].commentLength] = 0;
            m_arrJointComments[i].comment = tmp;
            delete[] tmp;
        }

        int nNumModelComments;
        fread(&nNumModelComments, 1, sizeof(int), fp);
        m_arrModelComments.resize(nNumModelComments);
        for (i = 0; i < nNumModelComments; i++)
        {
            fread(&m_arrModelComments[i].index, 1, sizeof(int), fp);
            fread(&m_arrModelComments[i].commentLength, 1, sizeof(int), fp);
            char *tmp = new char[m_arrModelComments[i].commentLength+1];
            fread(tmp, m_arrModelComments[i].commentLength, sizeof(char), fp);
            tmp[m_arrModelComments[i].commentLength] = 0;
            m_arrModelComments[i].comment = tmp;
            delete[] tmp;
        }

        m_arrVerticesEx.resize(nNumVertices);
        fread(&m_arrVerticesEx[0], nNumVertices, sizeof(ms3d_vertex_ex_t), fp);
	} else {
	    fclose(fp);
	    return 2;
	}

	fclose(fp);

	return TRUE;
}

void CMS3DFile::Clear()
{
	m_arrVertices.clear();
	m_arrTriangles.clear();
	m_arrEdges.clear();
	m_arrGroups.clear();
	m_arrMaterials.clear();
	m_arrJoints.clear();
	m_arrGroupComments.clear();
	m_arrMaterialComments.clear();
	m_arrJointComments.clear();
	m_arrModelComments.clear();
	m_arrVerticesEx.clear();
}

DWORD CMS3DFile::GetNumVertices()
{
	return (DWORD) m_arrVertices.size();
}

void CMS3DFile::GetVertexAt(int nIndex, ms3d_vertex_t **ppVertex)
{
	if (nIndex >= 0 && nIndex < (int) m_arrVertices.size())
		*ppVertex = &m_arrVertices[nIndex];
}


DWORD CMS3DFile::GetNumTriangles()
{
	return (DWORD) m_arrTriangles.size();
}

void CMS3DFile::GetTriangleAt(int nIndex, ms3d_triangle_t **ppTriangle)
{
	if (nIndex >= 0 && nIndex < (int) m_arrTriangles.size())
		*ppTriangle = &m_arrTriangles[nIndex];
}

DWORD CMS3DFile::GetNumEdges()
{
	return (DWORD) m_arrEdges.size();
}

void CMS3DFile::GetEdgeAt(int nIndex, ms3d_edge_t **ppEdge)
{
	if (nIndex >= 0 && nIndex < (int) m_arrEdges.size())
		*ppEdge = &m_arrEdges[nIndex];
}

DWORD CMS3DFile::GetNumGroups()
{
	return (DWORD) m_arrGroups.size();
}

void CMS3DFile::GetGroupAt(int nIndex, ms3d_group_t **ppGroup)
{
	if (nIndex >= 0 && nIndex < (int) m_arrGroups.size())
		*ppGroup = &m_arrGroups[nIndex];
}

DWORD CMS3DFile::GetNumMaterials()
{
	return (DWORD) m_arrMaterials.size();
}

void CMS3DFile::GetMaterialAt(int nIndex, ms3d_material_t **ppMaterial)
{
	if (nIndex >= 0 && nIndex < (int) m_arrMaterials.size())
		*ppMaterial = &m_arrMaterials[nIndex];
}

DWORD CMS3DFile::GetNumJoints()
{
	return (DWORD) m_arrJoints.size();
}

void CMS3DFile::GetJointAt(int nIndex, ms3d_joint_t **ppJoint)
{
	if (nIndex >= 0 && nIndex < (int) m_arrJoints.size())
		*ppJoint = &m_arrJoints[nIndex];
}

int CMS3DFile::FindJointByName(LPCTSTR lpszName)
{
	for (UINT i = 0; i < m_arrJoints.size(); i++)
	{
		if (!strcmp(m_arrJoints[i].name, lpszName))
			return i;
	}

	return -1;
}

float CMS3DFile::GetAnimationFPS()
{
	return m_fAnimationFPS;
}

float CMS3DFile::GetCurrentTime()
{
	return m_fCurrentTime;
}

int CMS3DFile::GetTotalFrames()
{
	return m_iTotalFrames;
}
