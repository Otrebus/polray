/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file CsgDifference.cpp
 * 
 * Implementation of the CsgDifference class used in constructive solid geometry.
 */

#include "BoundingBox.h"
#include "CsgDifference.h"
#include "Scene.h"
#include "Utils.h"

#include <algorithm>

CsgDifference::CsgDifference(CsgObject* a, CsgObject* b)
    : objA_(a->Clone()), objB_(b->Clone())
{
}

bool CsgDifference::Intersect(const Ray& ray, CsgObject::hits& intersects) const
{
    CsgObject::hits hitsA, hitsB;
    objA_->Intersect(ray, hitsA);
    objB_->Intersect(ray, hitsB);
    CsgObject::hits allHits(hitsA.size() + hitsB.size());

    for(auto& hit : hitsA)
        hit.object = objA_.get();

    for(auto& hit : hitsB)
        hit.object = objB_.get();

    std::merge(hitsA.begin(), hitsA.end(), hitsB.begin(), hitsB.end(), 
               allHits.begin(), [](CsgHit& a, CsgHit& b) { return a.t < b.t; });

    int I = 0;
    auto func = [&intersects, &I, this](CsgHit& h)  {
        if(h.object == objA_.get())
            I += (h.type == CsgHit::Enter ? 1 : -1);
        if(h.object == objB_.get())
            I += (h.type == CsgHit::Enter ? -1 : 1);
        if(I > 0)
            intersects.push_back(h);
        h.object = this;
    };
    for_each(allHits.begin(), allHits.end(), func);

    return !intersects.empty();
}

BoundingBox CsgDifference::GetBoundingBox() const
{
    BoundingBox boxA = objA_->GetBoundingBox();
    BoundingBox boxB = objB_->GetBoundingBox();
    Vector3d c1(min(boxA.c1.x, boxB.c1.x), 
                min(boxA.c1.y, boxB.c1.y), 
                min(boxA.c1.z, boxB.c1.z));
    Vector3d c2(max(boxA.c2.x, boxB.c2.x), 
                max(boxA.c2.y, boxB.c2.y), 
                max(boxA.c2.z, boxB.c2.z));
    return BoundingBox(c1, c2);
}

std::tuple<bool, BoundingBox> CsgDifference::GetClippedBoundingBox(const BoundingBox&) const
{
    return { true, GetBoundingBox() };
}

double CsgDifference::Intersect(const Ray& ray) const
{
    CsgObject::hits hits;
    Intersect(ray, hits);
    auto firstHit = std::find_if(hits.begin(), hits.end(), 
                    [] (CsgHit& a) { return (a.t > 0); });
    return firstHit != hits.end() ? (*firstHit).t : -inf;
}

bool CsgDifference::GenerateIntersectionInfo(const Ray& ray, 
                                               IntersectionInfo& info) const
{
    CsgObject::hits hits;
    if(!Intersect(ray, hits))
        return false;
    auto firstHit = std::find_if(hits.begin(), hits.end(), 
                    [] (CsgHit& a) { return (a.t > 0); });
    if(firstHit == hits.end())
        return false;
    info = (*firstHit).info;
    return true;
}

void CsgDifference::Translate(const Vector3d& direction)
{
    objA_->Translate(direction);
    objB_->Translate(direction);
}

void CsgDifference::Rotate(const Vector3d& axis, double angle)
{
    objA_->Rotate(axis, angle);
    objB_->Rotate(axis, angle);
}

void CsgDifference::AddToScene(Scene& scene)
{
    Scene::PrimitiveAdder::AddPrimitive(scene, this);
}

void CsgDifference::Save(Bytestream&) const
{
}

void CsgDifference::Load(Bytestream&)
{
}

std::unique_ptr<CsgObject> CsgDifference::Clone()
{
    return std::unique_ptr<CsgObject>(new CsgDifference(&*objA_, &*objB_));
}