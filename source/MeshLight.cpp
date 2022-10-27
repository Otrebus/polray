/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file MeshLight.cpp
 * 
 * Implementation of the MeshLight class.
 */

#include "Color.h"
#include "EmissiveMaterial.h"
#include "MeshLight.h"
#include "Renderer.h"
#include "TriangleMesh.h"
#include "Utils.h"
#include <string>
#include <vector>

MeshLight::MeshLight(Color intensity, std::string fileName)
{
#ifdef DETERMINISTIC
    r.Seed(0);
#endif
    material = new EmissiveMaterial();
    material->light = this;
    material->emissivity = intensity;
    mesh = new TriangleMesh(fileName, material);
    this->intensity = intensity;

    area_ = 0;
    for(auto it = mesh->triangles.cbegin(); it < mesh->triangles.cend(); it++)
        area_ += (*it)->GetArea();

    triangleTree_ = BuildTree(0, (int) mesh->triangles.size() - 1, area_, 0);
    std::vector<const Primitive*> v;
    for(auto t : mesh->triangles)
        v.push_back(t);
    mesh->materials.push_back(material);
    tree.Build(v);
    builtTree = true;
}

/**
 * Constructor.
 * 
 * @param intensity The radiance that the light lets out in every direction.
 */
MeshLight::MeshLight(Color intensity)
{
#ifdef DETERMINISTIC
    r.Seed(0);
#endif
    material = new EmissiveMaterial();
    material->light = this;
    material->emissivity = intensity;
    mesh = new TriangleMesh();
    mesh->materials.push_back(material);
    this->intensity = intensity;
    builtTree = false;
}

/**
 * Constructor.
 */
MeshLight::MeshLight()
{
}

/**
 * Picks a random triangle inside the mesh light weighted by area.
 * 
 * @param rnd The randomizer used to generate random numbers.
 */
MeshTriangle* MeshLight::PickRandomTriangle(Randomizer& rnd) const
{
    if(!builtTree)
    {
        area_ = 0;
        for(auto it = mesh->triangles.cbegin(); it < mesh->triangles.cend(); it++)
            area_ += (*it)->GetArea();
        triangleTree_ = BuildTree(0, (int) mesh->triangles.size() - 1, area_, 0);
        std::vector<const Primitive*> v;
        for(auto t : mesh->triangles)
            v.push_back(t);
        tree.Build(v);
        builtTree = true;
    }
    double f = rnd.GetDouble(0, area_);

    TriangleNode* node = triangleTree_;
    while(true)
        if(node->triangle)
            return node->triangle;
        else if(f < node->cutoff)
            node = node->leftChild;
        else
            node = node->rightChild;
}

/**
 * Builds the acceleration tree structure of the mesh light that speeds up triangle picking.
 * 
 * @param from The first triangle to put into the tree.
 * @param to The first triangle to not put into the tree.
 * @param area The total area of the triangles we put into the tree.
 * @param areaStart The total area of the triangles before 'from'.
 * 
 * @returns A pointer to the root of the tree.
 */
TriangleNode* MeshLight::BuildTree(int from, int to, double area, double areaStart) const
{
    std::vector<MeshTriangle*>& triangles = mesh->triangles;
    if(from == to)
    {
        TriangleNode* n = new TriangleNode;
        n->triangle = triangles[from];
        return n;
    }

    double areaSum = 0;
    int halfIndex = 0;
    // First, find out where the halfway area mark is
    for(int i = from; i < to; i++)
    {
        MeshTriangle* t = triangles[i];
        areaSum += t->GetArea();
        halfIndex = i;
        if(areaSum > area/2)
            break;
    }

    TriangleNode* node = new TriangleNode;

    node->leftChild = BuildTree(from, halfIndex, areaSum, areaStart);
    node->rightChild = BuildTree(halfIndex+1, to, area - areaSum, areaStart + areaSum);
    node->triangle = 0;
    node->cutoff = areaStart + areaSum;
    return node;
}

/**
 * Samples an outgoing ray from the light.
 * 
 * @param rnd The randomizer to sample with.
 * @returns A tuple of the outgoing ray, its sampled color and normal and the area and angle pdfs.
 */
std::tuple<Ray, Color, Normal, AreaPdf, AnglePdf> MeshLight::SampleRay(Randomizer& rnd) const
{
    Ray ray;
    double areaPdf, anglePdf;

    auto [point, normal] = SamplePoint(rnd);
    ray.origin = point;

    auto [right, forward] = MakeBasis(normal);
    double r1 = rnd.GetDouble(0, 1), r2 = rnd.GetDouble(0, 1);
    ray.direction = SampleHemisphereCos(r1, r2, normal);

    anglePdf = abs(ray.direction*normal)/pi;
    areaPdf = 1.0/GetArea();

    return { ray, Color::Identity*pi, normal, areaPdf, anglePdf };
}

/**
 * Destructor.
 */
MeshLight::~MeshLight()
{
}

/**
 * Checks if and where the given ray intersects the light.
 * 
 * @param ray The ray to check against the light.
 * @returns The distance along the ray that the light source was hit.
 */
