#ifndef XRCRES_H_INCLUDED
#define XRCRES_H_INCLUDED

//void InitXmlCreateScenery();
void InitXmlEffect();
void InitXmlInfo();
//void InitXmlLOD();
void InitXmlMatrix();
//void InitXmlMesh();
void InitXmlModel();
void InitXmlTexture();
void InitXmlTextureSimple();
void InitXmlEffectMenu();
void InitXmlStallMan();
void InitXmlStall();
void InitXmlAttractMan();
void InitXmlAttract();
void InitXmlBitmapCrystal();
void InitXmlBitmapIcons();
//void InitXmlScenerySettings();
//void InitXmlAnimation();
//void InitXmlMisc();

inline void InitXmlResource() {
//    InitXmlCreateScenery();
    InitXmlEffect();
    InitXmlInfo();
//    InitXmlLOD();
    InitXmlMatrix();
//    InitXmlMesh();
    InitXmlModel();
    InitXmlTexture();
    InitXmlTextureSimple();
    InitXmlEffectMenu();
    InitXmlStallMan();
    InitXmlStall();
    InitXmlAttractMan();
    InitXmlAttract();
    InitXmlBitmapCrystal();
    InitXmlBitmapIcons();
//    InitXmlScenerySettings();
//    InitXmlAnimation();
//    InitXmlMisc();
}

#endif
