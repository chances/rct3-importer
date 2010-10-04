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

#include <string.h>
#include <setjmp.h>
#include <stdlib.h>

#include "libASE.h"
#include "symbols.h"

FILE* ASE_file = 0;
char* ASE_buffer = 0;
int ASE_offset = 0;
int ASE_bufsize = 0;

extern char* ASE_text;
extern int ASE_linenum;

jmp_buf ASE_jumpbuf;

#define ASE_error(err) { \
        fprintf(stderr, "(%s:%i) input line %i: %s", __FILE__, __LINE__, ASE_linenum, err); \
        longjmp(ASE_jumpbuf, 1); \
}

int ASE_lex();

int ASE_read(char* buf, int maxsize)
{
    if(ASE_file) {
        return fread(buf, 1, maxsize, ASE_file);
    } else {
        int size = (ASE_bufsize - ASE_offset) < maxsize ? (ASE_bufsize - ASE_offset) : maxsize;
        memcpy(buf, ASE_buffer + ASE_offset, size);
        ASE_offset += size;
        return size;
    }
}

void ASE_readInt(int* i)
{
    if(ASE_lex() != INTEGER) {
        ASE_error("Expected an integer\n");
    }
    sscanf(ASE_text, "%i", i);
}

void ASE_readFloat(float* f)
{
    if(ASE_lex() != FLOAT) {
        ASE_error("Expected a float\n");
    }
    sscanf(ASE_text, "%f", f);
}

void ASE_readInt3(int i[3])
{
    ASE_readInt(&i[0]);
    ASE_readInt(&i[1]);
    ASE_readInt(&i[2]);
}

void ASE_readFloat3(float f[3])
{
    ASE_readFloat(&f[0]);
    ASE_readFloat(&f[1]);
    ASE_readFloat(&f[2]);
}

void ASE_readVector3D(ASE_Vector3D* v)
{
    ASE_readFloat(&(v->x));
    ASE_readFloat(&(v->y));
    ASE_readFloat(&(v->z));
}

void ASE_readRGB(ASE_ColorRGB* c)
{
    ASE_readFloat(&(c->r));
    ASE_readFloat(&(c->g));
    ASE_readFloat(&(c->b));
}

void ASE_readString(char** s)
{
    int len;
    if(ASE_lex() != STRING) {
        ASE_error("Expected a string\n");
    }
    len = strlen(ASE_text);
    *s = (char*)malloc(len - 1);
    memcpy(*s, ASE_text+1, len - 2);
    (*s)[len - 2] = 0;
}

void ASE_readSymbol(char** s)
{
    int len;
    if(ASE_lex() != SYMBOL) {
        ASE_error("Expected a symbol\n");
    }
    len = strlen(ASE_text);
    *s = (char*)malloc(len + 1);
    memcpy(*s, ASE_text, len + 1);
}

void ASE_readIndex()
{
    if(ASE_lex() != INDEX) {
        ASE_error("Expected a index\n");
    }
}


void ASE_readBlockStart()
{
    if(ASE_lex() != BLOCKSTART) {
        ASE_error("Expected an open brace\n");
    }
}

void ASE_readBlockEnd()
{
    if(ASE_lex() != BLOCKEND) {
        ASE_error("Expected a close brace\n");
    }
}

int ASE_skip()
{
    int nextToken;
    int depth = 0;
    char neednext = 1;
    do {
        if(neednext) nextToken = ASE_lex();
        else neednext = 1;
        if(nextToken == BLOCKSTART) depth++;
        if(nextToken == BLOCKEND) if(depth > 0) {
            /*printf("skipping %s\n", ASE_text);*/
            depth--;
            nextToken = ASE_lex();
            neednext = 0;
        }
        /*printf("skipping %s (%i) (depth %i)\n", ASE_text, ASE_linenum, depth);*/
    } while(nextToken && (depth || (nextToken != NODE && nextToken != BLOCKEND)));
    /*printf("done (got %s)\n", ASE_text);*/
    return nextToken;
}


