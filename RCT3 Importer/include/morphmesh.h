#ifndef MORPHMESH_H_INCLUDED
#define MORPHMESH_H_INCLUDED

struct MorphMeshVertexUV {
    unsigned short unk1a;
    unsigned short unk1b; ///< usually unk1a = unk1b
    float u;
    float v;
};

struct MorphMesh2 {
    float unk1;
    float unk2;
    float unk3;
    unsigned long unk4; ///< Seen 0
    float unk5;
    float unk6;
    float unk7;
    unsigned long unk8; ///< Seen 0
    char* morphname; ///< Animation?
    unsigned long timecount;
    float* times;
    void* morphdata; ///< length related to timecount withith same MorphMesh
    unsigned long unk13; ///< Seen 0
    unsigned long unk14; ///< Seen 0
    unsigned long unk15; ///< Seen 0
    unsigned long unk16; ///< Seen 0
};

struct MorphMesh {
    unsigned long vertexcount;
    unsigned long indexcount;
    unsigned long unk3;
    unsigned long unk4; ///< Usually = unk3
    unsigned long unk5; ///< Seen 0
    unsigned long morphcount;
    MorphMeshVertexUV* vertexuvs; ///< vertexcount items.
    unsigned short* indices; ///< count2 items.
    unsigned long unk9; ///< Seen 0
    MorphMesh2* morphs; ///< morphcount items.
};


#endif // MORPHMESH_H_INCLUDED
