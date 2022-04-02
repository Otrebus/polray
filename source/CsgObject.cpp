#include "CsgObject.h"
#include "Vector3d.h"

Vector3d CsgObject::Multiply(const Vector3d& u, const Vector3d& v, 
                             const Vector3d& w, const Vector3d& x)
{
    return x.x*u + x.y*v + x.z*w;
}

/*void CsgObject::SetMaterial(Material* mat)
{
    material = mat;
}

Material* CsgObject::GetMaterial() const
{
    return material;
}*/