void ASE_loadScene(ASE_Scene* scene)
{
    int nextToken;
    int haveNext = 0;
    char* c;

    ASE_readBlockStart();

    while(haveNext || (nextToken = ASE_lex()) != BLOCKEND) {
        if(nextToken != NODE) {
            ASE_error("Expected a node\n");
        }
        haveNext = 0;

        if(strcmp(ASE_text, "*SCENE_FILENAME") == 0) {
            ASE_readString(&c);
            free(c);
        } else if(strcmp(ASE_text, "*SCENE_FIRSTFRAME") == 0) {
            ASE_readInt(&scene->firstFrame);
        } else if(strcmp(ASE_text, "*SCENE_LASTFRAME") == 0) {
            ASE_readInt(&scene->lastFrame);
        } else if(strcmp(ASE_text, "*SCENE_FRAMESPEED") == 0) {
            ASE_readInt(&scene->frameSpeed);
        } else if(strcmp(ASE_text, "*SCENE_TICKSPERFRAME") == 0) {
            ASE_readInt(&scene->ticksPerFrame);
        } else if(strcmp(ASE_text, "*SCENE_BACKGROUND_STATIC") == 0) {
            ASE_readRGB(&scene->background_light);
        } else if(strcmp(ASE_text, "*SCENE_AMBIENT_STATIC") == 0) {
            ASE_readRGB(&scene->ambient_light);
        } else {
            if((nextToken = ASE_skip()) == BLOCKEND || !nextToken) break;
            haveNext = 1;
        }
    }
}

void ASE_loadMaterialMap(ASE_MaterialMap* map)
{
    int nextToken;
    int haveNext = 0;

    ASE_readBlockStart();

    while(haveNext || (nextToken = ASE_lex()) != BLOCKEND) {
        if(nextToken != NODE) {
            ASE_error("Expected a node\n");
        }
        haveNext = 0;

        if(strcmp(ASE_text, "*MAP_NAME") == 0) {
            ASE_readString(&map->name);
        } else if(strcmp(ASE_text, "*MAP_CLASS") == 0) {
            ASE_readString(&map->mapclass);
        } else if(strcmp(ASE_text, "*MAP_SUBNO") == 0) {
            ASE_readInt(&map->subno);
        } else if(strcmp(ASE_text, "*MAP_AMOUNT") == 0) {
            ASE_readFloat(&map->amount);
        } else if(strcmp(ASE_text, "*BITMAP") == 0) {
            ASE_readString(&map->image_path);
        } else if(strcmp(ASE_text, "*MAP_TYPE") == 0) {
            ASE_readSymbol(&map->type);
        } else if(strcmp(ASE_text, "*UVW_U_OFFSET") == 0) {
            ASE_readFloat(&map->u_offset);
        } else if(strcmp(ASE_text, "*UVW_V_OFFSET") == 0) {
            ASE_readFloat(&map->v_offset);
        } else if(strcmp(ASE_text, "*UVW_U_TILING") == 0) {
            ASE_readFloat(&map->u_tiling);
        } else if(strcmp(ASE_text, "*UVW_V_TILING") == 0) {
            ASE_readFloat(&map->v_tiling);
        } else if(strcmp(ASE_text, "*UVW_ANGLE") == 0) {
            ASE_readFloat(&map->angle);
        } else if(strcmp(ASE_text, "*UVW_BLUR") == 0) {
            ASE_readFloat(&map->blur);
        } else if(strcmp(ASE_text, "*UVW_BLUR_OFFSET") == 0) {
            ASE_readFloat(&map->blur_offset);
        } else if(strcmp(ASE_text, "*UVW_NOUSE_AMT") == 0) {
            ASE_readFloat(&map->noise_amt);
        } else if(strcmp(ASE_text, "*UVW_NOISE_SIZE") == 0) {
            ASE_readFloat(&map->noise_size);
        } else if(strcmp(ASE_text, "*UVW_NOISE_LEVEL") == 0) {
            ASE_readInt(&map->noise_level);
        } else if(strcmp(ASE_text, "*UVW_NOISE_PHASE") == 0) {
            ASE_readFloat(&map->noise_phase);
        } else if(strcmp(ASE_text, "*BITMAP_FILTER") == 0) {
            ASE_readSymbol(&map->bitmap_filter);
        } else {
            if((nextToken = ASE_skip()) == BLOCKEND || !nextToken) break;
            haveNext = 1;
        }
    }
}

