/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Triangle.h
 * 
 * Declaration of the Triangle class.
 */

#pragma once

#include "Ray.h"
#include "Primitive.h"
#include "Boundingbox.h"
#include "Model.h"
#include "Vertex3d.h"

class Scene;
class Vector3d;
class BoundingBox;

class Triangle : public Primitive, public Model
{
public:
    Triangle(const Vector3d&, const Vector3d&, const Vector3d&);
    Triangle(const Vertex3d&, const Vertex3d&, const Vertex3d&);
    Triangle();
    ~Triangle();

    BoundingBox GetBoundingBox() const;
    std::tuple<bool, BoundingBox> GetClippedBoundingBox(const BoundingBox& clipbox) const;
    
    double Intersect(const Ray& ray) const;
    bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    Vertex3d v0, v1, v2;
protected:
    friend class Scene;
    void AddToScene(Scene& scene);
};
