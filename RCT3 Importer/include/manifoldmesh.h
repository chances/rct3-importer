#ifndef MANIFOLDMESH_H_INCLUDED
#define MANIFOLDMESH_H_INCLUDED

namespace r3 {

struct ManifoldMeshVertex {
    float_t x;
    float_t y;
    float_t z;
    uint32_t unk04;
};

struct ManifoldMesh {
    ManifoldMeshVertex  bbox_min;
    ManifoldMeshVertex  bbox_max;
    uint32_t            vertex_count;
    uint32_t            mainfoldface_count;     ///< a manifoldface seems to consist of 6 shorts.
    ManifoldMeshVertex* vertices;
    uint16_t*           mainfoldface_indices;   ///< Zero-padded to 4 longs.
};


};

#endif // MANIFOLDMESH_H_INCLUDED