void ASE_loadMaterial(ASE_Material* material)
{
    int nextToken;
    int haveNext = 0;

    ASE_readBlockStart();

    while(haveNext || (nextToken = ASE_lex()) != BLOCKEND) {
        if(nextToken != NODE) {
            ASE_error("Expected a node\n");
        }
        haveNext = 0;

        if(strcmp(ASE_text, "*MATERIAL_NAME") == 0) {
            ASE_readString(&material->name);
        } else if(strcmp(ASE_text, "*MATERIAL_CLASS") == 0) {
            ASE_readString(&material->matclass);
        } else if(strcmp(ASE_text, "*MATERIAL_AMBIENT") == 0) {
            ASE_readRGB(&material->ambient);
        } else if(strcmp(ASE_text, "*MATERIAL_DIFFUSE") == 0) {
            ASE_readRGB(&material->diffuse);
        } else if(strcmp(ASE_text, "*MATERIAL_SPECULAR") == 0) {
            ASE_readRGB(&material->specular);
        } else if(strcmp(ASE_text, "*MATERIAL_SHINE") == 0) {
            ASE_readFloat(&material->shine);
        } else if(strcmp(ASE_text, "*MATERIAL_SHINESTRENGTH") == 0) {
            ASE_readFloat(&material->shinestrength);
        } else if(strcmp(ASE_text, "*MATERIAL_TRANSPARENCY") == 0) {
            ASE_readFloat(&material->transparency);
        } else if(strcmp(ASE_text, "*MATERIAL_WIRESIZE") == 0) {
            ASE_readFloat(&material->wiresize);
        } else if(strcmp(ASE_text, "*MATERIAL_SHADING") == 0) {
            ASE_readSymbol(&material->shading);
        } else if(strcmp(ASE_text, "*MATERIAL_XP_FALLOFF") == 0) {
            ASE_readFloat(&material->xp_falloff);
        } else if(strcmp(ASE_text, "*MATERIAL_SELFILLUM") == 0) {
            ASE_readFloat(&material->selfIllum);
        } else if(strcmp(ASE_text, "*MATERIAL_FALLOFF") == 0) {
            ASE_readSymbol(&material->falloff);
        } else if(strcmp(ASE_text, "*MATERIAL_XP_TYPE") == 0) {
            ASE_readSymbol(&material->falloff);
        } else if(strcmp(ASE_text, "*MAP_DIFFUSE") == 0) {
            ASE_loadMaterialMap(&material->diffuseMaterialMap);
        } else if(strcmp(ASE_text, "*NUMSUBMTLS") == 0) {
            ASE_readInt(&material->numsubmaterials);
            material->submaterials = (ASE_Material*)malloc(material->numsubmaterials * sizeof(ASE_Material));
            memset(material->submaterials, 0, material->numsubmaterials * sizeof(ASE_Material));
        } else if(strcmp(ASE_text, "*SUBMATERIAL") == 0) {
            int which;
            ASE_readInt(&which);
            if(which >= material->numsubmaterials) ASE_error("File refered to non-existant submaterial");
            ASE_loadMaterial(&material->submaterials[which]);
        } else {
            if((nextToken = ASE_skip()) == BLOCKEND || !nextToken) break;
            haveNext = 1;
        }
    }
}

void ASE_loadMaterials(ASE_Scene* scene)
{
    int nextToken;
    int haveNext = 0;

    ASE_readBlockStart();

    if(ASE_lex() != NODE) {
        ASE_error("Expected a node\n");
    }
    if(strcmp(ASE_text, "*MATERIAL_COUNT") != 0) {
        ASE_error("Expected a *MATERIAL_COUNT node\n");
    }

    ASE_readInt(&scene->materialCount);

    scene->materials = (ASE_Material*)malloc(sizeof(ASE_Material) * scene->materialCount);
    memset(scene->materials, 0, sizeof(ASE_Material) * scene->materialCount);

    while(haveNext || (nextToken = ASE_lex()) != BLOCKEND) {
        if(nextToken != NODE) {
            ASE_error("Expected a node\n");
        }
        haveNext = 0;

        if(strcmp(ASE_text, "*MATERIAL") == 0) {
            int m;
            ASE_readInt(&m);
            if(m >= scene->materialCount) ASE_error("File referred to a non-existent material.\n");
            ASE_loadMaterial(&scene->materials[m]);
        } else {
            if((nextToken = ASE_skip()) == BLOCKEND || !nextToken) break;
            haveNext = 1;
        }
    }
}

