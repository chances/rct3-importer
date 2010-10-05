///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for MAM structures
// Copyright (C) 2010 Tobias Minch
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
// Based on libOVL by Jonathan Wilson
//
///////////////////////////////////////////////////////////////////////////////

#include "OVLDebug.h"

#include "ManagerMAM.h"

using namespace r3;
using namespace std;

const char* ovlMAMManager::NAME = "ManifoldMesh";
const char* ovlMAMManager::TAG = "mam";

void cManifoldMesh::Fill(r3::ManifoldMesh* mam) const {
	mam->bbox_min.position = bbox1;
	mam->bbox_min.unk04 = 0;
	mam->bbox_max.position = bbox2;
	mam->bbox_max.unk04 = 0;
	mam->vertex_count = vertices.size();
	mam->mainfoldface_count = indices.size() / 3;
	int i;
	for(i=0; i < vertices.size(); i++)
		mam->vertices[i] = vertices[i];
	for(i=0; i < indices.size(); i++)
		mam->mainfoldface_indices[i] = indices[i];
}


void ovlMAMManager::AddMesh(const cManifoldMesh& item) {
    DUMP_LOG("Trace: ovlMAMManager::Make(%s)", UNISTR(item.name.c_str()));
    Check("ovlMAMManager::AddMesh");
    if (item.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlMAMManager::AddMesh called without name"));
    if (m_items.find(item.name) != m_items.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlMAMManager::AddMesh: Item with name '"+item.name+"' already exists"));

    m_items[item.name] = item;

    // ManifoldMesh
    m_size += sizeof(ManifoldMesh);
	// Vertices
	m_size += item.vertices.size() * sizeof(ManifoldMeshVertex);
	// Indices
	int indices = item.indices.size();
	if (indices % 8)
		indices += (8 - (indices % 8));
	m_size += indices * sizeof(unsigned short);

	cLoader& loader = GetLSRManager()->reserveIndexElement(OVLT_COMMON, item.name, ovlMAMManager::TAG);
}

void ovlMAMManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlMAMManager::Make()");
    Check("ovlMAMManager::Make");

    m_blobs[""] = cOvlMemBlob(OVLT_COMMON, 4, m_size);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs[""].data;
	
	foreach(const cManifoldMesh::mapentry item, m_items) {
		ManifoldMesh* c_mesh = reinterpret_cast<ManifoldMesh*>(c_data);
		c_data += sizeof(ManifoldMesh);
		
		c_mesh->vertices = reinterpret_cast<ManifoldMeshVertex*>(c_data);
		c_data += item.second.vertices.size() * sizeof(ManifoldMeshVertex);
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_mesh->vertices));
        DUMP_RELOCATION("ovlMAMManager::Make, vertices", c_mesh->vertices);
		
		c_mesh->mainfoldface_indices = reinterpret_cast<uint16_t*>(c_data);
		int indices = item.second.indices.size();
		if (indices % 8)
			indices += (8 - (indices % 8));
		c_data += indices * sizeof(unsigned short);
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_mesh->mainfoldface_indices));
        DUMP_RELOCATION("ovlMAMManager::Make, faces", c_mesh->mainfoldface_indices);
		
		cLoader& loader = GetLSRManager()->assignIndexElement(OVLT_COMMON, item.first, ovlMAMManager::TAG, c_mesh);
		item.second.Fill(c_mesh);
	}
}
