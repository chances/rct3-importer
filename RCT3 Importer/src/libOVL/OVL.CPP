/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#include "Common.h"
#include "CommonOVL.h"
#include "OVL.h"

#include <list>
#include <map>

#define DUMP_RELOCATIONS

#ifdef DUMP_RELOCATIONS
#define DUMP_RELOCATION( ... ) fprintf( __VA_ARGS__ )
#else
#define DUMP_RELOCATION( ... )
#endif

FILE *f;
extern unsigned long CurrentFile;
extern OvlFile OpenFiles[2];
std::queue<unsigned long *> relocations;
std::queue<unsigned long> fixups;
std::vector <libOVL_Resource *> ResourceItems;
// Static shapes and textures
std::vector <StaticShape1 *> StaticShapes;
std::vector <char *> StaticShapeNames;
std::vector <char **> StaticShapeFTXNames;
std::vector <char **> StaticShapeTXSNames;
// Bone shapes and textures
std::vector <BoneShape1 *> BoneShapes;
std::vector <BoneAnim *> BoneAnims;
std::vector <char *> BoneShapeNames;
std::vector <char *> BoneAnimNames;
std::vector <char **> BoneShapeFTXNames;
std::vector <char **> BoneShapeTXSNames;

std::vector <FlexiTextureInfoStruct *> FlexiTextures;
std::vector <char *> FlexiTextureNames;
SceneryItemVisual *SVD;
char *SVDName;
char **SHSNames;
char **BSHNames;
char **BANNames;
// ++++ Need bone and anim version of the above.
unsigned long CurrentStaticShape = 0;
char **LoaderNames=NULL;
char ObjectName[50]="";
char FileName[50]="";

File *Type0Files;
File *Type2Files;

//Scenary items
SymbolStruct *Sym;
LoaderStruct *Ldr;
SymbolRefStruct *Ref;

unsigned char *svd;
SceneryItemVisual *sv;
SceneryItemVisualLOD **svlod;
SceneryItemVisualLOD *svlod1;
SceneryItemVisualLOD *svlod2;
SceneryItemVisualLOD *svlod3;
unsigned char *shape;
StaticShape1 *sh;
StaticShape2 **sh2x;
StaticShape2 *sh2;
VERTEX *Vertexes;
unsigned long *Triangles;

char *names;
char *objname;
char *texturename;
char *shapename;
char *sceneryname;
char *txsname;

//Functions
unsigned long saveResourceData();
void saveScenaryResourceData();
unsigned long saveScenaryData(unsigned long);
void clearOVLData();
int compare(const void *s1,const void *s2);


////////////////////////////////////////////////////////////////////////////////
//
//  CreateSceneryOvl
//  Used
//
////////////////////////////////////////////////////////////////////////////////

//Filename - .common.ovl
LIBOVL_API void CreateSceneryOvl(char *filename) {
    char *UniqueOVLFilename = new char[strlen(filename)+1];
    strcpy(UniqueOVLFilename,filename);
    strcpy(strstr(UniqueOVLFilename,".common.ovl"),".unique.ovl");
    strcpy(filename,UniqueOVLFilename);
    strcpy(strstr(filename,".unique.ovl"),".common.ovl");
    OpenFiles[OVL_COMMON].FileName= new char[strlen(filename)+1];
    strcpy(OpenFiles[OVL_COMMON].FileName , filename);
    OpenFiles[OVL_UNIQUE].FileName = UniqueOVLFilename;
    OpenFiles[OVL_COMMON].ReferenceCount=0;
    OpenFiles[OVL_COMMON].References=NULL;
    memset(OpenFiles[OVL_COMMON].Types,0,sizeof(OpenFiles[OVL_COMMON].Types));
    memset(OpenFiles[OVL_UNIQUE].Types,0,sizeof(OpenFiles[OVL_UNIQUE].Types));
    StaticShapes.clear();
    StaticShapeNames.clear();
    StaticShapeFTXNames.clear();
    StaticShapeTXSNames.clear();
    BoneShapes.clear();
    BoneAnims.clear();
    BoneShapeNames.clear();
    BoneShapeFTXNames.clear();
    BoneShapeTXSNames.clear();
    FlexiTextures.clear();
    FlexiTextureNames.clear();
    References.clear();
    SVD = 0;
    SVDName = 0;
    SHSNames = 0;
    BSHNames = 0;
    BANNames = 0;
    Sym = 0;
    Ldr = 0;
    Ref = 0;
}


////////////////////////////////////////////////////////////////////////////////
//
//  AddStaticShape
//  Used
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void AddStaticShape(char *StaticShapeName, char **FTXNames, char **TXSNames, StaticShape1 *sh) {
    StaticShapeNames.push_back(StaticShapeName);
    StaticShapeFTXNames.push_back(FTXNames);
    StaticShapeTXSNames.push_back(TXSNames);
    StaticShapes.push_back(sh);
}

LIBOVL_API void AddBoneShape(char *BoneShapeName, char **FTXNames, char **TXSNames, BoneShape1 *bsh) {
    BoneShapeNames.push_back(BoneShapeName);
    BoneShapeFTXNames.push_back(FTXNames);
    BoneShapeTXSNames.push_back(TXSNames);
    BoneShapes.push_back(bsh);
}

LIBOVL_API void AddBoneAnim(char *BoneAnimName, BoneAnim *ban) {
    BoneAnimNames.push_back(BoneAnimName);
    BoneAnims.push_back(ban);
}

////////////////////////////////////////////////////////////////////////////////
//
//  AddSceneryItemVisual
//  Used
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void AddSceneryItemVisual(char *SceneryItemVisualName, char **StaticShapeNames, char **BoneShapeNames, char **BoneAnimNames, SceneryItemVisual *svd) {
    SVDName = SceneryItemVisualName;
    SVD = svd;
    SHSNames = StaticShapeNames;
    BSHNames = BoneShapeNames;
    BANNames = BoneAnimNames;
}


////////////////////////////////////////////////////////////////////////////////
//
//  AddFlexiTexture
//  Used
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void AddFlexiTexture(char *FlexiTextureName, FlexiTextureInfoStruct *fti) {
    FlexiTextureNames.push_back(FlexiTextureName);
    FlexiTextures.push_back(fti);
}


////////////////////////////////////////////////////////////////////////////////
//
//  AddReference
//  Used
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void AddReference(char *reference) {
    References.push_back(reference);
}


////////////////////////////////////////////////////////////////////////////////
//
//  ovlStringTable class
//  Used
//
////////////////////////////////////////////////////////////////////////////////

class ovlStringTable {
public:
    static const char* SVD;
    static const char* FTX;
    static const char* TXS;
    static const char* SHS;
    static const char* BSH;
    static const char* BAN;
private:
    std::list<std::string> m_strings;
    std::map<std::string, char*> m_ptrmap;
    int m_size;
    char* m_table;

    char* FindRawString(const std::string& findit) const {
        std::map<std::string, char*>::const_iterator fst = m_ptrmap.find(findit);
        if (fst == m_ptrmap.end()) {
            DUMP_RELOCATION(f, "Find '%s' failed\n", findit.c_str());
            return NULL;
        }
        DUMP_RELOCATION(f, "Find '%s': %s (%x/%x)\n", findit.c_str(), fst->second, fst->second, m_table);
        return fst->second;
    }
#ifdef DUMP_RELOCATIONS
    FILE *f;
#endif
public:
#ifdef DUMP_RELOCATIONS
    ovlStringTable(FILE* fi) {
        m_size = 0;
        m_table = NULL;
        f = fi;
    }
#else
    ovlStringTable() {
        m_size = 0;
        m_table = NULL;
    }
#endif
    ~ovlStringTable() {
        if (m_table)
            delete[] m_table;
    }

    void AddString(const char *lstring) {
        std::string st = lstring;
        m_strings.push_back(st);
        DUMP_RELOCATION(f, "Add String '%s'\n", lstring);
    }
    void AddSymbolString(const char *lstring, const char *lextension) {
        std::string st = lstring;
        st += ':';
        st += lextension;
        m_strings.push_back(st);
        DUMP_RELOCATION(f, "Add Symbol String '%s':'%s'\n", lstring, lextension);
    }
    char* Make() {
        char* tempstr;
        if (m_table)
            return m_table;
        m_strings.sort();
        m_strings.unique();
        for (std::list<std::string>::iterator it = m_strings.begin(); it != m_strings.end(); ++it) {
            m_size += it->length() + 1;
        }
        m_table = new char[m_size];
        tempstr = m_table;
        for (std::list<std::string>::iterator it = m_strings.begin(); it != m_strings.end(); ++it) {
            strcpy(tempstr, it->c_str());
            m_ptrmap[*it] = tempstr;
            tempstr += it->length() + 1;
        }
        return m_table;
    }
    char* FindString(const char *lstring) const {
        std::string st = lstring;
        return FindRawString(st);
    }
    char* FindSymbolString(const char *lstring, const char *lextension) const {
        std::string st = lstring;
        st += ':';
        st += lextension;
        return FindRawString(st);
    }
    int GetSize() const {
        return m_size;
    }

};

const char* ovlStringTable::SVD = "svd";
const char* ovlStringTable::FTX = "ftx";
const char* ovlStringTable::TXS = "txs";
const char* ovlStringTable::SHS = "shs";
const char* ovlStringTable::BSH = "bsh";
const char* ovlStringTable::BAN = "ban";