void ASE_loadTransform(ASE_Transform* trans)
{
    int nextToken;
    int haveNext = 0;

    ASE_readBlockStart();

    while(haveNext || (nextToken = ASE_lex()) != BLOCKEND) {
        if(nextToken != NODE) {
            ASE_error("Expected a node\n");
        }
        haveNext = 0;

        if(strcmp(ASE_text, "*NODE_NAME") == 0) {
            ASE_readString(&trans->name);
        } else if(strcmp(ASE_text, "*INHERIT_POS") == 0) {
            ASE_readInt3(trans->inherit_pos);
        } else if(strcmp(ASE_text, "*INHERIT_ROT") == 0) {
            ASE_readInt3(trans->inherit_rot);
        } else if(strcmp(ASE_text, "*INHERIT_SCL") == 0) {
            ASE_readInt3(trans->inherit_scl);
        } else if(strcmp(ASE_text, "*TM_ROW0") == 0) {
            ASE_readFloat3(trans->matrix[0]);
        } else if(strcmp(ASE_text, "*TM_ROW1") == 0) {
            ASE_readFloat3(trans->matrix[1]);
        } else if(strcmp(ASE_text, "*TM_ROW2") == 0) {
            ASE_readFloat3(trans->matrix[2]);
        } else if(strcmp(ASE_text, "*TM_POS") == 0) {
            ASE_readVector3D(&trans->pos);
        } else if(strcmp(ASE_text, "*TM_ROTAXIS") == 0) {
            ASE_readVector3D(&trans->rotaxis);
        } else if(strcmp(ASE_text, "*TM_ROTANGLE") == 0) {
            ASE_readFloat(&trans->rotangle);
        } else if(strcmp(ASE_text, "*TM_SCALE") == 0) {
            ASE_readVector3D(&trans->scale);
        } else if(strcmp(ASE_text, "*TM_SCALEAXIS") == 0) {
            ASE_readVector3D(&trans->scaleaxis);
        } else if(strcmp(ASE_text, "*TM_SCALEAXISANG") == 0) {
            ASE_readFloat(&trans->scaleangle);
        } else {
            if((nextToken = ASE_skip()) == BLOCKEND || !nextToken) break;
            haveNext = 1;
        }
    }
}

void ASE_loadMeshFaces(ASE_Mesh* mesh)
{
    int nextToken;
    int haveNext = 0;
    int f, i;

    ASE_readBlockStart();

    while(haveNext || (nextToken = ASE_lex()) != BLOCKEND) {
        if(nextToken != NODE) {
            ASE_error("Expected a node\n");
        }
        haveNext = 0;

        if(strcmp(ASE_text, "*MESH_FACE") == 0) {
            ASE_readIndex();
            f = atoi(ASE_text);
            if(f >= mesh->faceCount) {
                fprintf(stderr, "File referred to a non-existent face (%i >= %i)\n", f, mesh->faceCount);
                ASE_error("");
            }

            for(i = 0; i < 3; i++) {
                ASE_readIndex();
                ASE_readInt(&mesh->faces[f].vertex[i]);
            }
            ASE_readIndex();
            ASE_readInt(&mesh->faces[f].ab);
            ASE_readIndex();
            ASE_readInt(&mesh->faces[f].bc);
            ASE_readIndex();
            ASE_readInt(&mesh->faces[f].ca);
        }/* else if(strcmp(ASE_text, "*MESH_SMOOTHING") == 0) {
            ASE_readInt(&mesh->faces[f].smoothing);
        } else if(strcmp(ASE_text, "*MESH_MTLID") == 0) {
            ASE_readInt(&mesh->faces[f].mtlid);
        }*/
        else {
            if((nextToken = ASE_skip()) == BLOCKEND || !nextToken) break;
            haveNext = 1;
        }
    }
}

void ASE_loadMeshNormals(ASE_Mesh* mesh)
{
    int nextToken;
    int haveNext = 0;
    int i;

    ASE_readBlockStart();

    while(haveNext || (nextToken = ASE_lex()) != BLOCKEND) {
        if(nextToken != NODE) {
            ASE_error("Expected a node\n");
        }
        haveNext = 0;

        if(strcmp(ASE_text, "*MESH_FACENORMAL") == 0) {
            ASE_readInt(&i);
            if(i >= mesh->faceCount) ASE_error("File referred to a non-existent face.\n");
            ASE_readVector3D(&mesh->faces[i].face_normal);
        } else if(strcmp(ASE_text, "*MESH_VERTEXNORMAL") == 0) {
            int i;
            ASE_readInt(&i);
            if(i >= mesh->vertexCount) ASE_error("File referred to a non-existent vertex.\n");
            ASE_readVector3D(&mesh->vertex_normals[i]);
        } else {
            if((nextToken = ASE_skip()) == BLOCKEND || !nextToken) break;
            haveNext = 1;
        }
    }
}


