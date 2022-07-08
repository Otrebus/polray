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
    r.Seed(GetTickCount() + (int) this);
    material = new EmissiveMaterial();
    material->light = this;
    mesh = new TriangleMesh(fileName, material);
    intensity_ = intensity;

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

MeshLight::MeshLight(Color intensity)
{
    r.Seed(GetTickCount() + (int) this);
    material = new EmissiveMaterial();
    material->light = this;
    mesh = new TriangleMesh();
    intensity_ = intensity;
    builtTree = false;
}

MeshTriangle* MeshLight::PickRandomTriangle() const
{
    if(!builtTree) {
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
    assert(from <= to);
    std::vector<MeshTriangle*>& triangles = mesh->triangles;
    if(from == to)
    {
        TriangleNode* n = new TriangleNode;
        n->triangle = triangles[from];
        return n;
    }

    double areaSum = triangles[from]->GetArea();
    int halfIndex = 0;
    // First, find out where the halfway area mark is
    for(int i = from + 1; i <= to; i++)
    {
        MeshTriangle* t = triangles[i];
        halfIndex = i;
        if(areaSum + t->GetArea() > area/2)
            break;
        areaSum += t->GetArea();
    }

    TriangleNode* node = new TriangleNode;

    node->leftChild = BuildTree(from, halfIndex - 1, areaSum, areaStart);
    node->rightChild = BuildTree(halfIndex, to, area - areaSum, areaStart + areaSum);
    node->triangle = 0;
    node->cutoff = areaStart + areaSum;
    return node;
}

Color MeshLight::SampleRay(Ray& ray, Vector3d& normal, double& areaPdf, double& anglePdf) const
{
    MeshTriangle* t = mesh->triangles[r.GetInt(0, mesh->triangles.size()-1)];
    SamplePoint(ray.origin, normal);

    Vector3d right, forward;
    MakeBasis(normal, right, forward);

    areaPdf = 1.0f/GetArea();

    double r1 = r.GetDouble(0, 2*F_PI);
    double r2 = r.GetDouble(0, 0.9999f);
    ray.direction =  forward*cos(r1)*sqrt(r2) + right*sin(r1)*sqrt(r2) 
        + normal * sqrt(1-r2);
    anglePdf = abs(ray.direction*normal)/(F_PI);
    return Color(1, 1, 1)*(F_PI);
}


MeshLight::~MeshLight()
{
}

double MeshLight::Intersect(const Ray& ray) const
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

bool MeshLight::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const
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
    Ray ray(info.position, out);
    return ray.direction*info.geometricnormal/F_PI;
}

void MeshLight::SamplePoint(Vector3d& point, Vector3d& normal) const
{
    MeshTriangle* t = PickRandomTriangle();
    double u = sqrt(r.GetDouble(0, 1));
    double v = r.GetDouble(0, 1);
    Vector3d e1 = t->v1->pos - t->v0->pos;
    Vector3d e2 = t->v2->pos - t->v0->pos;
    normal = t->GetNormal();
    point = t->v0->pos + u*(e1 + v*(e2-e1)) + 0.0001f*normal;
}

void MeshLight::Transform(const Matrix3d& m)
{
    mesh->Transform(m);
}

void MeshLight::Save(Bytestream& s) const
{

}
void MeshLight::Load(Bytestream& s)
{

}

double MeshLight::GetArea() const
{
    if(!builtTree) {
        area_ = 0;
        for(auto it = mesh->triangles.cbegin(); it < mesh->triangles.cend(); it++)
            area_ += (*it)->GetArea();

        triangleTree_ = BuildTree(0, mesh->triangles.size() - 1, area_, 0);
    }
    return area_;
}

void MeshLight::AddToScene(Scene* scene)
{
    mesh->AddToScene(*scene);
    Scene::LightAdder::AddLight(*scene, this);
}

Color MeshLight::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Vector3d& lightPoint, Vector3d& lightNormal, int component) const
{
    Vector3d lightPoint_, lightNormal_;
    SamplePoint(lightPoint_, lightNormal_);
    Vector3d toLight = lightPoint_ - info.GetPosition();
    double d = toLight.GetLength();
    Vector3d normal = info.GetNormal();

    //if(toLight*lightNormal_ < 0)
    if(toLight*lightNormal_ > 0)
        lightNormal = -lightNormal;
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

Color MeshLight::NextEventEstimationMIS(const Renderer* renderer, const IntersectionInfo& info, int component) const
{
    Vector3d lightPoint, lightNormal;
    MeshTriangle* t = PickRandomTriangle();
    double u = sqrt(r.GetDouble(0, 1));
    double v = r.GetDouble(0, 1);
    Vector3d e1 = t->v1->pos - t->v0->pos;
    Vector3d e2 = t->v2->pos - t->v0->pos;
    auto nr = t->GetNormal();
    auto point = t->v0->pos + u*(e1 + v*(e2-e1)) + 0.0001f*nr;
    auto weight = t->GetArea()/area_;
    Vector3d toLight = lightPoint - info.GetPosition();
    Vector3d normal = info.GetNormal();

    if(toLight*lightNormal < 0)
    {
        double d = toLight.GetLength();
        toLight.Normalize();
        Ray lightRay = Ray(info.GetPosition(), toLight);

        if(renderer->TraceShadowRay(lightRay, d))
        {
            double cosphi = abs(normal*toLight);
            double costheta = abs(toLight*lightNormal);
            Material* mat = info.GetMaterial();
            Color c = mat->BRDF(info, toLight, component)
                      *costheta*cosphi*intensity_*GetArea()/(d*d);
            double brdfPdf = costheta*mat->PDF(info, toLight, false, component)/(d*d);
            double lightPdf = 1.0f/GetArea();
            return c/(1.0f + brdfPdf*brdfPdf/(lightPdf*lightPdf));
        }
    }
    return Color(0, 0, 0);
}


Color MeshLight::GetIntensity() const
{
    return intensity_;
}

Color MeshLight::DirectHitMIS(const Renderer* renderer, 
                                const IntersectionInfo& lastInfo, 
                                const IntersectionInfo& thisInfo, int component) const
{
    Vector3d v = thisInfo.position - lastInfo.position;
    double d = v.GetLength();
    v.Normalize();
    double costheta = abs(v*thisInfo.normal);
    double lightPdf = 1.0f/GetArea();
    Material* mat = lastInfo.GetMaterial();
    double brdfPdf = costheta*mat->PDF(lastInfo, v, false, component)/(d*d);
    return intensity_/(1.0f + lightPdf*lightPdf/(brdfPdf*brdfPdf));
}