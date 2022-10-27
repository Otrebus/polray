/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Material.cpp
 * 
 * Implementation of the Material class.
 */

#include "material.h"
#include "AshikhminShirley.h"
#include "DielectricMaterial.h"
#include "EmissiveMaterial.h"
#include "LambertianMaterial.h"
#include "MirrorMaterial.h"
#include "PhongMaterial.h"
#include "Bytestream.h"


Material::Material()
{
    light = 0;
}

Material::~Material()
{
}

Material* Material::Create(unsigned char id)
{
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