void ASE_loadMesh(ASE_Mesh* mesh)
{
    int nextToken;
    int haveNext = 0;

    ASE_readBlockStart();

    while(haveNext || (nextToken = ASE_lex()) != BLOCKEND) {
        if(nextToken != NODE) {
            ASE_error("Expected a node\n");
        }
        haveNext = 0;

        if(strcmp(ASE_text, "*TIMEVALUE") == 0) {
            ASE_readInt(&mesh->timevalue);
        } else if(strcmp(ASE_text, "*MESH_NUMVERTEX") == 0) {
            ASE_readInt(&mesh->vertexCount);
            mesh->vertices = (ASE_Vector3D*)malloc(mesh->vertexCount * sizeof(ASE_Vector3D));
            mesh->vertex_normals = (ASE_Vector3D*)malloc(mesh->vertexCount * sizeof(ASE_Vector3D));
        } else if(strcmp(ASE_text, "*MESH_NUMFACES") == 0) {
            ASE_readInt(&mesh->faceCount);
            mesh->faces = (ASE_Face*)malloc(mesh->faceCount * sizeof(ASE_Face));
        } else if(strcmp(ASE_text, "*MESH_VERTEX_LIST") == 0) {
            ASE_readBlockStart();
            while((nextToken = ASE_lex()) != BLOCKEND) {
                int i;
                if(nextToken != NODE || strcmp(ASE_text, "*MESH_VERTEX") != 0){
                    ASE_error("Expected a *MESH_VERTEX node\n");
                }
                ASE_readInt(&i);
                if(i >= mesh->vertexCount) ASE_error("File referred to a non-existent vertex.\n");
                ASE_readVector3D(&mesh->vertices[i]);
            }
        } else if(strcmp(ASE_text, "*MESH_FACE_LIST") == 0) {
            ASE_loadMeshFaces(mesh);
        } else if(strcmp(ASE_text, "*MESH_NUMTVERTEX") == 0) {
            ASE_readInt(&mesh->textureCoordinateCount);
        } else if(strcmp(ASE_text, "*MESH_TVERTLIST") == 0) {
            mesh->texture_coordinates = (ASE_Vector3D*)malloc(mesh->textureCoordinateCount * sizeof(ASE_Vector3D));
            ASE_readBlockStart();
            while((nextToken = ASE_lex()) != BLOCKEND) {
                int i;
                if(nextToken != NODE || strcmp(ASE_text, "*MESH_TVERT") != 0){
                    ASE_error("Expected a *MESH_TVERT node\n");
                }
                ASE_readInt(&i);
                if(i >= mesh->textureCoordinateCount) ASE_error("File referred to a non-existent vertex.\n");
                ASE_readVector3D(&mesh->texture_coordinates[i]);
            }
        } else if(strcmp(ASE_text, "*MESH_NUMTVFACES") == 0) {
            int i;
            ASE_readInt(&i);
            if(i != mesh->faceCount) {
                ASE_error("Expected face texture count does not match face count\n");
            }
        } else if(strcmp(ASE_text, "*MESH_TFACELIST") == 0) {
            ASE_readBlockStart();
            while((nextToken = ASE_lex()) != BLOCKEND) {
                int i;
                if(nextToken != NODE || strcmp(ASE_text, "*MESH_TFACE") != 0){
                    ASE_error("Expected a *MESH_TFACE node\n");
                }
                ASE_readInt(&i);
                if(i >= mesh->faceCount) ASE_error("File referred to a non-existent face.\n");
                ASE_readInt3(mesh->faces[i].texture_coordinates);
            }
        } else if(strcmp(ASE_text, "*MESH_NORMALS") == 0) {
            ASE_loadMeshNormals(mesh);
        } else {
            if((nextToken = ASE_skip()) == BLOCKEND || !nextToken) break;
            haveNext = 1;
        }
    }
}

