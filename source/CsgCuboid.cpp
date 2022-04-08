#include "BoundingBox.h"
#include "CsgCuboid.h"
#include "Scene.h"
#include "Ray.h"

CsgCuboid::CsgCuboid(const Vector3d& position, const Vector3d& x, 
                     const Vector3d& y, float a, float b, float c) 
                     : pos_(position), x_(x), y_(y), a_(a), b_(b), c_(c)
{
    x_.Normalize();
    y_ = (x_^y_)^x_; // Ensure orthogonality
    y_.Normalize();
    z_ = x^y;
    z_.Normalize();
    Precalculate();
}

bool CsgCuboid::Intersect(const Ray& inRay, 
                          std::vector<CsgHit>& intersects) const
{
    float tnear, tfar;
    int axisNear, axisFar, sideNear, sideFar;
    Vector3d nearNormal(0, 0, 0), farNormal(0, 0, 0);

    if(!SlabsTest(inRay, tnear, tfar, axisNear, axisFar, sideNear, sideFar))
        return false;

    nearNormal[axisNear] = sideNear;
    farNormal[axisFar] = sideFar;

    IntersectionInfo nearHit, farHit;

    nearHit.normal = nearHit.geometricnormal 
                   = Multiply(x_, y_, z_, nearNormal);
    nearHit.direction = inRay.direction;
    nearHit.position = inRay.origin + inRay.direction*tnear 
                     + nearHit.normal*0.0001f;
    nearHit.material = material;

    farHit.normal = farHit.geometricnormal 
                   = Multiply(x_, y_, z_, farNormal);
    farHit.direction = inRay.direction;
    farHit.position = inRay.origin + inRay.direction*tfar 
                    + farHit.normal*0.0001f;
    farHit.material = material;

    CsgHit nearI, farI;
    nearI.info = nearHit;
    nearI.t = tnear;
    nearI.type = CsgHit::Enter;
    farI.info = farHit;
    farI.t = tfar;
    farI.type = CsgHit::Exit;

    intersects.push_back(nearI);
    intersects.push_back(farI);
    return true;
}

void CsgCuboid::Rotate(const Vector3d& axis, float angle)
{
    float u = axis.x;
    float v = axis.y;
    float w = axis.z;
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

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

void CsgCuboid::Translate(const Vector3d& dir)
{
    pos_ += dir;
}

void CsgCuboid::Precalculate()
{
    // This is the standard adjoint/determinant calculation of an inverse matrix
    float det = x_*(y_^z_);
    invMatU_ = Vector3d(y_.y*z_.z - z_.y*y_.z,
                        -(x_.y*z_.z - z_.y*x_.z),
                        x_.y*y_.z - y_.y*x_.z)/det;
    invMatV_ = Vector3d(-(y_.x*z_.z - z_.x*y_.z),
                        x_.x*z_.z - z_.x*-x_.z,
                        -(x_.x*y_.z - y_.x*x_.z))/det;
    invMatW_ = Vector3d(y_.x*z_.y - y_.y*x_.z,
                        -(x_.x*z_.y - x_.y*z_.x),
                        x_.x*y_.y - y_.x*x_.y)/det;
}

float CsgCuboid::Intersect(const Ray& inRay) const
{
    float tnear, tfar;
    int axisNear, axisFar, sideNear, sideFar;

    if(SlabsTest(inRay, tnear, tfar, axisNear, axisFar, sideNear, sideFar))
    {
        if(tnear > 0)
            return tnear;
        else if(tfar > 0)
            return tfar;
        else
            return -1.0f;
    }
    else
        return -1.0f;
}

bool CsgCuboid::GenerateIntersectionInfo(const Ray& inRay, 
                                         IntersectionInfo& info) const
{
    float t, tnear, tfar;
    int axisNear, axisFar, sideNear, sideFar, axis, side;
    
    SlabsTest(inRay, tnear, tfar, axisNear, axisFar, sideNear, sideFar);
    
    if(tnear > 0)
    {
        t = tnear;
        axis = axisNear;
        side = sideNear;
    }
    else if(tfar > 0)
    {
        t = tfar;
        axis = axisFar;
        side = sideFar;
    }
    else
        return false;

    Vector3d normal(0, 0, 0);
    normal[axis] = side;
    info.normal = info.geometricnormal 
                = Multiply(x_, y_, z_, normal);
    info.direction = inRay.direction;
    info.position = inRay.origin + inRay.direction*t + info.normal*0.0001f;
    info.material = material;
    return true;
}

BoundingBox CsgCuboid::GetBoundingBox() const
{
    float X = a_*abs(x_.x) + b_*abs(y_.x) + c_*abs(z_.x);
    float Y = a_*abs(x_.y) + b_*abs(y_.y) + c_*abs(z_.y);
    float Z = a_*abs(x_.z) + b_*abs(y_.z) + c_*abs(z_.z);
    return BoundingBox(pos_ - Vector3d(X, Y, Z), pos_ + Vector3d(X, Y, Z));
}

bool CsgCuboid::GetClippedBoundingBox(const BoundingBox& clipbox, 
                                      BoundingBox& resultbox) const
{
    resultbox = GetBoundingBox();
    return true;
}

void CsgCuboid::AddToScene(Scene& scene)
{
    Scene::PrimitiveAdder::AddPrimitive(scene, this);
}

void CsgCuboid::Save(Bytestream& stream) const
{
}

void CsgCuboid::Load(Bytestream& stream)
{
}

void CsgCuboid::SetMaterial(Material* mat)
{
    material = mat;
}

bool CsgCuboid::SlabsTest(const Ray& inRay, float& tNear, float& tFar, int& axisNear, int& axisFar, int& sideNear, int& sideFar) const
{
    float x, y, z;
    Vector3d transPos = Multiply(invMatU_, invMatV_, invMatW_, pos_);
    Vector3d transOrigin = Multiply(invMatU_, invMatV_, invMatW_, inRay.origin);
    Vector3d transDir = Multiply(invMatU_, invMatV_, invMatW_, inRay.direction);
    Ray ray(transOrigin - transPos, transDir);

    tFar = std::numeric_limits<float>::infinity();
    tNear = -std::numeric_limits<float>::infinity();

    Vector3d c1(-a_/2, -b_/2, -c_/2);
    Vector3d c2(a_/2, b_/2, c_/2);

    for(int u = 0; u < 3; u++)
    {
        if (ray.direction[u] == 0)
        {  
            if (ray.origin[u] > c2[u] || ray.origin[u] < c1[u]) 
                return false;
        }
        else
        {  
            float tmpSideNear = -1;
            float tmpSideFar = 1;
            float t1 = (c1[u] - ray.origin[u]) / ray.direction[u];
            float t2 = (c2[u] - ray.origin[u]) / ray.direction[u];
            if (t1 > t2)
            {
                std::swap(tmpSideNear, tmpSideFar);
                std::swap(t1, t2);
            }
            if (t1 > tNear) 
            {
                sideNear = tmpSideNear;
                axisNear = u;
                tNear = t1;
            }
            if (t2 < tFar) 
            {
                sideFar = tmpSideFar;
                axisFar = u;
                tFar = t2;
            }
            if (tNear > tFar) 
                return false;
        }  
    }
    return true;
}

std::unique_ptr<CsgObject> CsgCuboid::Clone()
{
    return std::unique_ptr<CsgObject>(new CsgCuboid(*this));
}