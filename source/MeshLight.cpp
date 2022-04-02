#include "Color.h"
#include "EmissiveMaterial.h"
#include "MeshLight.h"
#include "Renderer.h"
#include "TriangleMesh.h"
#include <string>
#include <vector>

MeshLight::MeshLight(Color intensity, std::string fileName)
{
    r.Seed(GetTickCount() + (int) this);
    mat_ = new EmissiveMaterial();
    mat_->light = this;
    mesh_ = new TriangleMesh(fileName, mat_);
    intensity_ = intensity;

    area_ = 0;
    for(auto it = mesh_->triangles.cbegin(); it < mesh_->triangles.cend(); it++)
        area_ += (*it)->GetArea();

    triangleTree_ = BuildTree(0, mesh_->triangles.size() - 1, area_, 0);
}

MeshTriangle* MeshLight::PickRandomTriangle() const
{
    float f = r.GetFloat(0, area_);
    TriangleNode* node = triangleTree_;
    while(true)
        if(node->triangle)
            return node->triangle;
        else if(f < node->cutoff)
            node = node->leftChild;
        else
            node = node->rightChild;
}

TriangleNode* MeshLight::BuildTree(int from, int to, float area, float areaStart)
{
    assert(from <= to);
    std::vector<MeshTriangle*>& triangles = mesh_->triangles;
    if(from == to)
    {
        TriangleNode* n = new TriangleNode;
        n->triangle = triangles[from];
        return n;
    }

    float areaSum = triangles[from]->GetArea();
    int halfIndex = 0;
    // First, find out where the halfway area mark is
    for(int i = from + 1; i <= to; i++)
    {
        MeshTriangle* t = triangles[i];
        halfIndex = i;
        if(areaSum > area/2)
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

Color MeshLight::SampleRay(Ray& ray, Vector3d& normal, float& areaPdf, float& anglePdf) const
{
    MeshTriangle* t = mesh_->triangles[r.GetInt(0, mesh_->triangles.size()-1)];
    SamplePoint(ray.origin, normal);

    Vector3d right, forward;
    MakeBasis(normal, right, forward);

    areaPdf = 1.0f/GetArea();

    if(!portals.empty())
    {
        LightPortal p = portals.front();
        Vector3d portalNormal = p.v1^p.v2;
        portalNormal.Normalize();
        float x = r.GetFloat(0, 0.9999f);
	    float y = r.GetFloat(0, 0.9999f);

        Vector3d portalPos = p.pos + p.v1*x + p.v2*y;

        ray.direction = portalPos - ray.origin;
        float d = ray.direction.GetLength();
        ray.direction.Normalize();
        anglePdf = (1/p.GetArea())*d*d/(abs(p.GetNormal()*ray.direction));
        return Color(1, 1, 1)*abs(ray.direction*normal)/anglePdf;
    }

    float r1 = r.GetFloat(0, 2*F_PI);
    float r2 = r.GetFloat(0, 0.9999f);
 	ray.direction =  forward*cos(r1)*sqrt(r2) + right*sin(r1)*sqrt(r2) 
        + normal * sqrt(1-r2);
    anglePdf = abs(ray.direction*normal)/(F_PI);
	return Color(1, 1, 1)*(F_PI);
}


MeshLight::~MeshLight()
{
}

float MeshLight::Pdf(const IntersectionInfo& info, const Vector3d& out) const
{
    Ray ray(info.position, out);
    if(!portals.empty())
    {   // Intersect the portal with the ray - this is almost
        // the same code as ordinary triangle intersection
        LightPortal p = portals.front();
       	float u, v, t;
	    Vector3d D;

	    D.x = ray.direction.x;
	    D.y = ray.direction.y;
	    D.z = ray.direction.z;

	    Vector3d E1 = p.v1;
	    Vector3d E2 = p.v2;
	    Vector3d T = ray.origin - p.pos;

	    Vector3d P = E2^T;
	    Vector3d Q = E1^D;

	    float det = E2*Q;
	    if(det < 0.0000000001f && det > -0.0000000001f)
		    return 0;

	    u = D*P/det;

	    if(u > 1 || u < 0)
		    return 0;

	    v = T*Q/det;

	    if(v > 1 || v < 0)
		    return 0;

	    t = E1*P/det;
        if(t < 0)
            return 0;
        return (1/p.GetArea())*t*t/(abs(p.GetNormal()*ray.direction));
    }
    return ray.direction*info.geometricnormal/F_PI;
}

float MeshLight::SamplePoint(Vector3d& point, Vector3d& normal) const
{
    MeshTriangle* t = PickRandomTriangle();
    float u = sqrt(r.GetFloat(0, 1));
    float v = r.GetFloat(0, 1);
    Vector3d e1 = t->v1->pos - t->v0->pos;
    Vector3d e2 = t->v2->pos - t->v0->pos;
    normal = t->GetNormal();
    point = t->v0->pos + u*(e1 + v*(e2-e1)) + 0.0001f*normal;
    return t->GetArea()/area_;
}

void MeshLight::Transform(const Matrix3d& m)
{
    mesh_->Transform(m);
}

void MeshLight::Save(Bytestream& s) const
{

}
void MeshLight::Load(Bytestream& s)
{

}

float MeshLight::GetArea() const
{
    return area_;
}

void MeshLight::AddToScene(std::shared_ptr<Scene> scn)
{
    mesh_->AddToScene(*scn);
    Scene::LightAdder::AddLight(*scn, this);
}

Color MeshLight::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, unsigned int component) const
{
    Vector3d lightPoint, lightNormal;
    SamplePoint(lightPoint, lightNormal);
    Vector3d toLight = lightPoint - info.GetPosition();
    float d = toLight.GetLength();
    Vector3d normal = info.GetNormal();

    if(toLight*lightNormal < 0)
    {
   		float d = toLight.GetLength();
		toLight.Normalize();

		Ray lightRay = Ray(info.GetPosition(), toLight);

		if(renderer->TraceShadowRay(lightRay, d))
		{
			float cosphi = abs(normal*toLight);
			float costheta = abs(toLight*lightNormal);
            Color c;
			c = info.GetMaterial()->ComponentBRDF(info, toLight, component)
                *costheta*cosphi*intensity_*GetArea()/(d*d);
            return c;
		}
    }
    return Color(0, 0, 0);
}

Color MeshLight::NextEventEstimationMIS(const Renderer* renderer, const IntersectionInfo& info, unsigned int component) const
{
    Vector3d lightPoint, lightNormal;
    float weight = SamplePoint(lightPoint, lightNormal);
    Vector3d toLight = lightPoint - info.GetPosition();
    Vector3d normal = info.GetNormal();

    if(toLight*lightNormal < 0)
    {
   		float d = toLight.GetLength();
		toLight.Normalize();
		Ray lightRay = Ray(info.GetPosition(), toLight);

        if(renderer->TraceShadowRay(lightRay, d))
		{
			float cosphi = abs(normal*toLight);
			float costheta = abs(toLight*lightNormal);
            Material* mat = info.GetMaterial();
			Color c = mat->ComponentBRDF(info, toLight, component)
                      *costheta*cosphi*intensity_*GetArea()/(d*d);
            float brdfPdf = costheta*mat->PDF(info, toLight, 
                                              component, false)/(d*d);
            float lightPdf = 1.0f/GetArea();
            return c/(1.0f + brdfPdf*brdfPdf/(lightPdf*lightPdf));
		}
    }
    return Color(0, 0, 0);
}


Color MeshLight::DirectHitMIS(const Renderer* renderer, 
                                const IntersectionInfo& lastInfo, 
                                const IntersectionInfo& thisInfo, 
                                unsigned int component) const
{
    Vector3d v = thisInfo.position - lastInfo.position;
    float d = v.GetLength();
    v.Normalize();
    float costheta = abs(v*thisInfo.normal);
    float lightPdf = 1.0f/GetArea();
    Material* mat = lastInfo.GetMaterial();
    float brdfPdf = costheta*mat->PDF(lastInfo, v, component, false)/(d*d);
    return intensity_/(1.0f + lightPdf*lightPdf/(brdfPdf*brdfPdf));
}