void ASE_loadGeomObject(ASE_Scene* scene)
{
    ASE_GeomObject* obj;
    int nextToken;
    int haveNext = 0;

    scene->objectCount++;
    scene->objs = (ASE_GeomObject*)realloc(scene->objs, sizeof(ASE_GeomObject) * scene->objectCount);
    obj = &scene->objs[scene->objectCount-1];
	memset(obj, 0, sizeof(ASE_GeomObject));

    ASE_readBlockStart();

    while(haveNext || (nextToken = ASE_lex()) != BLOCKEND) {
        if(nextToken != NODE) {
            ASE_error("Expected a node\n");
        }
        haveNext = 0;

        if(strcmp(ASE_text, "*NODE_NAME") == 0) {
            ASE_readString(&obj->name);
        } else if(strcmp(ASE_text, "*NODE_TM") == 0) {
            ASE_loadTransform(&obj->transform);
        } else if(strcmp(ASE_text, "*MESH") == 0) {
            ASE_loadMesh(&obj->mesh);
        } else if(strcmp(ASE_text, "*PROP_MOTIONBLUR") == 0) {
            ASE_readInt(&obj->motionblur);
        } else if(strcmp(ASE_text, "*PROP_CASTSHADOW") == 0) {
            ASE_readInt(&obj->castShadow);
        } else if(strcmp(ASE_text, "*PROP_RECVSHADOW") == 0) {
            ASE_readInt(&obj->receiveShadow);
        } else if(strcmp(ASE_text, "*TM_ANIMATION") == 0) {
            ASE_skip();
        } else if(strcmp(ASE_text, "*MATERIAL_REF") == 0) {
            ASE_readInt(&obj->material);
        } else {
            if((nextToken = ASE_skip()) == BLOCKEND || !nextToken) break;
            haveNext = 1;
        }
    }
}

void ASE_loadLightSettings(ASE_LightSettings* settings)
{
    int nextToken;
    int haveNext = 0;

    ASE_readBlockStart();

    while(haveNext || (nextToken = ASE_lex()) != BLOCKEND) {
        if(nextToken != NODE) {
            ASE_error("Expected a node\n");
        }
        haveNext = 0;

        if(strcmp(ASE_text, "*TIMEVALUE") == 0) {
            ASE_readInt(&settings->timevalue);
        }  else if(strcmp(ASE_text, "*LIGHT_COLOR") == 0) {
            ASE_readRGB(&settings->color);
        }  else if(strcmp(ASE_text, "*LIGHT_INTENS") == 0) {
            ASE_readFloat(&settings->intensity);
        }  else if(strcmp(ASE_text, "*LIGHT_ASPECT") == 0) {
            ASE_readFloat(&settings->aspect);
        }  else if(strcmp(ASE_text, "*LIGHT_TDIST") == 0) {
            ASE_readFloat(&settings->tdist);
        }  else if(strcmp(ASE_text, "*LIGHT_MAPBIAS") == 0) {
            ASE_readFloat(&settings->mapBias);
        }  else if(strcmp(ASE_text, "*LIGHT_MAPRANGE") == 0) {
            ASE_readFloat(&settings->mapRange);
        }  else if(strcmp(ASE_text, "*LIGHT_MAPSIZE") == 0) {
            ASE_readInt(&settings->mapSize);
        }  else if(strcmp(ASE_text, "*LIGHT_RAYBIAS") == 0) {
            ASE_readFloat(&settings->rayBias);
        } else {
            if((nextToken = ASE_skip()) == BLOCKEND || !nextToken) break;
            haveNext = 1;
        }
    }
}