////////////////////////////////////////////////////////////////////////////////
//
//  SaveSceneryOvl
//  Used
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void SaveSceneryOvl() {
#ifdef DUMP_RELOCATIONS
    f = fopen("relocation.log","w");
#endif
    CurrentFile = OVL_UNIQUE;
//    unsigned long StringTableSize = 0;
#ifdef DUMP_RELOCATIONS
    ovlStringTable stringtable(f);
#else
    ovlStringTable stringtable;
#endif
    unsigned long SVDSize = 0;
    unsigned long StaticShapeSize = 0;
    unsigned long BoneShapeSize = 0;
    unsigned long BoneAnimSize = 0;
    unsigned long FlexiTextureSize = 0;
    unsigned long SymbolCount = 0;
    unsigned long LoaderCount = 0;
    unsigned long SymbolRefCount = 0;
    unsigned long i;
    unsigned long j;

    if (SVD) {
        SVDSize += sizeof(SceneryItemVisual);
//        StringTableSize += strlen(SVDName) + 5;
        stringtable.AddSymbolString(SVDName, ovlStringTable::SVD);
        SymbolCount++;
        LoaderCount++;
        for (i = 0;i < SVD->LODCount;i++) {
            SVDSize += 4;
            SVDSize += sizeof(SceneryItemVisualLOD);
            SVDSize += 8*SVD->LODMeshes[i]->AnimationCount;
            if (SHSNames) {
                SymbolRefCount++;
//                StringTableSize += strlen(SHSNames[i]) + 1;
                stringtable.AddString(SHSNames[i]);
            }
            if (BSHNames) {
                SymbolRefCount++;
//                StringTableSize += strlen(BSHNames[i]) + 1;
                stringtable.AddString(BSHNames[i]);
            }
            if (BANNames) {
                SymbolRefCount++;
                //StringTableSize += strlen(BANNames[i]) + 1;
                stringtable.AddString(BANNames[i]);
            }
        }
    }
    for (i = 0;i < FlexiTextures.size();i++) {
        SymbolCount++;
        LoaderCount++;
        // InfoStruct
        FlexiTextureSize += sizeof(FlexiTextureInfoStruct);
        // Offsets
        FlexiTextureSize += 4 * FlexiTextures[i]->offsetCount;
        // Textures
        FlexiTextureSize += sizeof(FlexiTextureStruct) * FlexiTextures[i]->fts2Count;

        // Probably all textures should be the same size, but let's be exact
        for(unsigned long m = 0; m < FlexiTextures[i]->fts2Count; m++) {
            FlexiTextureSize += 256*sizeof(RGBQUAD);
            FlexiTextureSize += FlexiTextures[i]->fts2[m].width*FlexiTextures[i]->fts2[m].height;
            if (FlexiTextures[i]->fts2[m].alpha != 0) {
                FlexiTextureSize += FlexiTextures[i]->fts2[m].width*FlexiTextures[i]->fts2[m].height;
            }
        }

        if (!SVD) {
//            SymbolRefCount++;
//            StringTableSize += strlen(FlexiTextureNames[i])+5;
              stringtable.AddSymbolString(FlexiTextureNames[i], ovlStringTable::FTX);
        }
    }
    for (i = 0;i < StaticShapes.size();i++) {
//        StringTableSize += strlen(StaticShapeNames[i])+5;
        stringtable.AddSymbolString(StaticShapeNames[i], ovlStringTable::SHS);
        StaticShapeSize += sizeof(StaticShape1);
        LoaderCount++;
        SymbolCount++;
        for (j = 0;j < StaticShapes[i]->MeshCount;j++) {
            StaticShape2 *obj = StaticShapes[i]->sh[j];
            StaticShapeSize += sizeof(StaticShape2) + (obj->VertexCount * sizeof(VERTEX)) + (obj->IndexCount * 4) + 4;
            SymbolRefCount++;
            SymbolRefCount++;
//            StringTableSize += strlen(StaticShapeFTXNames[i][j])+5;
            stringtable.AddSymbolString(StaticShapeFTXNames[i][j], ovlStringTable::FTX);
//            StringTableSize += strlen(StaticShapeTXSNames[i][j])+5;
            stringtable.AddSymbolString(StaticShapeTXSNames[i][j], ovlStringTable::TXS);
        }
        for (j = 0;j < StaticShapes[i]->EffectCount;j++) {
            StaticShapeSize += 4;
            StaticShapeSize += sizeof(D3DMATRIX);
            unsigned long x = strlen(StaticShapes[i]->EffectName[j]) + 1;
            while (x % 4 != 0) {
                x++;
            }
            StaticShapeSize += x;
        }
    }
    // ++++ Iterate through the BoneShapes array (vis StaticShapes above)
    //      increment symbol and loader by 2, for the bsh and ban files.
    //      determine bsh and ban size, from relevant mesh and index information.
    for (i = 0;i < BoneShapes.size();i++) {
//        StringTableSize += strlen(BoneShapeNames[i])+5;
        stringtable.AddSymbolString(BoneShapeNames[i], ovlStringTable::BSH);
        BoneShapeSize += sizeof(BoneShape1);
        LoaderCount++;
        SymbolCount++;
        for (j = 0;j < BoneShapes[i]->MeshCount;j++) {
            BoneShape2 *obj = BoneShapes[i]->sh[j];
            BoneShapeSize += sizeof(BoneShape2) + (obj->VertexCount * sizeof(VERTEX2)) + (obj->IndexCount * 4) + 4;
            SymbolRefCount++;
            SymbolRefCount++;
//            StringTableSize += strlen(BoneShapeFTXNames[i][j])+5;
            stringtable.AddSymbolString(BoneShapeFTXNames[i][j], ovlStringTable::FTX);
//            StringTableSize += strlen(BoneShapeTXSNames[i][j])+5;
            stringtable.AddSymbolString(BoneShapeTXSNames[i][j], ovlStringTable::TXS);
        }
        for (j = 0;j < BoneShapes[i]->BoneCount;j++) {
//            BoneShapeSize += 4;  // No need, name pointer is part of BoneStruct
            BoneShapeSize += 2*sizeof(D3DMATRIX);
            BoneShapeSize += sizeof(BoneStruct);
            stringtable.AddString(BoneShapes[i]->Bones[j].BoneName);
//            unsigned long x = strlen(BoneShapes[i]->Bones[j].BoneName) + 1;
//            while (x % 4 != 0) {
//                x++;
//            }
//            BoneShapeSize += x;
        }
    }

    for (i = 0;i < BoneAnims.size();i++) {
//        StringTableSize += strlen(BoneAnimNames[i])+5;
        stringtable.AddSymbolString(BoneAnimNames[i], ovlStringTable::BAN);
        BoneAnimSize += sizeof(BoneAnim);
        LoaderCount++;
        SymbolCount++;
        for (j = 0;j < BoneAnims[i]->BoneCount;j++) {
            BoneAnimBone *obj = &BoneAnims[i]->Bones[j];
            BoneAnimSize += sizeof(BoneAnimBone) + ((obj->TranslateCount+obj->RotateCount) * sizeof(txyz)) + 4;
            stringtable.AddString(BoneAnims[i]->Bones[j].Name);

//            unsigned long x = strlen(BoneAnims[i]->Bones[j].Name) + 1;
//            while (x % 4 != 0) {
//                x++;
//            }
//            BoneAnimSize += x;
        }
    }
//    char *StringTable = new char[StringTableSize];
//    char *TempStringTable = StringTable;
    stringtable.Make();
    unsigned char *SVDx = new unsigned char[SVDSize];
    unsigned char *TempSVD = SVDx;
    unsigned char *StaticShape = new unsigned char[StaticShapeSize];
    unsigned char *TempStaticShape = StaticShape;
    unsigned char *BoneShape = new unsigned char[BoneShapeSize];
    unsigned char *TempBoneShape = BoneShape;
    unsigned char *BoneAnimx = new unsigned char[BoneAnimSize];
    unsigned char *TempBoneAnim = BoneAnimx;
    unsigned char *FlexiTexture = new unsigned char[FlexiTextureSize];
    unsigned char *TempFlexiTexture = FlexiTexture;
    // Create various reference structures using counts determined above.
    Ldr = new LoaderStruct[LoaderCount];
    Sym = new SymbolStruct[SymbolCount];
    Ref = new SymbolRefStruct[SymbolRefCount];
    // Setup pointers to the above arrays.
    LoaderStruct *TempLdr = Ldr;
    SymbolStruct *TempSym = Sym;
    SymbolRefStruct *TempRef = Ref;
    // Iterate over the static shapes
    for (i = 0;i < StaticShapes.size();i++) {
//        strcpy(TempStringTable,StaticShapeNames[i]);
//        char *shsname = TempStringTable;
//        TempStringTable += strlen(StaticShapeNames[i]);
//        strcpy(TempStringTable,":shs");
//        TempStringTable += 5;
        char *shsname = stringtable.FindSymbolString(StaticShapeNames[i], ovlStringTable::SHS);
        StaticShape1 *shs = (StaticShape1 *)TempStaticShape;
        memcpy(shs,StaticShapes[i],sizeof(StaticShape1));
        TempStaticShape += sizeof(StaticShape1);
        shs->sh = (StaticShape2 **)TempStaticShape;
        relocations.push((unsigned long *)&shs->sh);
        DUMP_RELOCATION(f,"relocations: %x: &shs->sh\n",&shs->sh);
        TempStaticShape += shs->MeshCount * 4;
        TempSym->data = (unsigned long *)shs;
        TempSym->IsPointer = true;
        TempSym->Symbol = shsname;
        TempLdr->data = (unsigned long *)shs;
        relocations.push((unsigned long *)&TempLdr->data);
        DUMP_RELOCATION(f,"relocations: %x: &TempLdr->data\n",&TempLdr->data);
        TempLdr->HasExtraData = false;
        // This will vary dependent upon the types within the ovl
        TempLdr->LoaderType = 2;
        TempLdr->Sym = TempSym;
        relocations.push((unsigned long *)&TempLdr->Sym);
        DUMP_RELOCATION(f,"relocations: %x: &TempLdr->Sym\n",&TempLdr->Sym);
        TempLdr->SymbolsToResolve = 0;
        for (unsigned long j = 0;j < StaticShapes[i]->MeshCount;j++) {
            StaticShape2 *obj = StaticShapes[i]->sh[j];
            StaticShape2 *sh2 = (StaticShape2 *)TempStaticShape;
            TempStaticShape += sizeof(StaticShape2);
            memcpy(sh2,obj,sizeof(StaticShape2));
            shs->sh[j] = sh2;
            relocations.push((unsigned long *)&shs->sh[j]);
            DUMP_RELOCATION(f,"relocations: %x: &shs->sh[j]\n",&shs->sh[j]);
            sh2->Vertexes = (VERTEX *)TempStaticShape;
            memcpy(sh2->Vertexes,obj->Vertexes,obj->VertexCount*sizeof(VERTEX));
            relocations.push((unsigned long *)&sh2->Vertexes);
            DUMP_RELOCATION(f,"relocations: %x: &sh2->Vertexes\n",&sh2->Vertexes);
            TempStaticShape += obj->VertexCount*sizeof(VERTEX);
            sh2->Triangles = (unsigned long *)TempStaticShape;
            memcpy(sh2->Triangles,obj->Triangles,obj->IndexCount*4);
            TempStaticShape += obj->IndexCount*4;
            relocations.push((unsigned long *)&sh2->Triangles);
            DUMP_RELOCATION(f,"relocations: %x: &sh2->Triangles\n",&sh2->Triangles);

            TempLdr->SymbolsToResolve += 2;
            TempRef->ldr = TempLdr;
            relocations.push((unsigned long *)&TempRef->ldr);
            DUMP_RELOCATION(f,"relocations: %x: &TempRef->ldr, StaticShape:ftx\n",&TempRef->ldr);
            TempRef->reference = (unsigned long *)&sh2->fts;
            relocations.push((unsigned long *)&TempRef->reference);
            DUMP_RELOCATION(f,"relocations: %x: &TempRef->reference, StaticShape:ftx\n",&TempRef->reference);
//            char *FTX = TempStringTable;
//            strcpy(TempStringTable,StaticShapeFTXNames[i][j]);
//            TempStringTable += strlen(StaticShapeFTXNames[i][j]);
//            strcpy(TempStringTable,":ftx");
//            TempStringTable += 5;
            char *FTX = stringtable.FindSymbolString(StaticShapeFTXNames[i][j], ovlStringTable::FTX);
            TempRef->Symbol = FTX;
            relocations.push((unsigned long *)&TempRef->Symbol);
            DUMP_RELOCATION(f,"relocations: %x: &TempRef->Symbol, StaticShape:ftx\n",&TempRef->Symbol);
            TempRef++;
            TempRef->ldr = TempLdr;
            relocations.push((unsigned long *)&TempRef->ldr);
            DUMP_RELOCATION(f,"relocations: %x: &TempRef->ldr, StaticShape:txs\n",&TempRef->ldr);
            TempRef->reference = (unsigned long *)&sh2->TextureData;
            relocations.push((unsigned long *)&TempRef->reference);
            DUMP_RELOCATION(f,"relocations: %x: &TempRef->reference, StaticShape:txs\n",&TempRef->reference);
//            char *TXS = TempStringTable;
//            strcpy(TempStringTable,StaticShapeTXSNames[i][j]);
//            TempStringTable += strlen(StaticShapeTXSNames[i][j]);
//            strcpy(TempStringTable,":txs");
//            TempStringTable += 5;
            char *TXS = stringtable.FindSymbolString(StaticShapeTXSNames[i][j], ovlStringTable::TXS);
            TempRef->Symbol = TXS;
            relocations.push((unsigned long *)&TempRef->Symbol);
            DUMP_RELOCATION(f,"relocations: %x: &TempRef->Symbol, StaticShape:txs\n",&TempRef->Symbol);
            TempRef++;
        }
        if (StaticShapes[i]->EffectCount != 0) {
            shs->EffectPosition = (D3DMATRIX *)TempStaticShape;
            TempStaticShape += (StaticShapes[i]->EffectCount * sizeof(D3DMATRIX));
            shs->EffectName = (char **)TempStaticShape;
            shs->EffectCount = StaticShapes[i]->EffectCount;
            TempStaticShape += (StaticShapes[i]->EffectCount * 4);
            relocations.push((unsigned long *)&shs->EffectName);
            DUMP_RELOCATION(f,"relocations: %x: &shs->EffectName\n",&shs->EffectName);
            relocations.push((unsigned long *)&shs->EffectPosition);
            DUMP_RELOCATION(f,"relocations: %x: &shs->EffectPosition\n",&shs->EffectPosition);
            for (unsigned long j = 0;j < StaticShapes[i]->EffectCount;j++) {
                shs->EffectName[j] = (char *)TempStaticShape;
                strcpy((char *)TempStaticShape,StaticShapes[i]->EffectName[j]);
                TempStaticShape += strlen(StaticShapes[i]->EffectName[j]) + 1;
                unsigned long x = strlen(StaticShapes[i]->EffectName[j]) + 1;
                while (x % 4 != 0) {
                    *TempStaticShape = 0;
                    TempStaticShape++;
                    x++;
                }
                relocations.push((unsigned long *)&shs->EffectName[j]);
                DUMP_RELOCATION(f,"relocations: %x: &shs->EffectName[j]\n",&shs->EffectName[j]);
                memcpy(&shs->EffectPosition[j],&StaticShapes[i]->EffectPosition[j],sizeof(D3DMATRIX));
            }
        } else {
            shs->EffectName = 0;
            shs->EffectCount = 0;
            shs->EffectPosition = 0;
        }
        TempSym++;
        TempLdr++;
    }
    DUMP_RELOCATION(f,"StaticShapeLimits %x, %x\n",TempStaticShape,StaticShape+StaticShapeSize);

    // Iterate over the bone shapes
    for (i = 0;i < BoneShapes.size();i++) {
//        strcpy(TempStringTable,BoneShapeNames[i]);
//        char *bshname = TempStringTable;
//        TempStringTable += strlen(BoneShapeNames[i]);
//        strcpy(TempStringTable,":bsh");
//        TempStringTable += 5;
        char *bshname = stringtable.FindSymbolString(BoneShapeNames[i], ovlStringTable::BSH);
        BoneShape1 *bsh = (BoneShape1 *)TempBoneShape;
        memcpy(bsh,BoneShapes[i],sizeof(BoneShape1));
        TempBoneShape += sizeof(BoneShape1);
        bsh->sh = (BoneShape2 **)TempBoneShape;
        relocations.push((unsigned long *)&bsh->sh);
        DUMP_RELOCATION(f,"relocations: %x: &bsh->sh\n",&bsh->sh);
        TempBoneShape += bsh->MeshCount * 4;
        TempSym->data = (unsigned long *)bsh;
        TempSym->IsPointer = true;
        TempSym->Symbol = bshname;
        TempLdr->data = (unsigned long *)bsh;
        relocations.push((unsigned long *)&TempLdr->data);
        DUMP_RELOCATION(f,"relocations: %x: &TempLdr->data\n",&TempLdr->data);
        TempLdr->HasExtraData = false;
        // This will vary dependent upon the types within the ovl
        TempLdr->LoaderType = 3;
        TempLdr->Sym = TempSym;
        relocations.push((unsigned long *)&TempLdr->Sym);
        DUMP_RELOCATION(f,"relocations: %x: &TempLdr->Sym\n",&TempLdr->Sym);
        TempLdr->SymbolsToResolve = 0;
        for (unsigned long j = 0;j < BoneShapes[i]->MeshCount;j++) {
            BoneShape2 *obj = BoneShapes[i]->sh[j];
            BoneShape2 *sh2 = (BoneShape2 *)TempBoneShape;
            TempBoneShape += sizeof(BoneShape2);
            memcpy(sh2,obj,sizeof(BoneShape2));
            bsh->sh[j] = sh2;
            relocations.push((unsigned long *)&bsh->sh[j]);
            DUMP_RELOCATION(f,"relocations: %x: &bsh->sh[j]\n",&bsh->sh[j]);
            sh2->Vertexes = (VERTEX2 *)TempBoneShape;
            memcpy(sh2->Vertexes,obj->Vertexes,obj->VertexCount*sizeof(VERTEX2));
            relocations.push((unsigned long *)&sh2->Vertexes);
            DUMP_RELOCATION(f,"relocations: %x: &sh2->Vertexes\n",&sh2->Vertexes);
            TempBoneShape += obj->VertexCount*sizeof(VERTEX2);
            sh2->Triangles = (unsigned short *)TempBoneShape;
            memcpy(sh2->Triangles,obj->Triangles,obj->IndexCount*2);
            TempBoneShape += obj->IndexCount*2;
            relocations.push((unsigned long *)&sh2->Triangles);
            DUMP_RELOCATION(f,"relocations: %x: &sh2->Triangles\n",&sh2->Triangles);
            TempLdr->SymbolsToResolve += 2;
            TempRef->ldr = TempLdr;
            relocations.push((unsigned long *)&TempRef->ldr);
            DUMP_RELOCATION(f,"relocations: %x: &TempRef->ldr, BoneShape:ftx\n",&TempRef->ldr);
            TempRef->reference = (unsigned long *)&sh2->fts;
            relocations.push((unsigned long *)&TempRef->reference);
            DUMP_RELOCATION(f,"relocations: %x: &TempRef->reference, BoneShape:ftx\n",&TempRef->reference);
//            char *FTX = TempStringTable;
//            strcpy(TempStringTable,BoneShapeFTXNames[i][j]);
//            TempStringTable += strlen(BoneShapeFTXNames[i][j]);
//            strcpy(TempStringTable,":ftx");
//            TempStringTable += 5;
            char *FTX = stringtable.FindSymbolString(BoneShapeFTXNames[i][j], ovlStringTable::FTX);
            TempRef->Symbol = FTX;
            relocations.push((unsigned long *)&TempRef->Symbol);
            DUMP_RELOCATION(f,"relocations: %x: &TempRef->Symbol, BoneShape:ftx\n",&TempRef->Symbol);
            TempRef++;
            TempRef->ldr = TempLdr;
            relocations.push((unsigned long *)&TempRef->ldr);
            DUMP_RELOCATION(f,"relocations: %x: &TempRef->ldr, BoneShape:txs\n",&TempRef->ldr);
            TempRef->reference = (unsigned long *)&sh2->TextureData;
            relocations.push((unsigned long *)&TempRef->reference);
            DUMP_RELOCATION(f,"relocations: %x: &TempRef->reference, BoneShape:txs\n",&TempRef->reference);
//            char *TXS = TempStringTable;
//            strcpy(TempStringTable,BoneShapeTXSNames[i][j]);
//            TempStringTable += strlen(BoneShapeTXSNames[i][j]);
//            strcpy(TempStringTable,":txs");
//            TempStringTable += 5;
            char *TXS = stringtable.FindSymbolString(BoneShapeTXSNames[i][j], ovlStringTable::TXS);
            TempRef->Symbol = TXS;
            relocations.push((unsigned long *)&TempRef->Symbol);
            DUMP_RELOCATION(f,"relocations: %x: &TempRef->Symbol, BoneShape:txs\n",&TempRef->Symbol);
            TempRef++;
        }
        if (BoneShapes[i]->BoneCount != 0) {
            // Copy the array of BoneStruct's into position as Bones
            // Copy the array of D3DMATRIX's into position as BonePositions1
            // Copy the array of D3DMATRIX's into position as BonePositions2
            bsh->Bones = (BoneStruct *)TempBoneShape;
            TempBoneShape += (BoneShapes[i]->BoneCount * sizeof(BoneStruct));
            relocations.push((unsigned long *)&bsh->Bones);
            DUMP_RELOCATION(f,"relocations: %x: &bsh->Bones\n",&bsh->Bones);
            bsh->BoneCount = BoneShapes[i]->BoneCount;

            bsh->BonePositions1 = (D3DMATRIX *)TempBoneShape;
            TempBoneShape += (BoneShapes[i]->BoneCount * sizeof(D3DMATRIX));
            relocations.push((unsigned long *)&bsh->BonePositions1);
            DUMP_RELOCATION(f,"relocations: %x: &bsh->BonePositions1\n",&bsh->BonePositions1);

            bsh->BonePositions2 = (D3DMATRIX *)TempBoneShape;
            TempBoneShape += (BoneShapes[i]->BoneCount * sizeof(D3DMATRIX));
            relocations.push((unsigned long *)&bsh->BonePositions2);
            DUMP_RELOCATION(f,"relocations: %x: &bsh->BonePositions2\n",&bsh->BonePositions2);

            for (unsigned long j = 0;j < BoneShapes[i]->BoneCount;j++) {
                bsh->Bones[j].ParentBoneNumber = BoneShapes[i]->Bones[j].ParentBoneNumber;
                bsh->Bones[j].BoneName = stringtable.FindString(BoneShapes[i]->Bones[j].BoneName);
//                bsh->Bones[j].BoneName = (char *)TempBoneShape;
//                strcpy((char *)TempBoneShape,BoneShapes[i]->Bones[j].BoneName);
//                TempBoneShape += strlen(BoneShapes[i]->Bones[j].BoneName) + 1;
//                unsigned long x = strlen(BoneShapes[i]->Bones[j].BoneName) + 1;
//                while (x % 4 != 0) {
//                    *TempBoneShape = 0;
//                    TempBoneShape++;
//                    x++;
//                }
                relocations.push((unsigned long *)&bsh->Bones[j].BoneName);
                DUMP_RELOCATION(f,"relocations: %x: &bsh->Bones[j].BoneName %s\n",&bsh->Bones[j].BoneName, bsh->Bones[j].BoneName);
                memcpy(&bsh->BonePositions1[j],&BoneShapes[i]->BonePositions1[j],sizeof(D3DMATRIX));
                memcpy(&bsh->BonePositions2[j],&BoneShapes[i]->BonePositions2[j],sizeof(D3DMATRIX));
            }
        } else {
            bsh->BoneCount = 0;
            bsh->Bones = 0;
            bsh->BonePositions1 = 0;
            bsh->BonePositions2 = 0;
        }
        TempSym++;
        TempLdr++;
    }
    DUMP_RELOCATION(f,"BoneShapeLimits %x, %x\n",TempBoneShape,BoneShape+BoneShapeSize);

    // Iterate over the bone anims
    for (i = 0;i < BoneAnims.size();i++) {
//        strcpy(TempStringTable,BoneAnimNames[i]);
//        char *banname = TempStringTable;
//        TempStringTable += strlen(BoneAnimNames[i]);
//        strcpy(TempStringTable,":ban");
//        TempStringTable += 5;
        char *banname = stringtable.FindSymbolString(BoneAnimNames[i], ovlStringTable::BAN);

        BoneAnim *ban = (BoneAnim *)TempBoneAnim;
        memcpy(ban,BoneAnims[i],sizeof(BoneAnim));
        TempBoneAnim += sizeof(BoneAnim);
        ban->Bones = (BoneAnimBone *)TempBoneAnim;
        relocations.push((unsigned long *)&ban->Bones);
        DUMP_RELOCATION(f,"relocations: %x: &ban->Bones\n",&ban->Bones);
        TempBoneAnim += ban->BoneCount * sizeof(BoneAnimBone);
        TempSym->data = (unsigned long *)ban;
        TempSym->IsPointer = true;
        TempSym->Symbol = banname;
        TempLdr->data = (unsigned long *)ban;
        relocations.push((unsigned long *)&TempLdr->data);
        DUMP_RELOCATION(f,"relocations: %x: &TempLdr->data\n",&TempLdr->data);
        TempLdr->HasExtraData = false;
        // This will vary dependent upon the types within the ovl
        TempLdr->LoaderType = 2;
        TempLdr->Sym = TempSym;
        relocations.push((unsigned long *)&TempLdr->Sym);
        DUMP_RELOCATION(f,"relocations: %x: &TempLdr->Sym\n",&TempLdr->Sym);
        TempLdr->SymbolsToResolve = 0;
        for (unsigned long j = 0;j < BoneAnims[i]->BoneCount;j++) {
            BoneAnimBone *obj = &BoneAnims[i]->Bones[j];
            BoneAnimBone *bab = &ban->Bones[j];
            memcpy(bab,obj,sizeof(BoneAnimBone));
            if (obj->TranslateCount) {
                // Copy translation array
                bab->Translate = (txyz *)TempBoneAnim;
                memcpy(bab->Translate,obj->Translate,obj->TranslateCount*sizeof(txyz));
                relocations.push((unsigned long *)&bab->Translate);
                DUMP_RELOCATION(f,"relocations: %x: &bab->Translate\n",&bab->Translate);
                TempBoneAnim += obj->TranslateCount*sizeof(txyz);
            } else {
                bab->Translate = 0;
            }
            if (obj->RotateCount) {
                // Copy rotation array
                bab->Rotate = (txyz *)TempBoneAnim;
                memcpy(bab->Rotate,obj->Rotate,obj->RotateCount*sizeof(txyz));
                relocations.push((unsigned long *)&bab->Rotate);
                DUMP_RELOCATION(f,"relocations: %x: &bab->Rotate\n",&bab->Rotate);
                TempBoneAnim += obj->RotateCount*sizeof(txyz);
            } else {
                bab->Rotate = 0;
            }
            // Copy name
            bab->Name = stringtable.FindString(BoneAnims[i]->Bones[j].Name);
//            bab->Name = (char *)TempBoneAnim;
//            strcpy((char *)TempBoneAnim,BoneAnims[i]->Bones[j].Name);
//            TempBoneAnim += strlen(BoneShapes[i]->Bones[j].BoneName) + 1;
//            unsigned long x = strlen(BoneShapes[i]->Bones[j].BoneName) + 1;
//            while (x % 4 != 0) {
//                *TempBoneAnim = 0;
//                TempBoneAnim++;
//                x++;
//            }
            relocations.push((unsigned long *)&bab->Name);
            DUMP_RELOCATION(f,"relocations: %x: &bab->Name\n",&bab->Name);
        }

        TempSym++;
        TempLdr++;
    }
    DUMP_RELOCATION(f,"BoneAnimLimits %x, %x\n",TempBoneAnim,BoneAnimx+BoneAnimSize);

    if (SVD) {
//        strcpy(TempStringTable,SVDName);
//        char *svdname = TempStringTable;
//        TempStringTable += strlen(SVDName);
//        strcpy(TempStringTable,":svd");
//        TempStringTable += 5;
        char *svdname = stringtable.FindSymbolString(SVDName, ovlStringTable::SVD);
        SceneryItemVisual *sv = (SceneryItemVisual *)TempSVD;
        memcpy(sv,SVD,sizeof(SceneryItemVisual));
        TempSVD += sizeof(SceneryItemVisual);
        sv->LODMeshes = (SceneryItemVisualLOD **)TempSVD;
        relocations.push((unsigned long *)&sv->LODMeshes);
        DUMP_RELOCATION(f,"relocations: %x: &sv->LODMeshes\n",&sv->LODMeshes);
        TempSVD += sv->LODCount * 4;
        TempSym->data = (unsigned long *)sv;
        TempSym->IsPointer = true;
        TempSym->Symbol = svdname;
        TempLdr->data = (unsigned long *)sv;
        relocations.push((unsigned long *)&TempLdr->data);
        DUMP_RELOCATION(f,"relocations: %x: &TempLdr->data\n",&TempLdr->data);
        TempLdr->HasExtraData = false;
        TempLdr->LoaderType = 1;
        TempLdr->Sym = TempSym;
        relocations.push((unsigned long *)&TempLdr->Sym);
        DUMP_RELOCATION(f,"relocations: %x: &TempLdr->Sym\n",&TempLdr->Sym);
        TempLdr->SymbolsToResolve = 0;

        int count_shs = 0;
        int count_bsh = 0;
        int count_ban = 0;

        for (unsigned long i = 0;i < SVD->LODCount;i++) {
            DUMP_RELOCATION(f,"SVD->LODMeshes[%i]\n",i);

            // Allocate the LOD once, then check the Names arrays to see what member data needs to be filled in or referenced.
            //char *lodname = NULL;
            SceneryItemVisualLOD *LOD = (SceneryItemVisualLOD *)TempSVD;
            TempSVD += sizeof(SceneryItemVisualLOD);
            memcpy(LOD,SVD->LODMeshes[i],sizeof(SceneryItemVisualLOD));
            sv->LODMeshes[i] = LOD;
            relocations.push((unsigned long *)&sv->LODMeshes[i]);
            DUMP_RELOCATION(f,"relocations: %x: &sv->LODMeshes[i]\n",&sv->LODMeshes[i]);

            if ((LOD->MeshType == SVDLOD_MESHTYPE_STATIC) && SHSNames) {
                char* st = stringtable.FindSymbolString(SHSNames[count_shs], ovlStringTable::SHS);
                DUMP_RELOCATION(f,"SYMBOL: %s:\n",st);

                TempRef->Symbol = st;
                relocations.push((unsigned long *)&TempRef->Symbol);
                DUMP_RELOCATION(f,"relocations: %x: &TempRef->Symbol, LOD shs\n",&TempRef->Symbol);

                TempRef->ldr = TempLdr;
                TempLdr->SymbolsToResolve++;
                relocations.push((unsigned long *)&TempRef->ldr);
                DUMP_RELOCATION(f,"relocations: %x: &TempRef->ldr, LOD shs\n",&TempRef->ldr);

                TempRef->reference = (unsigned long *)&LOD->StaticShape;
                relocations.push((unsigned long *)&TempRef->reference);
                DUMP_RELOCATION(f,"relocations: %x: &TempRef->reference, LOD shs\n",&TempRef->reference);
                TempRef++;

                LOD->LODName = stringtable.FindString(SHSNames[count_shs]);
                relocations.push((unsigned long *)&LOD->LODName);
                DUMP_RELOCATION(f,"relocations: %x: &LOD->LODName\n",&LOD->LODName);

                count_shs++;
            } else if ((LOD->MeshType == SVDLOD_MESHTYPE_ANIMATED) && BSHNames) {
                char* st = stringtable.FindSymbolString(BSHNames[count_bsh], ovlStringTable::BSH);
                DUMP_RELOCATION(f,"SYMBOL: %s:\n",st);

                TempRef->Symbol = st;
                relocations.push((unsigned long *)&TempRef->Symbol);
                DUMP_RELOCATION(f,"relocations: %x: &TempRef->Symbol, LOD bsh\n",&TempRef->Symbol);
                TempRef->ldr = TempLdr;
                TempLdr->SymbolsToResolve++;
                relocations.push((unsigned long *)&TempRef->ldr);
                DUMP_RELOCATION(f,"relocations: %x: &TempRef->ldr, LOD bsh\n",&TempRef->ldr);
                TempRef->reference = (unsigned long *)&LOD->BoneShape;
                relocations.push((unsigned long *)&TempRef->reference);
                DUMP_RELOCATION(f,"relocations: %x: &TempRef->reference, LOD bsh\n",&TempRef->reference);
                TempRef++;

                LOD->LODName = stringtable.FindString(BSHNames[count_bsh]);
                relocations.push((unsigned long *)&LOD->LODName);
                DUMP_RELOCATION(f,"relocations: %x: &LOD->LODName\n",&LOD->LODName);

                count_bsh++;

                if (BANNames && LOD->AnimationCount) {
                    // Allocate space for the animation array pointers
                    LOD->AnimationArray = (BoneAnim ***)TempSVD;
                    TempSVD += 4*LOD->AnimationCount;
                    relocations.push((unsigned long *)&LOD->AnimationArray);
                    DUMP_RELOCATION(f,"AnimationCount %i\n",LOD->AnimationCount);

                    for (unsigned int j = 0;j<LOD->AnimationCount;j++) {
                        LOD->AnimationArray[j] = (BoneAnim **)TempSVD;
                        TempSVD += 4;
                        relocations.push((unsigned long *)&LOD->AnimationArray[j]);
                        DUMP_RELOCATION(f,"relocations: %x: &LOD->AnimationArray[j]\n",&LOD->AnimationArray[j]);


                        char* st = stringtable.FindSymbolString(BANNames[count_ban], ovlStringTable::BAN);
                        DUMP_RELOCATION(f,"SYMBOL: %s:\n",st);

                        TempRef->Symbol = st;
                        relocations.push((unsigned long *)&TempRef->Symbol);
                        DUMP_RELOCATION(f,"relocations: %x: &TempRef->Symbol, LOD ban\n",&TempRef->Symbol);

                        TempRef->ldr = TempLdr;
                        TempLdr->SymbolsToResolve++;
                        relocations.push((unsigned long *)&TempRef->ldr);
                        DUMP_RELOCATION(f,"relocations: %x: &TempRef->ldr, LOD ban\n",&TempRef->ldr);

                        // Note; this is deliberately the location pointed to by LOD->AnimationArray[j]
                        TempRef->reference = (unsigned long *)LOD->AnimationArray[j];
                        relocations.push((unsigned long *)&TempRef->reference);
                        DUMP_RELOCATION(f,"relocations: %x: &TempRef->reference, LOD ban\n",&TempRef->reference);
                        TempRef++;

                        count_ban++;
                    }

                }

            } else {
                DUMP_RELOCATION(f,"ERROR: Wrong meshtype for LOD\n");
            }

#if 0
// Mostly ridiculous crap
            if (SHSNames) {
/*
                unsigned long namel = strlen(SHSNames[i]);
                for (char *st = (char *)StringTable;st < TempStringTable;st++) {
                    if (memcmp(SHSNames[i],st,namel) == 0) {
#ifdef DUMP_RELOCATIONS
                        fprintf(f,"found %s %i\n",st, namel);
#endif
                        if (memcmp(st+namel,":shs",4) == 0) {
*/
                            char* st = stringtable.FindSymbolString(SHSNames[i], ovlStringTable::SHS);
#ifdef DUMP_RELOCATIONS
                            fprintf(f,"SYMBOL: %s:\n",st);
#endif
                            TempRef->Symbol = st;
                            relocations.push((unsigned long *)&TempRef->Symbol);
#ifdef DUMP_RELOCATIONS
                            fprintf(f,"relocations: %x: &TempRef->Symbol, LOD shs\n",&TempRef->Symbol);
#endif
                            TempRef->ldr = TempLdr;
                            TempLdr->SymbolsToResolve++;
                            relocations.push((unsigned long *)&TempRef->ldr);
#ifdef DUMP_RELOCATIONS
                            fprintf(f,"relocations: %x: &TempRef->ldr, LOD shs\n",&TempRef->ldr);
#endif
                            TempRef->reference = (unsigned long *)&LOD->StaticShape;
                            relocations.push((unsigned long *)&TempRef->reference);
#ifdef DUMP_RELOCATIONS
                            fprintf(f,"relocations: %x: &TempRef->reference, LOD shs\n",&TempRef->reference);
#endif
                            TempRef++;

                            if (!lodname) {
//                                lodname = TempStringTable;
//                                strcpy(lodname,SHSNames[i]);
//                                TempStringTable += strlen(SHSNames[i]);
//                                TempStringTable++;
//                                LOD->LODName = lodname;
                                LOD->LODName = stringtable.FindString(SHSNames[i]);
                                relocations.push((unsigned long *)&LOD->LODName);
#ifdef DUMP_RELOCATIONS
                                fprintf(f,"relocations: %x: &LOD->LODName\n",&LOD->LODName);
#endif
                            }
/*
                            break;
                        }
                    }
                }
*/
            }

            if (BSHNames) {
                unsigned long namel = strlen(BSHNames[i]);
                for (char *st = (char *)StringTable;st < TempStringTable;st++) {
                    if (memcmp(BSHNames[i],st,namel) == 0) {
#ifdef DUMP_RELOCATIONS
                        fprintf(f,"found %s %i\n",st, namel);
#endif
                        if (memcmp(st+namel,":bsh",4) == 0) {
#ifdef DUMP_RELOCATIONS
                            fprintf(f,"SYMBOL: %s:\n",st);
#endif
                            TempRef->Symbol = st;
                            relocations.push((unsigned long *)&TempRef->Symbol);
#ifdef DUMP_RELOCATIONS
                            fprintf(f,"relocations: %x: &TempRef->Symbol, LOD bsh\n",&TempRef->Symbol);
#endif
                            TempRef->ldr = TempLdr;
                            TempLdr->SymbolsToResolve++;
                            relocations.push((unsigned long *)&TempRef->ldr);
#ifdef DUMP_RELOCATIONS
                            fprintf(f,"relocations: %x: &TempRef->ldr, LOD bsh\n",&TempRef->ldr);
#endif
                            TempRef->reference = (unsigned long *)&LOD->BoneShape;
                            relocations.push((unsigned long *)&TempRef->reference);
#ifdef DUMP_RELOCATIONS
                            fprintf(f,"relocations: %x: &TempRef->reference, LOD bsh\n",&TempRef->reference);
#endif
                            TempRef++;

                            if (!lodname) {
                                lodname = TempStringTable;
                                strcpy(lodname,BSHNames[i]);
                                TempStringTable += strlen(BSHNames[i]);
                                TempStringTable++;
                                LOD->LODName = lodname;
                                relocations.push((unsigned long *)&LOD->LODName);
#ifdef DUMP_RELOCATIONS
                                fprintf(f,"relocations: %x: &LOD->LODName\n",&LOD->LODName);
#endif
                            }
                            break;
                        }
                    }
                }
            }
            if (BANNames) {
                unsigned long namel = strlen(BANNames[i]);
                for (char *st = (char *)StringTable;st < TempStringTable;st++) {
                    if (memcmp(BANNames[i],st,namel) == 0) {
#ifdef DUMP_RELOCATIONS
                        fprintf(f,"found %s %i\n",st, namel);
#endif
                        if (memcmp(st+namel,":ban",4) == 0) {
#ifdef DUMP_RELOCATIONS
                            fprintf(f,"matched suffix :ban %x\n",LOD->AnimationArray);
#endif

                            // Allocate space for the animation array pointers
                            LOD->AnimationArray = (BoneAnim ***)TempSVD;
                            TempSVD += 4*LOD->AnimationCount;
                            relocations.push((unsigned long *)&LOD->AnimationArray);

#ifdef DUMP_RELOCATIONS
                            fprintf(f,"AnimationCount %i\n",LOD->AnimationCount);
#endif
                            for (unsigned int j = 0;j<LOD->AnimationCount;j++) {
                                LOD->AnimationArray[j] = (BoneAnim **)TempSVD;
                                TempSVD += 4;
                                relocations.push((unsigned long *)&LOD->AnimationArray[j]);

#ifdef DUMP_RELOCATIONS
                                fprintf(f,"SYMBOL: %s:\n",st);
#endif
                                TempRef->Symbol = st;
                                relocations.push((unsigned long *)&TempRef->Symbol);
#ifdef DUMP_RELOCATIONS
                                fprintf(f,"relocations: %x: &TempRef->Symbol, LOD ban\n",&TempRef->Symbol);
#endif
                                TempRef->ldr = TempLdr;
                                TempLdr->SymbolsToResolve++;
                                relocations.push((unsigned long *)&TempRef->ldr);
#ifdef DUMP_RELOCATIONS
                                fprintf(f,"relocations: %x: &TempRef->ldr, LOD ban\n",&TempRef->ldr);
#endif
                                // Note; this is deliberately the location pointed to by LOD->AnimationArray[j]
                                TempRef->reference = (unsigned long *)LOD->AnimationArray[j];
                                relocations.push((unsigned long *)&TempRef->reference);
#ifdef DUMP_RELOCATIONS
                                fprintf(f,"relocations: %x: &TempRef->reference, LOD ban\n",&TempRef->reference);
#endif
                                TempRef++;

                                if (!lodname) {
                                    lodname = TempStringTable;
                                    strcpy(lodname,BANNames[i]);
                                    TempStringTable += strlen(BANNames[i]);
                                    TempStringTable++;
                                    LOD->LODName = lodname;
                                    relocations.push((unsigned long *)&LOD->LODName);
#ifdef DUMP_RELOCATIONS
                                    fprintf(f,"relocations: %x: &LOD->LODName\n",&LOD->LODName);
#endif
                                }
                            }
                            break;
                        }
                    }
                }
            }
#endif // Ridiculous crap
        }	// LODMesh loop

        TempSym++;
        TempLdr++;
    } else {
        // Add the Texture Names if we write a texture ovl
//        for (i = 0;i < FlexiTextures.size();i++) {
//            strcpy(TempStringTable,FlexiTextureNames[i]);
//            TempStringTable += strlen(FlexiTextureNames[i]);
//            strcpy(TempStringTable,":ftx");
//            TempStringTable += 5;
//        }
    }
    for (i = 0;i < FlexiTextures.size();i++) {
        // FlexiTextureInfoStruct
        FlexiTextureInfoStruct *ftis = (FlexiTextureInfoStruct *)TempFlexiTexture;
        memcpy(ftis,FlexiTextures[i],sizeof(FlexiTextureInfoStruct));
        TempFlexiTexture += sizeof(FlexiTextureInfoStruct);

        // Offsets
        // Now to be provided by the creator
        unsigned long *ofs1 = (unsigned long *)TempFlexiTexture;
        memcpy(ofs1, FlexiTextures[i]->offset1, 4*FlexiTextures[i]->offsetCount);
        TempFlexiTexture += 4*FlexiTextures[i]->offsetCount;
        ftis->offset1 = ofs1;
        relocations.push((unsigned long *)&ftis->offset1);
        DUMP_RELOCATION(f,"relocations: %x: &ftis->offset1\n",&ftis->offset1);

        // FlexiTextureStructs
        FlexiTextureStruct *fs = (FlexiTextureStruct *)TempFlexiTexture;
        TempFlexiTexture += sizeof(FlexiTextureStruct)*FlexiTextures[i]->fts2Count;
        memcpy(fs,FlexiTextures[i]->fts2,sizeof(FlexiTextureStruct)*FlexiTextures[i]->fts2Count);
        ftis->fts2 = fs;
        relocations.push((unsigned long *)&ftis->fts2);
        DUMP_RELOCATION(f,"relocations: %x: &ftis->fts2\n",&ftis->fts2);
        for(unsigned long m = 0; m < FlexiTextures[i]->fts2Count; m++) {
            // Palette
            fs[m].palette = TempFlexiTexture;
            relocations.push((unsigned long *)&fs[m].palette);
            DUMP_RELOCATION(f,"relocations: %x: &fs[%ld].palette\n",&fs[m].palette, m);
            TempFlexiTexture += 256*sizeof(RGBQUAD);
            memcpy(fs[m].palette,FlexiTextures[i]->fts2[m].palette,256*sizeof(RGBQUAD));

            // Texture
            fs[m].texture = TempFlexiTexture;
            relocations.push((unsigned long *)&fs[m].texture);
            DUMP_RELOCATION(f,"relocations: %x: &fs[%ld].texture\n",&fs[m].texture, m);
            unsigned char *t = FlexiTextures[i]->fts2[m].texture;
            unsigned long w = FlexiTextures[i]->fts2[m].width;
            unsigned long h = FlexiTextures[i]->fts2[m].height;
            memcpy(fs[m].texture,t,w*h);
            TempFlexiTexture += FlexiTextures[i]->fts2[m].width*FlexiTextures[i]->fts2[m].height;
            if (FlexiTextures[i]->fts2[m].alpha != 0) {
                fs[m].alpha = TempFlexiTexture;
                relocations.push((unsigned long *)&fs[m].alpha);
                DUMP_RELOCATION(f,"relocations: %x: &fs[%ld].alpha\n",&fs[m].alpha, m);
                memcpy(fs[m].alpha,FlexiTextures[i]->fts2[m].alpha,FlexiTextures[i]->fts2[m].width*FlexiTextures[i]->fts2[m].height);
                TempFlexiTexture += FlexiTextures[i]->fts2[m].width*FlexiTextures[i]->fts2[m].height;
            }
        }

        // Name & Symbols
//        unsigned long namel = strlen(FlexiTextureNames[i]);
//        for (char *st = (char *)StringTable;st < TempStringTable;st++) {
//            char *x = FlexiTextureNames[i];
//            if (memcmp(x,st,namel) == 0) {
//                if (memcmp(st+namel,":ftx",4) == 0) {
//                    TempSym->Symbol = st;
//                    break;
//                }
//            }
//        }
        TempSym->Symbol = stringtable.FindSymbolString(FlexiTextureNames[i], ovlStringTable::FTX);
        TempSym->data = (unsigned long *)ftis;
        TempSym->IsPointer = true;
        TempLdr->data = (unsigned long *)ftis;
        relocations.push((unsigned long *)&TempLdr->data);
        DUMP_RELOCATION(f,"relocations: %x: &TempLdr->data\n",&TempLdr->data);

        TempLdr->HasExtraData = false;
        TempLdr->LoaderType = 0;
        TempLdr->Sym = TempSym;
        relocations.push((unsigned long *)&TempLdr->Sym);
        DUMP_RELOCATION(f,"relocations: %x: &TempLdr->Sym\n",&TempLdr->Sym);

        TempLdr->SymbolsToResolve = 0;
        TempSym++;
        TempLdr++;

    }

    SymbolStruct *syms2 = (SymbolStruct *)Sym;
    qsort(syms2,SymbolCount,sizeof(SymbolStruct),compare);
    for (i = 0;i < SymbolCount;i++) {
        relocations.push((unsigned long *)&(syms2[i].Symbol));
        DUMP_RELOCATION(f,"relocations: %x: &(syms2[i].Symbol)\n",&(syms2[i].Symbol));
        relocations.push((unsigned long *)&(syms2[i].data));
        DUMP_RELOCATION(f,"relocations: %x: &(syms2[i].data)\n",&(syms2[i].data));
    }
    LoaderStruct *ldrs2 = (LoaderStruct *)Ldr;
    for (i = 0;i < LoaderCount;i++) {
        for (unsigned long j = 0;j < SymbolCount;j++) {
            if (ldrs2[i].data == syms2[j].data) {
                ldrs2[i].Sym = &syms2[j];
            }
        }
    }
    Type0Files = new File;
    OpenFiles[CurrentFile].Types[0].count = 1;
    OpenFiles[CurrentFile].Types[0].dataptr = Type0Files;
    OpenFiles[CurrentFile].Types[0].reloffset = 0;
    OpenFiles[CurrentFile].Types[0].size = 0;
    Type0Files[0].size = stringtable.GetSize();
    Type0Files[0].reloffset = 0;
    Type0Files[0].data =(unsigned long *)stringtable.Make();
    Type0Files[0].unk = 0;
    OpenFiles[CurrentFile].Types[1].reloffset = Type0Files[0].size + Type0Files[0].reloffset;
    int fcount = 4;
    if (SVD)
        fcount++;
    if ( StaticShapes.size() )
        fcount++;
    if ( BoneShapes.size() )
        fcount++;
    if ( BoneAnims.size() )
        fcount++;

    Type2Files = new File[fcount];
    int fidx = 0;
    Type2Files[fidx].size = sizeof(SymbolStruct)*SymbolCount;
    Type2Files[fidx].reloffset = Type0Files[0].size + Type0Files[0].reloffset;
    Type2Files[fidx].data = (unsigned long *)Sym;
    Type2Files[fidx].unk = 0;
    fidx++;
    Type2Files[fidx].size = sizeof(LoaderStruct)*LoaderCount;
    Type2Files[fidx].reloffset = Type2Files[fidx-1].size + Type2Files[fidx-1].reloffset;
    Type2Files[fidx].data = (unsigned long *)Ldr;
    Type2Files[fidx].unk = 0;
    fidx++;
    Type2Files[fidx].size = sizeof(SymbolRefStruct)*SymbolRefCount;
    Type2Files[fidx].reloffset = Type2Files[fidx-1].size + Type2Files[fidx-1].reloffset;
    Type2Files[fidx].data = (unsigned long *)Ref;
    Type2Files[fidx].unk = 0;
    fidx++;
    Type2Files[fidx].size = FlexiTextureSize;
    Type2Files[fidx].reloffset = Type2Files[fidx-1].size + Type2Files[fidx-1].reloffset;
    Type2Files[fidx].data = (unsigned long *)FlexiTexture;
    Type2Files[fidx].unk = 0;
    fidx++;

    if (SVD) {
        Type2Files[fidx].size = SVDSize;
        Type2Files[fidx].reloffset = Type2Files[fidx-1].size + Type2Files[fidx-1].reloffset;
        Type2Files[fidx].data = (unsigned long *)SVDx;
        Type2Files[fidx].unk = 0;
        fidx++;
    }

    if ( StaticShapes.size() ) {
        Type2Files[fidx].size = StaticShapeSize;
        Type2Files[fidx].reloffset = Type2Files[fidx-1].size + Type2Files[fidx-1].reloffset;
        Type2Files[fidx].data = (unsigned long *)StaticShape;
        Type2Files[fidx].unk = 0;
        fidx++;
    }

    if ( BoneShapes.size() ) {
        Type2Files[fidx].size = BoneShapeSize;
        Type2Files[fidx].reloffset = Type2Files[fidx-1].size + Type2Files[fidx-1].reloffset;
        Type2Files[fidx].data = (unsigned long *)BoneShape;
        Type2Files[fidx].unk = 0;
        fidx++;
    }

    if ( BoneAnims.size() ) {
        Type2Files[fidx].size = BoneAnimSize;
        Type2Files[fidx].reloffset = Type2Files[fidx-1].size + Type2Files[fidx-1].reloffset;
        Type2Files[fidx].data = (unsigned long *)BoneAnimx;
        Type2Files[fidx].unk = 0;
        fidx++;
    }

//    if (SVD)
//        OpenFiles[CurrentFile].Types[2].count = 6;
//    else
//        OpenFiles[CurrentFile].Types[2].count = 4;
    OpenFiles[CurrentFile].Types[2].count = fidx;

    OpenFiles[CurrentFile].Types[2].dataptr = Type2Files;
    OpenFiles[CurrentFile].Types[2].size = 0;
    for (unsigned long i = 0;i < OpenFiles[CurrentFile].Types[2].count;i++) {
        OpenFiles[CurrentFile].Types[2].size += Type2Files[i].size;
    }
    OpenFiles[CurrentFile].Types[2].reloffset = Type2Files[OpenFiles[CurrentFile].Types[2].count-1].reloffset;
    OpenFiles[CurrentFile].Types[3].reloffset = OpenFiles[CurrentFile].Types[2].reloffset;
    OpenFiles[CurrentFile].Types[4].reloffset = OpenFiles[CurrentFile].Types[2].reloffset;
    OpenFiles[CurrentFile].Types[5].reloffset = OpenFiles[CurrentFile].Types[2].reloffset;
    OpenFiles[CurrentFile].Types[6].reloffset = OpenFiles[CurrentFile].Types[2].reloffset;
    OpenFiles[CurrentFile].Types[7].reloffset = OpenFiles[CurrentFile].Types[2].reloffset;
    OpenFiles[CurrentFile].Types[8].reloffset = OpenFiles[CurrentFile].Types[2].reloffset;
    while (relocations.empty() == false) {
        unsigned long *relocation = relocations.front();
        relocations.pop();
        unsigned long *reloc = *(unsigned long **)relocation;
        if (reloc != 0) {
            unsigned long fixup = DoRelocationForSaving(reloc);
            if (fixup == 0xFFFFFFFF) {
                fixup = 0;
            }
            *relocation = fixup;
            unsigned long fixup2 = DoRelocationForSaving(relocation);
            DUMP_RELOCATION(f,"relocation %x, reloc %x, fixup %x, fixup2 %x\n",relocation,reloc,fixup,fixup2);
            fixups.push(fixup2);
        }
    }

    ///////////////////////////////////
    // Write Unique
    ///////////////////////////////////
    OvlHeader h;
    h.magic = 0x4B524746;
    h.References = References.size();
    h.reserved = 0;
    h.version = 1;
    OvlHeader2 h2;
    // Work out the filetype count.
    h2.FileTypeCount = 1;
    if ( StaticShapes.size() )
        h2.FileTypeCount++;
    if ( BoneShapes.size() )
        h2.FileTypeCount++;
    if ( BoneAnims.size() )
        h2.FileTypeCount++;
    if (SVD)
        h2.FileTypeCount++;

    h2.unk = 0;
    unsigned short len1 = 4;
    const char *str1 = "FGDK";
    unsigned short len2 = 0xC;
    const char *str2 = "FlexiTexture";
    unsigned long type = 1;
    unsigned short len3 = 3;
    const char *str3 = "ftx";

#ifdef DUMP_RELOCATIONS
    // Close the log file
    fclose(f);
#endif

    // Open file for writing, write header.
    f = fopen(OpenFiles[CurrentFile].FileName,"wb");
    fwrite(&h,sizeof(h),1,f);
    for (i = 0;i < References.size();i++) {
        unsigned short len = (unsigned short)strlen(References[i]);
        fwrite(&len,2,1,f);
        fwrite(References[i],len,1,f);
    }
    fwrite(&h2,sizeof(h2),1,f);
    fwrite(&len1,sizeof(len1),1,f);
    fwrite(str1,len1,1,f); // str1 = "FGDK"
    fwrite(&len2,sizeof(len2),1,f);
    fwrite(str2,len2,1,f); // str2 = "FlexiTexture"
    fwrite(&type,sizeof(type),1,f); // type = 1
    fwrite(&len3,sizeof(len3),1,f); // str3 = "ftx"
    fwrite(str3,len3,1,f);
    if (SVD) {
        len1 = 4;
        str1 = "RCT3";
        len2 = 0x11;
        str2 = "SceneryItemVisual";
        type = 1;
        len3 = 3;
        str3 = "svd";
        fwrite(&len1,sizeof(len1),1,f);
        fwrite(str1,len1,1,f);
        fwrite(&len2,sizeof(len2),1,f);
        fwrite(str2,len2,1,f);
        fwrite(&type,sizeof(type),1,f);
        fwrite(&len3,sizeof(len3),1,f);
        fwrite(str3,len3,1,f);
    }
    if ( BoneAnims.size() ) {
        len1 = 4;
        str1 = "RCT3";
        len2 = 0x8;
        str2 = "BoneAnim";
        type = 1;
        len3 = 3;
        str3 = "ban";
        fwrite(&len1,sizeof(len1),1,f);
        fwrite(str1,len1,1,f);
        fwrite(&len2,sizeof(len2),1,f);
        fwrite(str2,len2,1,f);
        fwrite(&type,sizeof(type),1,f);
        fwrite(&len3,sizeof(len3),1,f);
        fwrite(str3,len3,1,f);
    }
    if ( BoneShapes.size() ) {
        len1 = 4;
        str1 = "RCT3";
        len2 = 0x9;
        str2 = "BoneShape";
        type = 1;
        len3 = 3;
        str3 = "bsh";
        fwrite(&len1,sizeof(len1),1,f);
        fwrite(str1,len1,1,f);
        fwrite(&len2,sizeof(len2),1,f);
        fwrite(str2,len2,1,f);
        fwrite(&type,sizeof(type),1,f);
        fwrite(&len3,sizeof(len3),1,f);
        fwrite(str3,len3,1,f);
    }
    if ( StaticShapes.size() ) {
        len1 = 4;
        str1 = "RCT3";
        len2 = 0xB;
        str2 = "StaticShape";
        type = 1;
        len3 = 3;
        str3 = "shs";
        fwrite(&len1,sizeof(len1),1,f);
        fwrite(str1,len1,1,f);
        fwrite(&len2,sizeof(len2),1,f);
        fwrite(str2,len2,1,f);
        fwrite(&type,sizeof(type),1,f);
        fwrite(&len3,sizeof(len3),1,f);
        fwrite(str3,len3,1,f);
    }

    // Write the number of files of each type
    for (i = 0;i < 9;i++) {
        fwrite(&OpenFiles[CurrentFile].Types[i].count,sizeof(OpenFiles[CurrentFile].Types[i].count),1,f);
    }
    // Write the files themselves.
    for (i = 0;i < 9;i++) {
        for (unsigned long j = 0;j < OpenFiles[CurrentFile].Types[i].count;j++) {
            fwrite(&OpenFiles[CurrentFile].Types[i].dataptr[j].size,sizeof(OpenFiles[CurrentFile].Types[i].dataptr[j].size),1,f);
            fwrite(OpenFiles[CurrentFile].Types[i].dataptr[j].data,OpenFiles[CurrentFile].Types[i].dataptr[j].size,1,f);
        }
    }
    // Write the fixups
    unsigned long size = fixups.size();
    fwrite(&size,sizeof(size),1,f);
    while (fixups.empty() == false) {
        unsigned long fixup = fixups.front();
        fixups.pop();
        fwrite(&fixup,sizeof(fixup),1,f);
    }
    fclose(f);

    ///////////////////////////////////
    // Write Common
    ///////////////////////////////////
    CurrentFile = OVL_COMMON;
    h.magic = 0x4B524746;
    h.References = 0;
    h.reserved = 0;
    h.version = 1;
    h2.FileTypeCount = 1;
    if ( StaticShapes.size() )
        h2.FileTypeCount++;
    if ( BoneShapes.size() )
        h2.FileTypeCount++;
    if ( BoneAnims.size() )
        h2.FileTypeCount++;
    if (SVD)
        h2.FileTypeCount++;

    h2.unk = 0;
    len1 = 4;
    str1 = "FGDK";
    len2 = 0xC;
    str2 = "FlexiTexture";
    type = 1;
    len3 = 3;
    str3 = "ftx";
    f = fopen(OpenFiles[OVL_COMMON].FileName,"wb");
    fwrite(&h,sizeof(h),1,f);
    fwrite(&h2,sizeof(h2),1,f);
    fwrite(&len1,sizeof(len1),1,f);
    fwrite(str1,len1,1,f);
    fwrite(&len2,sizeof(len2),1,f);
    fwrite(str2,len2,1,f);
    fwrite(&type,sizeof(type),1,f);
    fwrite(&len3,sizeof(len3),1,f);
    fwrite(str3,len3,1,f);
    if (SVD) {
        len1 = 4;
        str1 = "RCT3";
        len2 = 0x11;
        str2 = "SceneryItemVisual";
        type = 1;
        len3 = 3;
        str3 = "svd";
        fwrite(&len1,sizeof(len1),1,f);
        fwrite(str1,len1,1,f);
        fwrite(&len2,sizeof(len2),1,f);
        fwrite(str2,len2,1,f);
        fwrite(&type,sizeof(type),1,f);
        fwrite(&len3,sizeof(len3),1,f);
        fwrite(str3,len3,1,f);
    }
    if ( BoneAnims.size() ) {
        len1 = 4;
        str1 = "RCT3";
        len2 = 0x8;
        str2 = "BoneAnim";
        type = 1;
        len3 = 3;
        str3 = "ban";
        fwrite(&len1,sizeof(len1),1,f);
        fwrite(str1,len1,1,f);
        fwrite(&len2,sizeof(len2),1,f);
        fwrite(str2,len2,1,f);
        fwrite(&type,sizeof(type),1,f);
        fwrite(&len3,sizeof(len3),1,f);
        fwrite(str3,len3,1,f);
    }
    if ( BoneShapes.size() ) {
        len1 = 4;
        str1 = "RCT3";
        len2 = 0x9;
        str2 = "BoneShape";
        type = 1;
        len3 = 3;
        str3 = "bsh";
        fwrite(&len1,sizeof(len1),1,f);
        fwrite(str1,len1,1,f);
        fwrite(&len2,sizeof(len2),1,f);
        fwrite(str2,len2,1,f);
        fwrite(&type,sizeof(type),1,f);
        fwrite(&len3,sizeof(len3),1,f);
        fwrite(str3,len3,1,f);
    }
    if ( StaticShapes.size() ) {
        len1 = 4;
        str1 = "RCT3";
        len2 = 0xB;
        str2 = "StaticShape";
        type = 1;
        len3 = 3;
        str3 = "shs";
        fwrite(&len1,sizeof(len1),1,f);
        fwrite(str1,len1,1,f);
        fwrite(&len2,sizeof(len2),1,f);
        fwrite(str2,len2,1,f);
        fwrite(&type,sizeof(type),1,f);
        fwrite(&len3,sizeof(len3),1,f);
        fwrite(str3,len3,1,f);
    }

    unsigned long num = 0;
    for (i = 0;i < 9;i++) {
        fwrite(&num,sizeof(num),1,f);
    }
    fwrite(&num,sizeof(num),1,f);
    fclose(f);
}