double MeshLight::Intersect(const Ray&) const
{    
    /*if(!builtTree) {
        area_ = 0;
        for(auto it = mesh->triangles.cbegin(); it < mesh->triangles.cend(); it++)
            area_ += (*it)->GetArea();
        triangleTree_ = BuildTree(0, mesh->triangles.size() - 1, area_, 0);
        std::vector<const Primitive*> v;
        for(auto t : mesh->triangles)
            v.push_back(t);
        tree.Build(v);
        builtTree = true;
    }
    const Primitive* prim;
    return tree.Intersect(ray, prim, 0, inf, false);*/

    // Intersecting against lights might need a rethink
    return -inf;
}

/**
 * Returns the intersection info of a ray that hit the light.
 * 
 * @param ray The ray that hit the light.
 * @param info The intersection info to fill in.
 * @returns Whether the area light was actually hit by the ray.
 */
bool MeshLight::GenerateIntersectionInfo(const Ray&, IntersectionInfo&) const
{
    /*if(!builtTree) {
        area_ = 0;
        for(auto it = mesh->triangles.cbegin(); it < mesh->triangles.cend(); it++)
            area_ += (*it)->GetArea();
        triangleTree_ = BuildTree(0, mesh->triangles.size() - 1, area_, 0);
        std::vector<const Primitive*> v;
        for(auto t : mesh->triangles)
            v.push_back(t);
        tree.Build(v);
        builtTree = true;
    }

    const Primitive* prim;
    tree.Intersect(ray, prim, 0, inf, true);
    return prim->GenerateIntersectionInfo(ray, info);*/
    return false;
}

/**
 * Returns the value of the angle pdf of a ray that was sampled at the light source.
 * 
 * @param info Contains the point that we evaluate the pdf at.
 * @param out The outgoing vector at that point.
 * @returns The value of the angle pdf at the given point and outgoing vector.
 */
double MeshLight::Pdf(const IntersectionInfo& info, const Vector3d& out) const
{
    return out*info.geometricnormal/pi;
}

/**
 * Samples a random point of the area light.
 * 
 * @param rnd The randomizer to sample with.
 * @returns A tuple of the point and its normal.
 */
std::tuple<Point, Normal> MeshLight::SamplePoint(Randomizer& rnd) const
{
    MeshTriangle* t = PickRandomTriangle(rnd);

    double u = sqrt(rnd.GetDouble(0, 1));
    double v = rnd.GetDouble(0, 1);

    Vector3d e1 = t->v1->pos - t->v0->pos;
    Vector3d e2 = t->v2->pos - t->v0->pos;

    auto normal = t->GetNormal();
    auto point = t->v0->pos + u*(e1 + v*(e2-e1)) + eps*normal;

    return { point, normal };
}

/**
 * Performs a linear transform of the model of the light.
 * 
 * @param m The matrix that describes the transformation.
 */
void MeshLight::Transform(const Matrix3d& m)
{
    mesh->Transform(m);
}

/**
 * Saves the light source to a stream.
 * 
 * @param stream The stream that we serialize to.
 */
void MeshLight::Save(Bytestream& stream) const
{
    stream << (unsigned char)ID_MESHLIGHT;
    mesh->Save(stream);
    stream << intensity;
}

/**
 * Loads the light source from a stream.
 * 
 * @param stream The stream that we deserialize from.
 */
void MeshLight::Load(Bytestream& stream)
{
    mesh = new TriangleMesh();
    unsigned char dummy;
    stream >> dummy;
    mesh->Load(stream);
    mesh->materials[0]->light = this;
    stream >> intensity;
}

/**
 * Returns the area of the light.
 * 
 * @returns The area of the light.
 */
double MeshLight::GetArea() const
{
    if(!builtTree) {
        area_ = 0;
        for(auto it = mesh->triangles.cbegin(); it < mesh->triangles.cend(); it++)
            area_ += (*it)->GetArea();

        triangleTree_ = BuildTree(0, (int) mesh->triangles.size() - 1, area_, 0);
        builtTree = true;
    }
    return area_;
}

/**
 * Adds the light to a scene.
 * 
 * @param scn The scene to add the light to.
 */
void MeshLight::AddToScene(Scene* scn)
{
    for(auto& t : mesh->triangles)
        Scene::PrimitiveAdder::AddPrimitive(*scn, t);
    Scene::LightAdder::AddLight(*scn, this);
}

/**
 * Estimates the integral of the rendering equation in the solid angle area that this light spans
 * on the surface of the given intersection info.
 * 
 * @param renderer The renderer that calculates the next event estimation.
 * @param info The intersection info at the point whose rendering equation integral we calculate.
 * @param rnd The randomizer.
 * @param component The component of the brdf.
 * @returns A tuple of the estimate and the point estimated on the light source.
 */
std::tuple<Color, Point> MeshLight::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Randomizer& rnd, int component) const
{
    auto [lightPoint, lightNormal] = SamplePoint(rnd);
    Vector3d toLight = lightPoint - info.position;
    Vector3d normal = info.normal;

    if(toLight*lightNormal < 0)
    {
        double d = toLight.Length();
        toLight.Normalize();

        Ray lightRay = Ray(info.position, toLight);

        if(renderer->TraceShadowRay(lightRay, (1-eps)*d))
        {
            double cosphi = abs(normal*toLight);
            double costheta = abs(toLight*lightNormal);
            Color c;
            c = info.material->BRDF(info, toLight, component)
                *costheta*cosphi*intensity*GetArea()/(d*d);
            return { c, lightPoint };
        }
    }
    return { Color(0, 0, 0), lightPoint };
}
