#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include "vertex.h"

namespace r3 {


struct ModelExtraHeader {
	int32_t bone_count;
	int32_t unk01;
	int32_t unk02;
	int32_t unk03;
};
	
struct ModelExtraBoneDecl {
	QUATERNION position; // last value appears to be always 1.0
	QUATERNION rotation;
	MATRIX matrix;
};

struct ModelExtraBoneParentDecl {
	uint16_t parent;  // 0xFFFF probably for no parent
	uint16_t bone_nr; // Maybe index for name?
	// Very Unsure...
};

struct ModelExtraModelHeader {
	uint16_t texture_count; //?
	uint16_t texture;       //?
	uint16_t bone_count;    //?
	uint16_t zeros[7];      // 14 bytes zeros
}

struct ModelExtraMeshHeader {
	uint32_t unk01;		// 0x00001305
	uint32_t index_count;
	uint16_t unk02;
	unit16_t vertex_count;
	uint16_t unk03;		// 0x0000
	uint16_t unk04;		// 0xFFFF
};

struct ModelExtraTerminator {
	uint32_t zeros[4];  // 16 0-bytes	
};

};


/*
 * Extradata:
 * 
 * Junk1
 * ModelExtraHeader
 * ModelExtraBoneDecl[ModelExtraHeader.bone_count]
 * ModelExtraBoneParentDecl[ModelExtraHeader.bone_count]
 * (Padding to 16 byte width if necessary)
 * int32_t TextureNameLength[?]  // Includes terminating 0. Unknown how length is determined (probably size check to detect characters?)
 * char[x]                       // 0-terminated strings of sizes
 * (Padding to 16 byte width if necessary)
 * 
 * ModelExtraModelHeader[6]
 * (Padding to 16 byte width if necessary)
 * ModelExtraTerminator
 * 
 * ModelExtraMeshHeader[y]
 * (Padding to 16 byte width if necessary)
 * ModelExtraTerminator
 * a in y {
 *   VERTEX2[ModelExtraMeshHeader[a].vertex_count]
 *   uint16_t[ModelExtraMeshHeader[a].index_count]
 * }
 * 

#endif // MANIFOLDMESH_H_INCLUDED