/*

////////////////////////////////////////////////////////////////////////////////
//
//  CreateOvl
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API unsigned long CreateOvl(char *filename,char *InternalName) {
    char *UniqueOVLFilename = new char[strlen(filename)+1];
    strcpy(UniqueOVLFilename,filename);
    strcpy(strstr(UniqueOVLFilename,".common.ovl"),".unique.ovl");

    OpenFiles[OVL_COMMON].FileName= new char[strlen(filename)+1];
    strcpy(OpenFiles[OVL_COMMON].FileName , filename);
    OpenFiles[OVL_UNIQUE].FileName = UniqueOVLFilename;
    OpenFiles[OVL_COMMON].InternalName = new char[strlen(InternalName)+1];
    OpenFiles[OVL_UNIQUE].InternalName = new char[strlen(InternalName)+1];
    strcpy(OpenFiles[OVL_COMMON].InternalName , InternalName);
    strcpy(OpenFiles[OVL_UNIQUE].InternalName , InternalName);


    OpenFiles[OVL_COMMON].ReferenceCount=0;
    OpenFiles[OVL_COMMON].References=NULL;
    OpenFiles[OVL_UNIQUE].ReferenceCount=0;
    OpenFiles[OVL_UNIQUE].References=NULL;
    memset(OpenFiles[OVL_COMMON].Types,0,sizeof(OpenFiles[OVL_COMMON].Types));
    memset(OpenFiles[OVL_UNIQUE].Types,0,sizeof(OpenFiles[OVL_UNIQUE].Types));

    clearOVLData();

    return 0;
}
//Filename - .common.ovl

////////////////////////////////////////////////////////////////////////////////
//
//  clearOVLData
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void clearOVLData() {
    DeleteOVLInfo_Meshes();
    DeleteOVLInfo_FlexiTextures();
    DeleteInfo_Resources();
    DeleteInfo_FileReferences(OVL_COMMON);
    DeleteInfo_FileReferences(OVL_UNIQUE);
}


////////////////////////////////////////////////////////////////////////////////
//
//  SaveOvl
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API bool SaveOvl() {
    int i;
    unsigned long reloffset;

    CurrentFile = OVL_UNIQUE;

    //Save resource things that are reqd by other filetypes, like, bitmaps
    saveScenaryResourceData();
    reloffset=saveResourceData();

    //Save actual data

    reloffset=saveScenaryData(reloffset);

    reloffset=0;

    for ( i=1;i<9;i++) {
        if (OpenFiles[CurrentFile].Types[i-1].size >0 ) {
            reloffset=OpenFiles[CurrentFile].Types[i-1].size + OpenFiles[CurrentFile].Types[i-1].reloffset;
        }
        OpenFiles[CurrentFile].Types[i].reloffset = reloffset;
    }

    while (relocations.empty() == false) {
        unsigned long *relocation = relocations.front();
        relocations.pop();
        unsigned long *reloc = *(unsigned long **)relocation;
        if (reloc != 0) {
            unsigned long fixup = DoRelocationForSaving(reloc);
            if (fixup == 0xFFFFFFFF) {
                fixup = 0;
            }
            *relocation = fixup;
            unsigned long fixup2 = DoRelocationForSaving(relocation);
            fixups.push(fixup2);
        }
    }
    OvlHeader h;
    h.magic = 0x4B524746;
    h.References = 0;
    h.reserved = 0;
    h.version = 1;
    OvlHeader2 h2;
    h2.FileTypeCount = 3;
    h2.unk = 0;
    unsigned short len1 = 4;
    char *str1 = "FGDK";
    unsigned short len2 = 0xC;
    char *str2 = "FlexiTexture";
    unsigned long type = 1;
    unsigned short len3 = 3;
    char *str3 = "ftx";
    f = fopen(OpenFiles[CurrentFile].FileName,"wb");
    fwrite(&h,sizeof(h),1,f);
    fwrite(&h2,sizeof(h2),1,f);
    fwrite(&len1,sizeof(len1),1,f);
    fwrite(str1,len1,1,f);
    fwrite(&len2,sizeof(len2),1,f);
    fwrite(str2,len2,1,f);
    fwrite(&type,sizeof(type),1,f);
    fwrite(&len3,sizeof(len3),1,f);
    fwrite(str3,len3,1,f);
    len1 = 4;
    str1 = "RCT3";
    len2 = 0x11;
    str2 = "SceneryItemVisual";
    type = 1;
    len3 = 3;
    str3 = "svd";
    fwrite(&len1,sizeof(len1),1,f);
    fwrite(str1,len1,1,f);
    fwrite(&len2,sizeof(len2),1,f);
    fwrite(str2,len2,1,f);
    fwrite(&type,sizeof(type),1,f);
    fwrite(&len3,sizeof(len3),1,f);
    fwrite(str3,len3,1,f);
    len1 = 4;
    str1 = "RCT3";
    len2 = 0xB;
    str2 = "StaticShape";
    type = 1;
    len3 = 3;
    str3 = "shs";
    fwrite(&len1,sizeof(len1),1,f);
    fwrite(str1,len1,1,f);
    fwrite(&len2,sizeof(len2),1,f);
    fwrite(str2,len2,1,f);
    fwrite(&type,sizeof(type),1,f);
    fwrite(&len3,sizeof(len3),1,f);
    fwrite(str3,len3,1,f);
    for (i = 0;i < 9;i++) {
        fwrite(&OpenFiles[CurrentFile].Types[i].count,sizeof(OpenFiles[CurrentFile].Types[i].count),1,f);
    }
    for (i = 0;i < 9;i++) {
        for (unsigned long j = 0;j < OpenFiles[CurrentFile].Types[i].count;j++) {
            fwrite(&OpenFiles[CurrentFile].Types[i].dataptr[j].size,sizeof(OpenFiles[CurrentFile].Types[i].dataptr[j].size),1,f);
            fwrite(OpenFiles[CurrentFile].Types[i].dataptr[j].data,OpenFiles[CurrentFile].Types[i].dataptr[j].size,1,f);
        }
    }
    unsigned long size = fixups.size();
    fwrite(&size,sizeof(size),1,f);
    while (fixups.empty() == false) {
        unsigned long fixup = fixups.front();
        fixups.pop();
        fwrite(&fixup,sizeof(fixup),1,f);
    }
    fclose(f);

    CurrentFile = OVL_COMMON;

    h.magic = 0x4B524746;
    h.References = 0;
    h.reserved = 0;
    h.version = 1;
    h2.FileTypeCount = 3;
    h2.unk = 0;
    len1 = 4;
    str1 = "FGDK";
    len2 = 0xC;
    str2 = "FlexiTexture";
    type = 1;
    len3 = 3;
    str3 = "ftx";
    f = fopen(OpenFiles[OVL_COMMON].FileName,"wb");
    fwrite(&h,sizeof(h),1,f);
    fwrite(&h2,sizeof(h2),1,f);
    fwrite(&len1,sizeof(len1),1,f);
    fwrite(str1,len1,1,f);
    fwrite(&len2,sizeof(len2),1,f);
    fwrite(str2,len2,1,f);
    fwrite(&type,sizeof(type),1,f);
    fwrite(&len3,sizeof(len3),1,f);
    fwrite(str3,len3,1,f);
    len1 = 4;
    str1 = "RCT3";
    len2 = 0x11;
    str2 = "SceneryItemVisual";
    type = 1;
    len3 = 3;
    str3 = "svd";
    fwrite(&len1,sizeof(len1),1,f);
    fwrite(str1,len1,1,f);
    fwrite(&len2,sizeof(len2),1,f);
    fwrite(str2,len2,1,f);
    fwrite(&type,sizeof(type),1,f);
    fwrite(&len3,sizeof(len3),1,f);
    fwrite(str3,len3,1,f);
    len1 = 4;
    str1 = "RCT3";
    len2 = 0xB;
    str2 = "StaticShape";
    type = 1;
    len3 = 3;
    str3 = "shs";
    fwrite(&len1,sizeof(len1),1,f);
    fwrite(str1,len1,1,f);
    fwrite(&len2,sizeof(len2),1,f);
    fwrite(str2,len2,1,f);
    fwrite(&type,sizeof(type),1,f);
    fwrite(&len3,sizeof(len3),1,f);
    fwrite(str3,len3,1,f);
    unsigned long num = 0;
    for (i = 0;i < 9;i++) {
        fwrite(&num,sizeof(num),1,f);
    }
    fwrite(&num,sizeof(num),1,f);
    fclose(f);
    //delete names;

    return true;
}


////////////////////////////////////////////////////////////////////////////////
//
//  saveResourceData
//
////////////////////////////////////////////////////////////////////////////////

unsigned long StringTableSize;
//Saves names, textures
unsigned long saveResourceData() {
    unsigned long RelOffset=0;
    Type0Files = new File[ResourceItems.size()+1]; //Additional 1 is for putting string table
    OpenFiles[CurrentFile].Types[0].count = ResourceItems.size()+1;
    OpenFiles[CurrentFile].Types[0].dataptr = Type0Files;
    OpenFiles[CurrentFile].Types[0].reloffset = 0;
    OpenFiles[CurrentFile].Types[0].size = 0;

    //First item is the names of all items in the OVL
    Type0Files[0].size = StringTableSize;
    Type0Files[0].reloffset = 0;
    Type0Files[0].data =(unsigned long *)names;
    Type0Files[0].unk = 0;

    //continue
    long curItem=1;
    std::vector <libOVL_Resource *>::iterator ResourceIterator;
    for (ResourceIterator = ResourceItems.begin();ResourceIterator != ResourceItems.end();ResourceIterator++) {
        libOVL_Resource *res=ResourceItems[curItem-1];
        Type0Files[curItem].size = res->length;
        Type0Files[curItem].reloffset = Type0Files[curItem-1].size + Type0Files[curItem-1].reloffset;

        Type0Files[curItem].data = res->data ;
        Type0Files[curItem].unk = 0;

        RelOffset=Type0Files[curItem].reloffset+Type0Files[curItem].size;
        curItem++;
    }
    for (unsigned long i = 0;i < OpenFiles[CurrentFile].Types[0].count;i++) {
        OpenFiles[CurrentFile].Types[0].size += Type0Files[i].size;
    }

    return RelOffset;
}


////////////////////////////////////////////////////////////////////////////////
//
//  saveScenaryResourceData
//
////////////////////////////////////////////////////////////////////////////////

void saveScenaryResourceData() {
    long curItem=0;
    char *name = OpenFiles[CurrentFile].InternalName;

    std::vector <libOVL_MeshInfo *>::iterator MeshItemsIterator;
    for (MeshItemsIterator = MeshItems.begin();MeshItemsIterator != MeshItems.end();MeshItemsIterator++) {
        //May be for multiple mesh, this must be appended with mesh no
        char *txs = MeshItems[curItem]->textureStyle;

        //Other things remain the same
        unsigned long namelength = strlen(name)+1;
        unsigned long txslength = strlen(txs)+1;
        StringTableSize = namelength;
        StringTableSize += namelength + 4;
        StringTableSize += namelength + 4;
        StringTableSize += namelength + 4;
        StringTableSize += txslength + 4;
        names = new char[StringTableSize];
        memset(names,0,StringTableSize);
        objname = names;
        texturename = objname + namelength;
        shapename = texturename + namelength + 4;
        sceneryname = shapename + namelength + 4;
        txsname = sceneryname + namelength + 4;

        sprintf(objname,"%s",name);
        sprintf(texturename,"%s:ftx",name);
        sprintf(shapename,"%s:shs",name);
        sprintf(sceneryname,"%s:svd",name);
        sprintf(txsname,"%s:txs",txs);
        curItem++;
    }
}


////////////////////////////////////////////////////////////////////////////////
//
//  saveScenaryData
//
////////////////////////////////////////////////////////////////////////////////

unsigned long  saveScenaryData(unsigned long PrevRelOffset) {
    long curItem=0;
    unsigned long RelOffset=PrevRelOffset;
    unsigned long ShapeSize=0;
    Type2Files = new File[6];
    unsigned long i;

    std::vector <libOVL_MeshInfo *>::iterator MeshItemsIterator;

    unsigned long scenerysize = sizeof(SceneryItemVisual)+(sizeof(SceneryItemVisualLOD)*3)+12;
    svd = new unsigned char[scenerysize];
    memset(svd,0,scenerysize);
    sv = (SceneryItemVisual *)svd;
    svlod = (SceneryItemVisualLOD **)(svd+sizeof(SceneryItemVisual));
    svlod1 = (SceneryItemVisualLOD *)(svd+sizeof(SceneryItemVisual)+12);
    svlod2 = (SceneryItemVisualLOD *)(svd+sizeof(SceneryItemVisual)+12+sizeof(SceneryItemVisualLOD));
    svlod3 = (SceneryItemVisualLOD *)(svd+sizeof(SceneryItemVisual)+12+(sizeof(SceneryItemVisualLOD)*2));
    sv->sivflags = 0;
    sv->sway = 0.0;
    sv->brightness = 1.0;
    sv->unk4 = 1.0;
    sv->scale = 0.0;
    sv->LODCount = 3;
    sv->LODMeshes = svlod;
    sv->unk6 = 0;
    sv->unk7 = 0;
    sv->unk8 = 0;
    sv->unk9 = 0;
    sv->unk10 = 0;
    sv->unk11 = 0;
    svlod[0] = svlod1;
    svlod[1] = svlod2;
    svlod[2] = svlod3;
    svlod1->MeshType = 0;
    svlod1->LODName = objname;
    svlod1->StaticShape = 0;
    svlod1->unk2 = 0;
    svlod1->BoneShape = 0;
    svlod1->unk4 = 0;
    svlod1->fts = 0;
    svlod1->TextureData = 0;
    svlod1->unk7 = 1.0;
    svlod1->unk8 = 1.0;
    svlod1->unk9 = 0;
    svlod1->unk10 = 1.0;
    svlod1->unk11 = 0;
    svlod1->unk12 = 1.0;
    svlod1->distance = 40;
    svlod1->AnimationCount = 0;
    svlod1->unk14 = 0;
    svlod1->AnimationArray = 0;
    svlod2->MeshType = 0;
    svlod2->LODName = objname;
    svlod2->StaticShape = 0;
    svlod2->unk2 = 0;
    svlod2->BoneShape = 0;
    svlod2->unk4 = 0;
    svlod2->fts = 0;
    svlod2->TextureData = 0;
    svlod2->unk7 = 1.0;
    svlod2->unk8 = 1.0;
    svlod2->unk9 = 0;
    svlod2->unk10 = 1.0;
    svlod2->unk11 = 0;
    svlod2->unk12 = 1.0;
    svlod2->distance = 100;
    svlod2->AnimationCount = 0;
    svlod2->unk14 = 0;
    svlod2->AnimationArray = 0;
    svlod3->MeshType = 0;
    svlod3->LODName = objname;
    svlod3->StaticShape = 0;
    svlod3->unk2 = 0;
    svlod3->BoneShape = 0;
    svlod3->unk4 = 0;
    svlod3->fts = 0;
    svlod3->TextureData = 0;
    svlod3->unk7 = 1.0;
    svlod3->unk8 = 1.0;
    svlod3->unk9 = 0;
    svlod3->unk10 = 1.0;
    svlod3->unk11 = 0;
    svlod3->unk12 = 1.0;
    svlod3->distance = 4000;
    svlod3->AnimationCount = 0;
    svlod3->unk14 = 0;
    svlod3->AnimationArray = 0;


    ShapeSize = sizeof(StaticShape1);
    unsigned long TotalVertexCount=0;
    unsigned long TotalFaceCount=0;

    for (MeshItemsIterator = MeshItems.begin();MeshItemsIterator != MeshItems.end();MeshItemsIterator++) {
        libOVL_MeshInfo *meshInfo=MeshItems[curItem];
        libOVL_Mesh *obj = meshInfo->mesh;

        TotalVertexCount+=obj->vertexCount;
        TotalFaceCount+=obj->faceCount;
        ShapeSize+= sizeof(StaticShape2)+(obj->vertexCount*sizeof(VERTEX)) + ((obj->faceCount*3)*sizeof(unsigned long))+4;

    }

    shape = new unsigned char[ShapeSize];
    memset(shape,0,ShapeSize);
    sh = (StaticShape1 *)shape;
    sh2x = (StaticShape2 **)(shape + sizeof(StaticShape1));

    unsigned char *CurShape=0;
    CurShape = (shape + sizeof(StaticShape1) + 4);

    sh2 = (StaticShape2 *)(CurShape);

    *sh2x = sh2;

    //Assume the bounding box of the first mesh as that of the object
    libOVL_MeshInfo *meshInf=MeshItems[0];
    sh->BoundingBox2.x = meshInf->BoundingBox2.x;
    sh->BoundingBox2.y = meshInf->BoundingBox2.y;
    sh->BoundingBox2.z = meshInf->BoundingBox2.z;
    sh->BoundingBox1.x = meshInf->BoundingBox1.x;
    sh->BoundingBox1.y = meshInf->BoundingBox1.y;
    sh->BoundingBox1.z = meshInf->BoundingBox1.z;
    sh->TotalVertexCount = TotalVertexCount;
    sh->TotalIndexCount = TotalFaceCount*3;
    sh->MeshCount = 1;
    sh->MeshCount2 = 1;
    sh->sh = sh2x;
    sh->EffectCount = 0;
    sh->EffectPosition = 0;
    sh->EffectName = 0;


    //TOCHECK : how to handle multiple textures
    //Here I have assumed there will be only one texture
    libOVL_FlexiTextureInfo *ftx = FlexiTextureItems[0];

    unsigned char *texturedata = ftx->texturedata;
    FlexiTextureInfoStruct *fti = ftx->fti ;
    FlexiTextureStruct *fts = ftx->fts;

    unsigned long SymCount = 3;
    unsigned long LdrCount = 3;
    unsigned long SymRefCount = 5;

    Sym = new SymbolStruct[sizeof(SymbolStruct)*SymCount];
    memset(Sym,0,sizeof(SymbolStruct)*SymCount);
    //TOCHECK : how to handle multiple textures
    Sym[0].Symbol = texturename;
    Sym[0].data = (unsigned long *)texturedata;
    Sym[0].IsPointer = 1;
    Sym[1].Symbol = shapename;
    Sym[1].data = (unsigned long *)shape;
    Sym[1].IsPointer = 1;
    Sym[2].Symbol = sceneryname;
    Sym[2].data = (unsigned long *)svd;
    Sym[2].IsPointer = 1;
    Ldr = new LoaderStruct[sizeof(LoaderStruct)*LdrCount];
    memset(Ldr,0,sizeof(LoaderStruct)*LdrCount);
    Ldr[0].LoaderType = 0;
    //TOCHECK : how to handle multiple textures
    Ldr[0].data = (unsigned long *)texturedata;
    Ldr[0].HasExtraData = 0;
    Ldr[0].Sym = &Sym[0];
    Ldr[0].SymbolsToResolve = 0;
    Ldr[1].LoaderType = 1;
    Ldr[1].data = (unsigned long *)svd;
    Ldr[1].HasExtraData = 0;
    Ldr[1].Sym = &Sym[1];
    Ldr[1].SymbolsToResolve = 3;
    Ldr[2].LoaderType = 2;
    Ldr[2].data = (unsigned long *)shape;
    Ldr[2].HasExtraData = 0;
    Ldr[2].Sym = &Sym[2];
    Ldr[2].SymbolsToResolve = 2;
    Ref = new SymbolRefStruct[sizeof(SymbolRefStruct)*SymRefCount];
    memset(Ref,0,sizeof(SymbolRefStruct)*SymRefCount);
    Ref[0].ldr = &Ldr[1];
    Ref[0].Symbol = shapename;
    Ref[0].reference = (unsigned long *)&svlod1->StaticShape;
    Ref[1].ldr = &Ldr[1];
    Ref[1].Symbol = shapename;
    Ref[1].reference = (unsigned long *)&svlod2->StaticShape;
    Ref[2].ldr = &Ldr[1];
    Ref[2].Symbol = shapename;
    Ref[2].reference = (unsigned long *)&svlod3->StaticShape;
    Ref[3].ldr = &Ldr[2];
    Ref[3].Symbol = texturename;
    //TOCHECK : How to handle multiple meshes
    Ref[3].reference = (unsigned long *)&sh2->fts;
    Ref[4].ldr = &Ldr[2];
    Ref[4].Symbol = txsname;
    Ref[4].reference = (unsigned long *)&sh2->TextureData;

    for (MeshItemsIterator = MeshItems.begin();MeshItemsIterator != MeshItems.end();MeshItemsIterator++) {

        //continue
        libOVL_MeshInfo *meshInfo=MeshItems[curItem];
        libOVL_Mesh *obj = meshInfo->mesh;
        //May be for multiple mesh, this must be appended with mesh no

        sh2 = (StaticShape2 *)(CurShape);
        Vertexes = (VERTEX *)(CurShape + sizeof(StaticShape2));
        Triangles = (unsigned long *)(CurShape + sizeof(StaticShape2) + (obj->vertexCount*sizeof(VERTEX)) );

        for (i = 0;i < obj->vertexCount;i++) {
            Vertexes[i].position.x = obj->vertices[i].x;
            Vertexes[i].position.y = obj->vertices[i].y;
            Vertexes[i].position.z = obj->vertices[i].z;
            Vertexes[i].color = 0xFFFFFFFF;
            Vertexes[i].normal.x = obj->vertex_normals[i].x;
            Vertexes[i].normal.y = obj->vertex_normals[i].y;
            Vertexes[i].normal.z = obj->vertex_normals[i].z;
            Vertexes[i].tu = 0;
            Vertexes[i].tv = 0;
        }
        for (i = 0;i < obj->faceCount*3;i+=3) {
            Triangles[i] = obj->faces[i/3].vertex[0];
            Triangles[i+1] = obj->faces[i/3].vertex[1];
            Triangles[i+2] = obj->faces[i/3].vertex[2];
            Vertexes[obj->faces[i/3].vertex[0]].tu = obj->texture_coordinates[obj->faces[i/3].texture_coordinates[0]].x;
            Vertexes[obj->faces[i/3].vertex[0]].tv = obj->texture_coordinates[obj->faces[i/3].texture_coordinates[0]].y;
            Vertexes[obj->faces[i/3].vertex[1]].tu = obj->texture_coordinates[obj->faces[i/3].texture_coordinates[1]].x;
            Vertexes[obj->faces[i/3].vertex[1]].tv = obj->texture_coordinates[obj->faces[i/3].texture_coordinates[1]].y;
            Vertexes[obj->faces[i/3].vertex[2]].tu = obj->texture_coordinates[obj->faces[i/3].texture_coordinates[2]].x;
            Vertexes[obj->faces[i/3].vertex[2]].tv = obj->texture_coordinates[obj->faces[i/3].texture_coordinates[2]].y;
        }
        sh2->unk1 = 0xFFFFFFFF;
        sh2->fts = 0;
        sh2->TextureData = 0;
        sh2->PlaceTexturing = 0;
        sh2->textureflags = 0;
        sh2->unk4 = 3;
        sh2->VertexCount = obj->vertexCount;
        sh2->IndexCount = obj->faceCount*3;
        sh2->Vertexes = Vertexes;
        sh2->Triangles = Triangles;

        //TOCHECK : I am not sure whether 4 should be added here
        CurShape = (unsigned char*)(Triangles + 4);
        curItem++;
    }

    Type2Files[0].size = sizeof(SymbolStruct)*SymCount;
    Type2Files[0].reloffset = RelOffset;
    Type2Files[0].data = (unsigned long *)Sym;
    Type2Files[0].unk = 0;
    Type2Files[1].size = sizeof(LoaderStruct)*LdrCount;
    Type2Files[1].reloffset = Type2Files[0].size + Type2Files[0].reloffset;
    Type2Files[1].data = (unsigned long *)Ldr;
    Type2Files[1].unk = 0;
    Type2Files[2].size = sizeof(SymbolRefStruct)*SymRefCount;
    Type2Files[2].reloffset = Type2Files[1].size + Type2Files[1].reloffset;
    Type2Files[2].data = (unsigned long *)Ref;
    Type2Files[2].unk = 0;
    //TOCHECK: How to handle multiple textures
    //Type2Files[3].size = sizeof(colors)+sizeof(FlexiTextureInfoStruct)+sizeof(FlexiTextureStruct)+1;
    Type2Files[3].size = (sizeof(RGBQUAD)*256)+sizeof(FlexiTextureInfoStruct)+sizeof(FlexiTextureStruct)+1;
    Type2Files[3].reloffset = Type2Files[2].size + Type2Files[2].reloffset;
    //TOCHECK: How to handle multiple textures
    Type2Files[3].data = (unsigned long *)texturedata;
    Type2Files[3].unk = 0;
    Type2Files[4].size = scenerysize;
    Type2Files[4].reloffset = Type2Files[3].size + Type2Files[3].reloffset;
    Type2Files[4].data = (unsigned long *)svd;
    Type2Files[4].unk = 0;
    Type2Files[5].size = ShapeSize;
    Type2Files[5].reloffset = Type2Files[4].size + Type2Files[4].reloffset;
    Type2Files[5].data = (unsigned long *)shape;
    Type2Files[5].unk = 0;


    OpenFiles[CurrentFile].Types[2].count = 6;
    OpenFiles[CurrentFile].Types[2].dataptr = Type2Files;
    OpenFiles[CurrentFile].Types[2].size = 0;
    for (unsigned long i = 0;i < OpenFiles[CurrentFile].Types[2].count;i++) {
        OpenFiles[CurrentFile].Types[2].size += Type2Files[i].size;
    }

    RelOffset = Type2Files[OpenFiles[CurrentFile].Types[2].count-1].reloffset;

    relocations.push((unsigned long *)&Sym[0].Symbol);
    relocations.push((unsigned long *)&Sym[0].data);
    relocations.push((unsigned long *)&Sym[1].Symbol);
    relocations.push((unsigned long *)&Sym[1].data);
    relocations.push((unsigned long *)&Sym[2].Symbol);
    relocations.push((unsigned long *)&Sym[2].data);
    relocations.push((unsigned long *)&Ldr[0].data);
    relocations.push((unsigned long *)&Ldr[0].Sym);
    relocations.push((unsigned long *)&Ldr[1].data);
    relocations.push((unsigned long *)&Ldr[1].Sym);
    relocations.push((unsigned long *)&Ldr[2].data);
    relocations.push((unsigned long *)&Ldr[2].Sym);
    relocations.push((unsigned long *)&Ref[0].reference);
    relocations.push((unsigned long *)&Ref[0].Symbol);
    relocations.push((unsigned long *)&Ref[0].ldr);
    relocations.push((unsigned long *)&Ref[1].reference);
    relocations.push((unsigned long *)&Ref[1].Symbol);
    relocations.push((unsigned long *)&Ref[1].ldr);
    relocations.push((unsigned long *)&Ref[2].reference);
    relocations.push((unsigned long *)&Ref[2].Symbol);
    relocations.push((unsigned long *)&Ref[2].ldr);
    relocations.push((unsigned long *)&Ref[3].reference);
    relocations.push((unsigned long *)&Ref[3].Symbol);
    relocations.push((unsigned long *)&Ref[3].ldr);
    relocations.push((unsigned long *)&Ref[4].reference);
    relocations.push((unsigned long *)&Ref[4].Symbol);
    relocations.push((unsigned long *)&Ref[4].ldr);

    //TOCHECK : how to handle multiple textures
    relocations.push((unsigned long *)&fti->offset1);
    relocations.push((unsigned long *)&fti->fts2);
    relocations.push((unsigned long *)&fts->palette);
    relocations.push((unsigned long *)&fts->texture);
    relocations.push((unsigned long *)&fts->alpha);
    relocations.push((unsigned long *)&sv->LODMeshes);
    relocations.push((unsigned long *)&svlod[0]);
    relocations.push((unsigned long *)&svlod[1]);
    relocations.push((unsigned long *)&svlod[2]);
    relocations.push((unsigned long *)&svlod1->LODName);
    relocations.push((unsigned long *)&svlod2->LODName);
    relocations.push((unsigned long *)&svlod3->LODName);
    relocations.push((unsigned long *)&sh->sh);
    relocations.push((unsigned long *)&sh->EffectPosition);
    relocations.push((unsigned long *)&sh->EffectName);
    relocations.push((unsigned long *)sh->sh);
    //TOCHECK: How to handle multiple meshes
    relocations.push((unsigned long *)&sh2->Vertexes);
    relocations.push((unsigned long *)&sh2->Triangles);
    return RelOffset;
}


////////////////////////////////////////////////////////////////////////////////
//
//  AddOVLInfo_Mesh
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void AddOVLInfo_Mesh(libOVL_Mesh* mesh, char* meshname, char* textureIntOVLName,char* textureStyle,D3DVECTOR BoundingBox1,D3DVECTOR BoundingBox2) {
    long curItem=-1;
    libOVL_MeshInfo *t = new libOVL_MeshInfo;
    MeshItems.push_back(t);
    curItem = MeshItems.size()-1;
    libOVL_MeshInfo *s = MeshItems[curItem];

    s->mesh=new libOVL_Mesh;
    (*s->mesh) = (*mesh);
    s->mesh->texture_coordinates  =(libOVL_Vector3D*) new unsigned char[sizeof(libOVL_Vector3D)*(mesh->textureCoordinateCount)];
    memcpy(s->mesh->texture_coordinates,mesh->texture_coordinates,sizeof(libOVL_Vector3D)*(mesh->textureCoordinateCount));
    s->mesh->vertex_normals  =(libOVL_Vector3D*) new unsigned char[sizeof(libOVL_Vector3D)*(mesh->vertexCount)];
    memcpy(s->mesh->vertex_normals,mesh->vertex_normals,sizeof(libOVL_Vector3D)*(mesh->vertexCount));
    s->mesh->vertices  =(libOVL_Vector3D*) new unsigned char[sizeof(libOVL_Vector3D)*(mesh->vertexCount)];
    memcpy(s->mesh->vertices,mesh->vertices,sizeof(libOVL_Vector3D)*(mesh->vertexCount));

    s->BoundingBox1=BoundingBox1;
    s->BoundingBox2=BoundingBox2;

    s->meshname = new char[strlen(meshname)+1];
    strcpy(s->meshname,meshname);

    s->textureIntOVLName = new char[strlen(textureIntOVLName)+1];
    strcpy(s->textureIntOVLName,textureIntOVLName);

    s->textureStyle = new char[strlen(textureStyle)+1];
    strcpy(s->textureStyle,textureStyle);

}


////////////////////////////////////////////////////////////////////////////////
//
//  DeleteOVLInfo_Meshes
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void DeleteOVLInfo_Meshes() {
    for (unsigned long i = 0;i < MeshItems.size();i++) {
        delete MeshItems[i]->mesh;
        delete MeshItems[i]->meshname;
        delete MeshItems[i]->textureIntOVLName;
        delete MeshItems[i]->textureStyle;
        delete MeshItems[i];
    }

    MeshItems.clear();

}


////////////////////////////////////////////////////////////////////////////////
//
//  AddOVLInfo_FlexiTexture
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void AddOVLInfo_FlexiTexture(FlexiTextureStruct *fts, FlexiTextureInfoStruct *fti,RGBQUAD * Bitmapcolors, char* textureIntOVLName) {
    long curItem=-1;
    unsigned char *texturedata;
    unsigned long *ofs1;
    RGBQUAD colors[256];
    RGBQUAD *c;

    memcpy(colors,Bitmapcolors,sizeof(RGBQUAD)*256);

    libOVL_FlexiTextureInfo *t = new libOVL_FlexiTextureInfo;
    FlexiTextureItems.push_back(t);
    curItem = FlexiTextureItems.size()-1;

    libOVL_FlexiTextureInfo *s = FlexiTextureItems[curItem];

    texturedata = new unsigned char[sizeof(colors)+sizeof(FlexiTextureInfoStruct)+sizeof(FlexiTextureStruct)+4];
    memset(texturedata,0,sizeof(colors)+sizeof(FlexiTextureInfoStruct)+sizeof(FlexiTextureStruct)+4);
    s->fti = (FlexiTextureInfoStruct *)texturedata;
    s->fts = (FlexiTextureStruct *)(texturedata+sizeof(FlexiTextureInfoStruct)+4);
    c = (RGBQUAD *)(texturedata+sizeof(FlexiTextureInfoStruct)+sizeof(FlexiTextureStruct)+4);
    s->colors=c;
    s->texturedata=texturedata;

    ofs1 = (unsigned long *)(texturedata+sizeof(FlexiTextureInfoStruct));
    memcpy(c,colors,sizeof(RGBQUAD)*256);

    (*s->fts) =(*fts) ;
    s->fts->texture  = new unsigned char[fts->height * fts->width ];
    memcpy(s->fts->texture,fts->texture,fts->height * fts->width);

    s->fts->palette  = (unsigned char*)c;
    if (fts->alpha!=0) {
        s->fts->alpha  = new unsigned char[fts->height * fts->width];
        memcpy(s->fts->alpha,fts->alpha,fts->height * fts->width);
    } else
        s->fts->alpha=0;

    (*s->fti)=(*fti);
    s->fti->fts2=s->fts;
    s->fti->offset1 = ofs1;

    s->textureIntOVLName = new char[strlen(textureIntOVLName)+1];
    strcpy(s->textureIntOVLName,textureIntOVLName);

    unsigned long size=s->fti->width * s->fti->height;
    AddInfo_Resource(size,(unsigned long*)s->fts->texture);
}


////////////////////////////////////////////////////////////////////////////////
//
//  DeleteOVLInfo_FlexiTextures
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void DeleteOVLInfo_FlexiTextures() {
    for (unsigned long i = 0;i < FlexiTextureItems.size();i++) {
        delete FlexiTextureItems[i]->fts;
        delete FlexiTextureItems[i]->fti;
        delete FlexiTextureItems[i]->textureIntOVLName;
        delete FlexiTextureItems[i];
    }

    FlexiTextureItems.clear();
}
*/

