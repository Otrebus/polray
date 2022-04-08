#include "material.h"
#include "AshikhminShirley.h"
#include "DielectricMaterial.h"
#include "EmissiveMaterial.h"
#include "LambertianMaterial.h"
#include "MirrorMaterial.h"
#include "PhongMaterial.h"

int Material::highestId = 0;

Material::Material()
{
    id = highestId++;
    texture = 0;
    normalmap = 0;
    light = 0;
}

Material::~Material()
{
    if(normalmap)
        delete normalmap;
    if(texture)
        delete texture;
}

Material* Material::Create(unsigned char id)
{
    // This is sort of unaesthetic
    switch(id)
    {
    case ID_LAMBERTIANMATERIAL:
        return new LambertianMaterial;
        break;
    case ID_PHONGMATERIAL:
        return new PhongMaterial;
        break;
    case ID_DIELECTRICMATERIAL:
        return new DielectricMaterial;
        break;
    case ID_MIRRORMATERIAL:
        return new MirrorMaterial;
        break;
    case ID_ASHIKHMINSHIRLEY:
        return new AshikhminShirley;
        break;
    case ID_EMISSIVEMATERIAL:
        return new EmissiveMaterial;
        break;
    default:
        return 0;
        break;
    }

}