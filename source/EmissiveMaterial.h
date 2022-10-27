/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file EmissiveMaterial.h
 * 
 * Declaration of the EmissiveMaterial class.
 */

#pragma once

#include "GeometricRoutines.h"
#include "Randomizer.h"
#include "Material.h"

class Light;
class Sample;
class Vector3d;

class EmissiveMaterial : public Material
{
public:
    EmissiveMaterial();
    ~EmissiveMaterial();

    Sample GetSample(const IntersectionInfo& info, Randomizer& random, bool adjoint) const;

    Color BRDF(const IntersectionInfo& info, const Vector3d& out, int component) const;

    Light* GetLight() const;

    void ReadProperties(std::stringstream& ss);

    double PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const;

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    Color emissivity;
};

    