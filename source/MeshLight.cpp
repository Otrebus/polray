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
    intensity_ = intensity;

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
    intensity_ = intensity;
    builtTree = false;
}

MeshLight::MeshLight()
{
}

MeshTriangle* MeshLight::PickRandomTriangle() const
{
    if(!builtTree) {
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
    double f = r.GetDouble(0, area_);

    TriangleNode* node = triangleTree_;
    while(true)
        if(node->triangle)
            return node->triangle;
        else if(f < node->cutoff)
            node = node->leftChild;
        else
            node = node->rightChild;
}

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

std::tuple<Ray, Color, Vector3d, AreaPdf, AnglePdf> MeshLight::SampleRay() const
{
    Ray ray;
    double areaPdf, anglePdf;

    auto [point, normal] = SamplePoint();
    ray.origin = point;

    auto [right, forward] = MakeBasis(normal);
    double r1 = r.GetDouble(0, 1), r2 = r.GetDouble(0, 1);
    ray.direction = SampleHemisphereCos(r1, r2, normal);

    anglePdf = abs(ray.direction*normal)/pi;
    areaPdf = 1.0f/GetArea();

    return { ray, Color::Identity*pi, normal, areaPdf, anglePdf };
}

MeshLight::~MeshLight()
{
}

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

double MeshLight::Pdf(const IntersectionInfo& info, const Vector3d& out) const
{
    return out*info.geometricnormal/F_PI;
}

std::tuple<Point, Normal> MeshLight::SamplePoint() const
{
    MeshTriangle* t = PickRandomTriangle();

    double u = sqrt(r.GetDouble(0, 1));
    double v = r.GetDouble(0, 1);

    Vector3d e1 = t->v1->pos - t->v0->pos;
    Vector3d e2 = t->v2->pos - t->v0->pos;

    auto normal = t->GetNormal();
    auto point = t->v0->pos + u*(e1 + v*(e2-e1)) + eps*normal;

    return { point, normal };
}

void MeshLight::Transform(const Matrix3d& m)
{
    mesh->Transform(m);
}

void MeshLight::Save(Bytestream& stream) const
{
    // This could also be a hash table, for very large triangle meshes
    stream << (unsigned char)ID_MESHLIGHT;
    mesh->Save(stream);
    stream << intensity_;
}

void MeshLight::Load(Bytestream& stream)
{
    mesh = new TriangleMesh();
    unsigned char dummy;
    stream >> dummy;
    mesh->Load(stream);
    mesh->materials[0]->light = this;
    stream >> intensity_;
}

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

void MeshLight::AddToScene(Scene* scn)
{
    for(auto& t : mesh->triangles)
		Scene::PrimitiveAdder::AddPrimitive(*scn, t);
    Scene::LightAdder::AddLight(*scn, this);
}

Color MeshLight::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Vector3d&, Vector3d&, int component) const
{
    auto [lightPoint_, lightNormal_] = SamplePoint();
    Vector3d toLight = lightPoint_ - info.GetPosition();
    Vector3d normal = info.GetNormal();

    if(toLight*lightNormal_ < 0)
    {
        double d = toLight.GetLength();
        toLight.Normalize();

        Ray lightRay = Ray(info.GetPosition(), toLight);

        if(renderer->TraceShadowRay(lightRay, (1-1e-6)*d))
        {
            double cosphi = abs(normal*toLight);
            double costheta = abs(toLight*lightNormal_);
            Color c;
            c = info.GetMaterial()->BRDF(info, toLight, component)
                *costheta*cosphi*intensity_*GetArea()/(d*d);
            return c;
        }
    }
    return Color(0, 0, 0);
}

Color MeshLight::GetIntensity() const
{
    return intensity_;
}
