#ifndef MANIFOLDMESH_H_INCLUDED
#define MANIFOLDMESH_H_INCLUDED

#include "vertex.h"

namespace r3 {
	
struct ManifoldMeshVertex {
	VECTOR position;
    uint32_t unk04;
};

struct ManifoldMesh {
    ManifoldMeshVertex  bbox_min;
    ManifoldMeshVertex  bbox_max;
    uint32_t            vertex_count;
    uint32_t            mainfoldface_count;     ///< a manifoldface seems to consist of 3 shorts.
    ManifoldMeshVertex* vertices;
    uint16_t*           mainfoldface_indices;   ///< Zero-padded to 4 longs.
};


};

#endif // MANIFOLDMESH_H_INCLUDED