void ASE_loadLight(ASE_Scene* scene)
{
    ASE_Light* light;
    int nextToken;
    int haveNext = 0;

    scene->lightCount++;
    scene->lights = (ASE_Light*)realloc(scene->lights, sizeof(ASE_Light) * scene->lightCount);
    light = &scene->lights[scene->lightCount-1];

    ASE_readBlockStart();

    while(haveNext || (nextToken = ASE_lex()) != BLOCKEND) {
        if(nextToken != NODE) {
            ASE_error("Expected a node\n");
        }
        haveNext = 0;

        if(strcmp(ASE_text, "*NODE_NAME") == 0) {
            ASE_readString(&light->name);
        } else if(strcmp(ASE_text, "*LIGHT_TYPE") == 0) {
            ASE_readSymbol(&light->name);
        } else if(strcmp(ASE_text, "*NODE_TM") == 0) {
            ASE_loadTransform(&light->transform);
        } else if(strcmp(ASE_text, "*LIGHT_SHADOWS") == 0) {
            ASE_readSymbol(&light->shadows);
        } else if(strcmp(ASE_text, "*LIGHT_USELIGHT") == 0) {
            ASE_readInt(&light->uselight);
        } else if(strcmp(ASE_text, "*LIGHT_SPOTSHAPE") == 0) {
            ASE_readSymbol(&light->spotshape);
        } else if(strcmp(ASE_text, "*LIGHT_USEGLOBAL") == 0) {
            ASE_readInt(&light->useglobal);
        } else if(strcmp(ASE_text, "*LIGHT_ABSMAPBIAS") == 0) {
            ASE_readInt(&light->absMapBias);
        } else if(strcmp(ASE_text, "*LIGHT_OVERSHOOT") == 0) {
            ASE_readInt(&light->overshoot);
        } else if(strcmp(ASE_text, "*LIGHT_SETTINGS") == 0) {
            ASE_loadLightSettings(&light->settings);
        } else {
            if((nextToken = ASE_skip()) == BLOCKEND || !nextToken) break;
            haveNext = 1;
        }
    }
}

void ASE_loadCameraSettings(ASE_CameraSettings* settings)
{
    int nextToken;
    int haveNext = 0;

    ASE_readBlockStart();

    while(haveNext || (nextToken = ASE_lex()) != BLOCKEND) {
        if(nextToken != NODE) {
            ASE_error("Expected a node\n");
        }
        haveNext = 0;

        if(strcmp(ASE_text, "*TIMEVALUE") == 0) {
            ASE_readInt(&settings->timevalue);
        } else if(strcmp(ASE_text, "*CAMERA_NEAR") == 0) {
            ASE_readFloat(&settings->near_val);
        } else if(strcmp(ASE_text, "*CAMERA_FAR") == 0) {
            ASE_readFloat(&settings->far_val);
        } else if(strcmp(ASE_text, "*CAMERA_FOV") == 0) {
            ASE_readFloat(&settings->fov);
        } else if(strcmp(ASE_text, "*CAMERA_TDIST") == 0) {
            ASE_readFloat(&settings->tdist);
        } else {
            if((nextToken = ASE_skip()) == BLOCKEND || !nextToken) break;
            haveNext = 1;
        }
    }
}

void ASE_loadCamera(ASE_Scene* scene)
{
    int nextToken;
    int haveNext = 0;
    ASE_Camera* camera;

    scene->cameraCount++;
    scene->cameras = (ASE_Camera*)realloc(scene->cameras, sizeof(ASE_Camera) * scene->cameraCount);
    camera = &scene->cameras[scene->cameraCount-1];

    ASE_readBlockStart();

    while(haveNext || (nextToken = ASE_lex()) != BLOCKEND) {
        if(nextToken != NODE) {
            ASE_error("Expected a node\n");
        }
        haveNext = 0;

        if(strcmp(ASE_text, "*NODE_NAME") == 0) {
            ASE_readString(&camera->name);
        } else if(strcmp(ASE_text, "*CAMERA_TYPE") == 0) {
            ASE_readSymbol(&camera->type);
            if(strcmp(camera->type, "Target") == 0) {
                ASE_lex();
                ASE_loadTransform(&camera->camera);
                ASE_lex();
                ASE_loadTransform(&camera->target);
            }
        } else if(strcmp(ASE_text, "*CAMERA_SETTINGS") == 0) {
            ASE_loadCameraSettings(&camera->settings);
        } else {
            if((nextToken = ASE_skip()) == BLOCKEND || !nextToken) break;
            haveNext = 1;
        }
    }
}

/* Template for all the loadFoo functions */
/*
void ASE_loadXXX(ASE_XXX* x)
{
    int nextToken;
    int haveNext = 0;

    ASE_readBlockStart();

    while(haveNext || (nextToken = ASE_lex()) != BLOCKEND) {
        if(nextToken != NODE) {
            ASE_error("Expected a node\n");
        }
        haveNext = 0;

        if() {
        } else {
            if((nextToken = ASE_skip()) == BLOCKEND) break;
            haveNext = 1;
        }
    }
}
*/

