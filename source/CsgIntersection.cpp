#include "BoundingBox.h"
#include "CsgIntersection.h"
#include "Scene.h"
#include "Utils.h"

#include <algorithm>

CsgIntersection::CsgIntersection(CsgObject* a, CsgObject* b)
    : objA_(a->Clone()), objB_(b->Clone())
{
}

bool CsgIntersection::Intersect(const Ray& ray, 
                                CsgObject::hits& intersects) const
{
    CsgObject::hits hitsA, hitsB;
    objA_->Intersect(ray, hitsA);
    objB_->Intersect(ray, hitsB);
    CsgObject::hits allHits(hitsA.size() + hitsB.size());
    std::merge(hitsA.begin(), hitsA.end(), hitsB.begin(), hitsB.end(), 
               allHits.begin(), [](CsgHit& a, CsgHit& b) { return a.t < b.t; });

    int I = 0;
    auto func = [&intersects, &I](CsgHit& h) 
                { if(h.type == CsgHit::Enter && ++I == 2 
                  || h.type == CsgHit::Exit && --I == 1) 
                      intersects.push_back(h); };
    for_each(allHits.begin(), allHits.end(), func);

    return !intersects.empty();
}

BoundingBox CsgIntersection::GetBoundingBox() const
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

bool CsgIntersection::GetClippedBoundingBox(const BoundingBox& clipbox, 
                                            BoundingBox& resultbox) const
{
    resultbox = GetBoundingBox();
    return true;
}

float CsgIntersection::Intersect(const Ray& ray) const
{
    CsgObject::hits hits;
    Intersect(ray, hits);
    auto firstHit = std::find_if(hits.begin(), hits.end(), 
                    [] (CsgHit& a) { return (a.t > 0); });
    return firstHit != hits.end() ? (*firstHit).t : -inf;
}

bool CsgIntersection::GenerateIntersectionInfo(const Ray& ray, 
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

void CsgIntersection::Translate(const Vector3d& direction)
{
    objA_->Translate(direction);
    objB_->Translate(direction);
}

void CsgIntersection::Rotate(const Vector3d& axis, float angle)
{
    objA_->Rotate(axis, angle);
    objB_->Rotate(axis, angle);
}

void CsgIntersection::AddToScene(Scene& scene)
{
    Scene::PrimitiveAdder::AddPrimitive(scene, this);
}

void CsgIntersection::Save(Bytestream& stream) const
{
}

void CsgIntersection::Load(Bytestream& stream)
{
}

std::unique_ptr<CsgObject> CsgIntersection::Clone()
{
    return std::unique_ptr<CsgObject>(new CsgIntersection(&*objA_, &*objB_));
}