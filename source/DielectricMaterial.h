/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file DielectricMaterial.h
 * 
 * Declaration of the DielectricMaterial class.
 */

#pragma once

#include "GeometricRoutines.h"
#include "Randomizer.h"
#include "Material.h"

class Light;
class Sample;
class Vector3d;

class DielectricMaterial : public Material
{
public:
    DielectricMaterial();
    ~DielectricMaterial();

    Sample GetSample(const IntersectionInfo& info, Randomizer& random, bool adjoint) const;

    Color BRDF(const IntersectionInfo& info, const Vector3d& out, int component) const;

    Light* GetLight() const;
    
    virtual double PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const;

    void ReadProperties(std::stringstream& ss);

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    double m_ior; // Index of refraction
};

    