ASE_Scene* ASE_load()
{
    int version;
    int nextToken;
    int haveNext = 0;

    ASE_Scene* scene = (ASE_Scene*)malloc(sizeof(ASE_Scene));
    memset(scene, 0, sizeof(ASE_Scene));

    if(setjmp(ASE_jumpbuf)) {
        fprintf(stderr, "There was an error\n");
        return 0;
    }

    if(ASE_lex() != NODE) {
        ASE_error("Expected a node\n");
    }
    if(strcmp(ASE_text, "*3DSMAX_ASCIIEXPORT") != 0) {
        ASE_error("File must start with *3DSMAX_ASCIIEXPORT\n");
    }

    ASE_readInt(&version);
    if(version != 200) {
        fprintf(stderr, "Warning: loader has not been tested with this file version (%i)\n", version);
    }


    while((nextToken = ASE_lex())) {
        if(nextToken != NODE) {
            ASE_error("Expected a node\n");
        }

        if(strcmp(ASE_text, "*COMMENT") == 0) {
            char *c;
            ASE_readString(&c);
            free(c);
        } else if(strcmp(ASE_text, "*SCENE") == 0) {
            ASE_loadScene(scene);
        } else if(strcmp(ASE_text, "*MATERIAL_LIST") == 0) {
            ASE_loadMaterials(scene);
        } else if(strcmp(ASE_text, "*GEOMOBJECT") == 0) {
            ASE_loadGeomObject(scene);
        } else if(strcmp(ASE_text, "*LIGHTOBJECT") == 0) {
            ASE_loadLight(scene);
        } else if(strcmp(ASE_text, "*CAMERAOBJECT") == 0) {
            ASE_loadCamera(scene);
        } else {
            if((nextToken = ASE_skip()) == BLOCKEND) break;
            haveNext = 1;
        }
    }

    return scene;
}

void ASE_freeMaterial(ASE_Material* mat)
{
    free(mat->name);
    free(mat->matclass);
    free(mat->shading);
    free(mat->falloff);
    free(mat->xp_type);
    free(mat->diffuseMaterialMap.name);
    free(mat->diffuseMaterialMap.mapclass);
    free(mat->diffuseMaterialMap.image_path);
    free(mat->diffuseMaterialMap.type);
    free(mat->diffuseMaterialMap.bitmap_filter);
	int i;
    for (i = 0; i < mat->numsubmaterials; i++)
        ASE_freeMaterial(&mat->submaterials[i]);
    free(mat->submaterials);
}

LIBASE_API void ASE_freeScene(ASE_Scene* scene)
{
    int i;
    for (i = 0; i < scene->objectCount; i++) {
        free(scene->objs[i].name);
        free(scene->objs[i].transform.name);
        free(scene->objs[i].mesh.vertices);
        free(scene->objs[i].mesh.vertex_normals);
        free(scene->objs[i].mesh.texture_coordinates);
        free(scene->objs[i].mesh.faces);
    }
    free(scene->objs);
    for (i = 0; i < scene->lightCount; i++) {
        free(scene->lights[i].name);
        free(scene->lights[i].transform.name);
        free(scene->lights[i].type);
        free(scene->lights[i].shadows);
        free(scene->lights[i].spotshape);
    }
    free(scene->lights);
    for (i = 0; i < scene->materialCount; i++)
        ASE_freeMaterial(&scene->materials[i]);
    free(scene->materials);
    for (i = 0; i < scene->cameraCount; i++) {
        free(scene->cameras[i].name);
        free(scene->cameras[i].type);
        free(scene->cameras[i].camera.name);
        free(scene->cameras[i].target.name);
    }
    free(scene->cameras);
    free(scene);
}

LIBASE_API ASE_Scene* ASE_loadFile(FILE* file)
{
    ASE_Scene* ret;

    ASE_file = file;
    ret = ASE_load();
    ASE_file = 0;

    return ret;
}

LIBASE_API ASE_Scene* ASE_loadFilename(const char* filename)
{
    ASE_Scene* ret;

    if((ASE_file = fopen(filename, "r")) == NULL) {
        perror("Could not open ASE file");
        return 0;
    }

    ret = ASE_load();

    fclose(ASE_file);
    ASE_file = 0;

    return ret;
}

LIBASE_API ASE_Scene* ASE_loadBuffer(char* buffer, int size)
{
    ASE_Scene* ret;

    ASE_buffer = buffer;
    ASE_offset = 0;
    ASE_bufsize = size;

    ret = ASE_load();

    ASE_buffer = 0;
    ASE_offset = 0;
    ASE_bufsize = 0;

    return ret;
}

