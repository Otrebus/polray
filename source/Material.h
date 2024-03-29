/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Material.h
 * 
 * Declaration of the Material base class.
 */

#pragma once

#include "Color.h"

class Light;
class IntersectionInfo;
class Bytestream;
class Sample;
class Color;
class Random;
class Randomizer;

class Material
{
public:
    Material();
    virtual ~Material();

    virtual Sample GetSample(const IntersectionInfo& info, Randomizer& random, bool adjoint) const = 0;

    virtual Color BRDF(const IntersectionInfo& info, const Vector3d& out, int component) const = 0;

    virtual Light* GetLight() const = 0;

    virtual void ReadProperties(std::stringstream& ss) = 0;
    virtual double PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const = 0;

    virtual void Save(Bytestream& stream) const = 0;
    virtual void Load(Bytestream& stream) = 0;

    static Material* Create(unsigned char);

    Light* light;
};
