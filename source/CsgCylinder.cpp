#include "BoundingBox.h"
#include "CsgCylinder.h"
#include "Scene.h"
#include "Utils.h"

CsgCylinder::CsgCylinder(Vector3d& position, Vector3d& dir, 
                         double length, double radius) 
                         : pos_(position), length_(length), radius_(radius)
{
    dir.Normalize();
    z_ = dir;
    y_ = Vector3d(1, 0, 0);
    y_ = (z_^y_)^z_; // Ensure orthogonality
    if(y_.GetLength() < eps)
    {
        y_ = Vector3d(0, 1, 0);
        y_ = (z_^y_)^z_;
    }
    y_.Normalize();
    x_ = z_^y_;
    x_.Normalize();
    Precalculate();
}

bool CsgCylinder::Intersect(const Ray& inRay, std::vector<CsgHit>& intersects) const
{
    Vector3d transPos = Multiply(invMatU_, invMatV_, invMatW_, pos_);    
    Vector3d transOrigin = Multiply(invMatU_, invMatV_, invMatW_, inRay.origin);
    Vector3d transDir = Multiply(invMatU_, invMatV_, invMatW_, inRay.direction);
    Ray ray(transOrigin - transPos, transDir);
    const double& x = ray.direction.x;
    const double& y = ray.direction.y;
    const double& z = ray.direction.z;
    const double& x_O = ray.origin.x;
    const double& y_O = ray.origin.y;
    const double& z_O = ray.origin.z;

    const double a = x*x + y*y;
    const double b = 2*x*x_O + 2*y*y_O;
    const double c = x_O*x_O + y_O*y_O - radius_*radius_;
    const double D = b*b - 4*a*c;

    if(D < 0)
        return false;

    double tNear = (-b - sqrt(D))/(2*a);
    double tFar = (-b + sqrt(D))/(2*a);

    double xNear = ray.origin.x + tNear*ray.direction.x;
    double yNear = ray.origin.y + tNear*ray.direction.y;
    double zNear = ray.origin.z + tNear*ray.direction.z;
    double xFar = ray.origin.x + tFar*ray.direction.x;
    double yFar = ray.origin.y + tFar*ray.direction.y;
    double zFar = ray.origin.z + tFar*ray.direction.z;
    IntersectionInfo nearInfo, farInfo;
    nearInfo.direction = farInfo.direction = ray.direction;
    nearInfo.material = farInfo.material = material;
    nearInfo.normal = Vector3d(xNear, yNear, 0);
    nearInfo.normal.Normalize();
    farInfo.normal = Vector3d(xFar, yFar, 0);
    farInfo.normal.Normalize();

    if(zFar < -length_/2)
    {
        if(zNear < -length_/2)
            return false;
        else if(zNear < length_/2)
        {
            farInfo.normal = Vector3d(0, 0, -1);
            tFar = (-length_/2 - z_O)/z;
        }
        else
        {
            nearInfo.normal = Vector3d(0, 0, 1);
            farInfo.normal = Vector3d(0, 0, -1);
            tNear = (length_/2 - z_O)/z;
            tFar = (-length_/2 - z_O)/z;
        }
    }
    else if(zFar > length_/2)
    {
        if(zNear > length_/2)
            return false;
        else if(zNear > -length_/2)
        {
            farInfo.normal = Vector3d(0, 0, 1);
            tFar = (length_/2 - z_O)/z;
        }
        else
        {
            nearInfo.normal = Vector3d(0, 0, -1);
            farInfo.normal = Vector3d(0, 0, 1);
            tNear = (-length_/2 - z_O)/z;
            tFar = (length_/2 - z_O)/z;
        }
    }
    else
    {
        if(zNear > length_/2)
        {
            nearInfo.normal = Vector3d(0, 0, 1);
            tNear = (length_/2 - z_O)/z;
        }
        else if(zNear < -length_/2)
        {
            nearInfo.normal = Vector3d(0, 0, -1);
            tNear = (-length_/2 - z_O)/z;
        }
    }

    nearInfo.normal = nearInfo.geometricnormal 
                    = Multiply(x_, y_, z_, nearInfo.normal);
    nearInfo.direction = inRay.direction;
    nearInfo.position = inRay.origin + inRay.direction*tNear;
    nearInfo.material = material;

    farInfo.normal = farInfo.geometricnormal 
                   = Multiply(x_, y_, z_, farInfo.normal);
    farInfo.direction = inRay.direction;
    farInfo.position = inRay.origin + inRay.direction*tFar;
    farInfo.material = material;

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

BoundingBox CsgCylinder::GetBoundingBox() const
{
    Vector3d v = z_^(z_^Vector3d(0, 0, 1));
    if(v.GetLengthSquared() == 0)
        return BoundingBox(Vector3d(pos_.x - radius_, pos_.y - 
                                    radius_, pos_.z - length_/2),
                           Vector3d(pos_.x + radius_, pos_.y +
                                    radius_, pos_.z + length_/2));
    v.Normalize();
    v *= radius_;
    Vector3d w = v^z_;
    w.Normalize();
    w *= radius_;
    Vector3d c1(-abs(z_.x) - abs(v.x) - abs(w.x), 
                -abs(z_.y) - abs(v.y) - abs(w.y), 
                -abs(z_.z) - abs(v.z) - abs(w.z));
    Vector3d c2(abs(z_.x) + abs(v.x) + abs(w.x), 
                abs(z_.y) + abs(v.y) + abs(w.y), 
                abs(z_.z) + abs(v.z) + abs(w.z));
    return BoundingBox(pos_ + c1, pos_ + c2);
}

bool CsgCylinder::GetClippedBoundingBox(const BoundingBox& clipbox, BoundingBox& resultbox) const
{
    resultbox = GetBoundingBox();
    return true;
}

double CsgCylinder::Intersect(const Ray& inRay) const
{
    Vector3d transPos = Multiply(invMatU_, invMatV_, invMatW_, pos_);    
    Vector3d transOrigin = Multiply(invMatU_, invMatV_, invMatW_, inRay.origin);
    Vector3d transDir = Multiply(invMatU_, invMatV_, invMatW_, inRay.direction);
    Ray ray(transOrigin - transPos, transDir);

    const double& x = ray.direction.x;
    const double& y = ray.direction.y;
    const double& z = ray.direction.z;
    const double& x_O = ray.origin.x;
    const double& y_O = ray.origin.y;
    const double& z_O = ray.origin.z;

    const double a = x*x + y*y;
    const double b = 2*x*x_O + 2*y*y_O;
    const double c = x_O*x_O + y_O*y_O - radius_*radius_;
    const double D = b*b - 4*a*c;

    if(D < 0)
        return -inf;

    double tNear = (-b - sqrt(D))/(2*a);
    double tFar = (-b + sqrt(D))/(2*a);
    double zNear = ray.origin.z + tNear*ray.direction.z;
    double zFar = ray.origin.z + tFar*ray.direction.z;

    if(zFar < -length_/2)
    {
        if(zNear < -length_/2)
            return -inf;
        else if(zNear < length_/2)
            tFar = (-length_/2 - z_O)/z;
        else
        {
            tNear = (length_/2 - z_O)/z;
            tFar = (-length_/2 - z_O)/z;
        }
    }
    else if(zFar > length_/2)
    {
        if(zNear > length_/2)
            return -inf;
        else if(zNear > -length_/2)
            tFar = (length_/2 - z_O)/z;
        else
        {
            tNear = (-length_/2 - z_O)/z;
            tFar = (length_/2 - z_O)/z;
        }
    }
    else
    {
        if(zNear > length_/2)
            tNear = (length_/2 - z_O)/z;
        else if(zNear < -length_/2)
            tNear = (-length_/2 - z_O)/z;
    }
    if(tNear > 0)
        return tNear;
    else if(tFar > 0)
        return tFar;
    return -inf;
}

bool CsgCylinder::GenerateIntersectionInfo(const Ray& inRay, IntersectionInfo& info) const
{
    std::vector<CsgHit> infos;
    if(Intersect(inRay, infos))
    {
        CsgHit isec = infos.front();
        if(isec.t > 0)
        {
            info = isec.info;
            return true;
        }

        isec = infos.back();
        if(isec.t > 0)
        {
            info = isec.info;
            return true;
        }
    }
    return false;
}


void CsgCylinder::AddToScene(Scene& scene)
{
    Scene::PrimitiveAdder::AddPrimitive(scene, this);
}

void CsgCylinder::Save(Bytestream& stream) const
{
}

void CsgCylinder::Load(Bytestream& stream)
{
}

void CsgCylinder::SetMaterial(Material* mat)
{
    material = mat;
}

void CsgCylinder::Rotate(const Vector3d& axis, double angle)
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
    x_ *= rot;
    y_ *= rot;
    z_ *= rot;
    Precalculate();
}

void CsgCylinder::Translate(const Vector3d& dir)
{
    pos_ += dir;
}

void CsgCylinder::Precalculate()
{
    // This is the standard adjoint/determinant calculation of an inverse matrix
    double det = x_*(y_^z_);
    invMatU_ = Vector3d(y_.y*z_.z - z_.y*y_.z,
                        -(x_.y*z_.z - z_.y*x_.z),
                        x_.y*y_.z - y_.y*x_.z)/det;
    invMatV_ = Vector3d(-(y_.x*z_.z - z_.x*y_.z),
                        x_.x*z_.z - z_.x*x_.z,
                        -(x_.x*y_.z - y_.x*x_.z))/det;
    invMatW_ = Vector3d(y_.x*z_.y - y_.y*z_.x,
                        -(x_.x*z_.y - x_.y*z_.x),
                        x_.x*y_.y - y_.x*x_.y)/det;
}

std::unique_ptr<CsgObject> CsgCylinder::Clone()
{
    return std::unique_ptr<CsgObject>(new CsgCylinder(*this));
}