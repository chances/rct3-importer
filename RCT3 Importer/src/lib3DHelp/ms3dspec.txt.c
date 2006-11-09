//
//
//
//                MilkShape 3D 1.7.3 File Format Specification
//
//
//                  This specifcation is written in C style.
//
//
// The data structures are defined in the order as they appear in the .ms3d file.
//
//
//
//
//



//
// max values
//
#define MAX_VERTICES    65534
#define MAX_TRIANGLES   65534
#define MAX_GROUPS      255
#define MAX_MATERIALS   128
#define MAX_JOINTS      128
#define MAX_KEYFRAMES   216     // increase when needed



//
// flags
//
#define SELECTED        1
#define HIDDEN          2
#define SELECTED2       4
#define DIRTY           8



//
// types
//
#ifndef byte
typedef unsigned char byte;
#endif // byte

#ifndef word
typedef unsigned short word;
#endif // word


// force one byte alignment
#include <pshpack1.h>

//
// First comes the header.
//
typdef struct
{
    char    id[10];                                     // always "MS3D000000"
    int     version;                                    // 4
} ms3d_header_t;

//
// Then comes the number of vertices
//
word nNumVertices;

//
// Then comes nNumVertices * sizeof (ms3d_vertex_t)
//
typedef struct
{
    byte    flags;                                      // SELECTED | SELECTED2 | HIDDEN
    float   vertex[3];                                  //
    char    boneId;                                     // -1 = no bone
    byte    referenceCount;
} ms3d_vertex_t;

//
// number of triangles
//
word nNumTriangles;

//
// nNumTriangles * sizeof (ms3d_triangle_t)
//
typedef struct
{
    word    flags;                                      // SELECTED | SELECTED2 | HIDDEN
    word    vertexIndices[3];                           //
    float   vertexNormals[3][3];                        //
    float   s[3];                                       //
    float   t[3];                                       //
    byte    smoothingGroup;                             // 1 - 32
    byte    groupIndex;                                 //
} ms3d_triangle_t;

//
// number of groups
//
word nNumGroups;

//
// nNumGroups * sizeof (ms3d_group_t)
//
typedef struct
{
    byte            flags;                              // SELECTED | HIDDEN
    char            name[32];                           //
    word            numtriangles;                       //
    word            triangleIndices[numtriangles];      // the groups group the triangles
    char            materialIndex;                      // -1 = no material
} ms3d_group_t;

//
// number of materials
//
word nNumMaterials;

//
// nNumMaterials * sizeof (ms3d_material_t)
//
typedef struct
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
} ms3d_material_t;

//
// save some keyframer data
//
float fAnimationFPS;
float fCurrentTime;
int iTotalFrames;

//
// number of joints
//
word nNumJoints;

//
// nNumJoints * sizeof (ms3d_joint_t)
//
//
typedef struct
{
    float           time;                               // time in seconds
    float           rotation[3];                        // x, y, z angles
} ms3d_keyframe_rot_t;

typedef struct
{
    float           time;                               // time in seconds
    float           position[3];                        // local position
} ms3d_keyframe_pos_t;

typedef struct
{
    byte            flags;                              // SELECTED | DIRTY
    char            name[32];                           //
    char            parentName[32];                     //
    float           rotation[3];                        // local reference matrix
    float           position[3];

    word            numKeyFramesRot;                    //
    word            numKeyFramesTrans;                  //

    ms3d_keyframe_rot_t keyFramesRot[numKeyFramesRot];      // local animation matrices
    ms3d_keyframe_pos_t keyFramesTrans[numKeyFramesTrans];  // local animation matrices
} ms3d_joint_t;

//
// subversion, stuff attached to file
//
int subVersion											// must be 1

int nNumGroupComments;

//
// nNumGroupComments * ms3d_comment_t
//
typedef struct
{
	int index;											// index of group, material or joint
	int commentLength;									// length of comment (terminating '\0' is not saved), "MC" has comment length of 2 (not 3)
	char comment[commentLength];						// comment
} ms3d_comment_t;

int nNumMaterialComments;

//
// nNumMaterialComments * ms3d_comment_t
//

int nNumJointComments;

//
// nNumJointComments * ms3d_comment_t
//

int nHasModelComment;

//
// nHasModelComment * ms3d_comment_t
//

typedef struct
{
	char boneIds[3];									// index of joint or -1, if -1, then that weight is ignored
	byte weights[3];									// vertex weight ranging from 0 - 255, last weight is computed by 1.0 - sum(all weights)
	// weight[0] is the weight for boneId in ms3d_vertex_t
	// weight[1] is the weight for boneIds[0]
	// weight[2] is the weight for boneIds[1]
	// 1.0f - weight[0] - weight[1] - weight[2] is the weight for boneIds[2]
} ms3d_vertex_ex_t;
//
// nNumVertices * sizeof (ms3d_vertex_ex_t)
//

#include <poppack.h>


//
// Mesh Transformation:
//
// 0. Build the transformation matrices from the rotation and position
// 1. Multiply the vertices by the inverse of local reference matrix (lmatrix0)
// 2. then translate the result by (lmatrix0 * keyFramesTrans)
// 3. then multiply the result by (lmatrix0 * keyFramesRot)
//
// For normals skip step 2.
//
//
//
// NOTE:  this file format may change in future versions!
//
//
// - Mete Ciragan
//
