#include "BoundingBox.h"
#include "CsgSphere.h"
#include "Scene.h"
#include "Utils.h"

CsgSphere::CsgSphere(const Vector3d& position, double radius) 
    : pos_(position), radius_(radius)
{
}

bool CsgSphere::Intersect(const Ray& ray, std::vector<CsgHit>& intersects) const
{
    double tNear, tFar;
    Vector3d dir(ray.direction);
    Vector3d vec = ray.origin - pos_;

    double A = dir*dir;
    double B = 2*(vec*dir);
    double C = vec*vec - radius_*radius_;
    double D = (B*B/(4*A) - C)/A;

    
    if(D < 0)
        return false;

    tNear = -B/(2*A) - sqrt(D);
    tFar = -B/(2*A) + sqrt(D);

    IntersectionInfo nearInfo, farInfo;
    nearInfo.direction = farInfo.direction = ray.direction;
    nearInfo.material = farInfo.material = material;
    nearInfo.position = ray.origin + ray.direction*tNear;
    nearInfo.normal = nearInfo.position - pos_;
    nearInfo.normal.Normalize();
    nearInfo.position += nearInfo.normal*eps;
    nearInfo.geometricnormal = nearInfo.normal;
    farInfo.position = ray.origin + ray.direction*tFar;
    farInfo.normal = farInfo.position - pos_;
    farInfo.normal.Normalize();
    farInfo.geometricnormal = farInfo.normal;
    farInfo.position += farInfo.normal*eps;

    CsgHit nearI, farI;
    nearI.info = nearInfo;
    nearI.t = tNear;
    nearI.type = CsgHit::Enter;

    farI.info = farInfo;
    farI.t = tFar;
    farI.type = CsgHit::Exit;

    intersects.push_back(nearI);
    intersects.push_back(farI);
    return true;
}

BoundingBox CsgSphere::GetBoundingBox() const
{
    return BoundingBox(pos_ - Vector3d(radius_, radius_, radius_),
                       pos_ + Vector3d(radius_, radius_, radius_));
}

std::tuple<bool, BoundingBox> CsgSphere::GetClippedBoundingBox(const BoundingBox&) const
{
    return { true, GetBoundingBox() };
}

double CsgSphere::Intersect(const Ray& ray) const
{
    double t;
    Vector3d dir(ray.direction);
    Vector3d vec = ray.origin - pos_;
    double A = dir*dir;
    double B = 2*(vec*dir);
    double C = vec*vec - radius_*radius_;
    double D = (B*B/(4*A) - C)/A;
    if(D < 0)
        return -inf;
    t = -B/(2*A) - sqrt(D);
    if(t < 0)
        t = -B/(2*A) + sqrt(D);
    if(t < 0)
        return false;
    return true;
}

bool CsgSphere::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const
{
    std::vector<CsgHit> infos;
    if(Intersect(ray, infos))
    {
        CsgHit isec = infos.front();
        if(isec.t >= 0)
        {
            info = isec.info;
            return true;
        }

        isec = infos.back();
        if(isec.t >= 0)
        {
            info = isec.info;
            return true;
        }
    }
    return false;
}

void CsgSphere::Translate(const Vector3d& direction)
{
    pos_ += direction;
}

void CsgSphere::Rotate(const Vector3d& axis, double angle)
{
    double u = axis.x;
    double v = axis.y;
    double w = axis.z;
    double cosAngle = cos(angle);
    double sinAngle = sin(angle);

    Matrix3d rot(u*u + (1 - u*u)*cosAngle, u*v*(1 - cosAngle) - w*sinAngle,
                 u*w*(1 - cosAngle) + v*sinAngle, 0, 
                 u*v*(1 - cosAngle) + w*sinAngle, v*v + (1 - v*v)*cosAngle,
                 v*w*(1 - cosAngle) - u*sinAngle, 0,
                 u*w*(1 - cosAngle) - v*sinAngle, v*w*(1-cosAngle) + u*sinAngle,
                 w*w + (1 - w*w)*cosAngle, 0, 0, 0, 0, 1);

    pos_ *= rot;
}

void CsgSphere::AddToScene(Scene& scene)
{
    Scene::PrimitiveAdder::AddPrimitive(scene, this);
}

void CsgSphere::Save(Bytestream&) const
{
}

void CsgSphere::Load(Bytestream&)
{
}

std::unique_ptr<CsgObject> CsgSphere::Clone()
{
    return std::unique_ptr<CsgObject>(new CsgSphere(*this));
}