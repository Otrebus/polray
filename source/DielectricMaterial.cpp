#include "Bytestream.h"
#include "DielectricMaterial.h"
#include <sstream>

using namespace std;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
DielectricMaterial::DielectricMaterial()
{
    m_ior = 1.5f;
    m_rnd.Seed(GetTickCount());
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
DielectricMaterial::~DielectricMaterial()
{
}

//------------------------------------------------------------------------------
// A sample is generated in either the reflected or refracted direction - both 
// cases are treated as the same component of the brdf since they are both 
// specular and reflect the same amount of light for any given bounce.
//------------------------------------------------------------------------------
Sample DielectricMaterial::GetSample(const IntersectionInfo& info, bool adjoint) const
{
    auto pdf = 1;  // This is not true, of course, which is indicated by the
    auto rpdf = 1; // specularity of this brdf

    Vector3d _normal = info.GetNormal();
    Vector3d normal;

    const Vector3d& wi = info.GetDirection();
    const Vector3d& Ng = info.GetGeometricNormal();
    Vector3d& Ns = normal;

    float cosi = _normal*wi*-1;
    float n1, n2;

    if(cosi > 0) // Ray from outside the material, going in
    {
        n1 = 1.0f;
        n2 = m_ior;
        normal = _normal;
    }
    else        // Ray from the inside, going out - stuff is flipped
    {           // to make the rest of the calculations still valid
        n1 = m_ior;
        n2 = 1.0f;
        normal = -_normal;
        cosi = -cosi;
    }
    
    float d = 1-(n1/n2)*(n1/n2)*(1-cosi*cosi);

    if(d < 0) // Total internal reflection
    {
        Ray out;
        out.direction = Reflect(info.GetDirection(), Ns);
        out.direction.Normalize();
        auto wo = out.direction;
        out.origin = info.GetPosition() + 0.0001f*(wo*Ng > 0 ? Ng : -Ng);
        auto color = adjoint ? abs((1/(wi*Ng))*(wo*Ng/(1))) * Color::Identity : Color::Identity;
        return Sample(color, out, pdf, rpdf, true);
    }
    Vector3d refraction = wi*(n1/n2) + Ns*(cosi*(n1/n2) - sqrt(d));
    refraction.Normalize();
    float cost = -refraction*normal;
    float Rs = (n1 * cosi - n2 * cost)/(n1 * cosi + n2*cost);
    float Rp = (n1 * cost - n2 * cosi)/(n1 * cost + n2*cosi);
    float R = (Rs*Rs+Rp*Rp)/2.0f;

    if(m_rnd.GetFloat(0, 1) > R) // Refracted
    {
        Ray out;
        out.direction = refraction;
        out.direction.Normalize();
        auto wo = out.direction;
        out.origin = info.GetPosition() + 0.0002f*(wo*Ng > 0 ? Ng : -Ng);
        auto color = adjoint ? abs((wi*Ns/(wi*Ng))*(wo*Ng/(wo*Ns))) * Color::Identity : (n1/n2)*(n1/n2)*Color::Identity;
        return Sample(color, out, pdf, rpdf, true);
    }
    else // Reflected
    {
        Ray out;
        out.direction = Reflect(info.GetDirection(), Ns);
        out.direction.Normalize();
        auto wo = out.direction;
        out.origin = info.GetPosition() + 0.0001f*(wo*Ng > 0 ? Ng : -Ng);
        auto color = adjoint ? abs((1/(wi*Ng))*(wo*Ng/(1))) * Color::Identity : Color::Identity;
        return Sample(color, out, pdf, rpdf, true);
    }
}

//------------------------------------------------------------------------------
// Returns the value of the BRDF in the direction given in direction out
// relative to the given intersectioninfo. In the case of any specular material
// the BRDF is 0 in all directions, since it is effectively a distribution.
//------------------------------------------------------------------------------
Color DielectricMaterial::BRDF(const IntersectionInfo& info, 
    const Vector3d& out) const
{
    return Color(0, 0, 0); // The chance that the out, in vectors 
}                          // are reflectant is 0

//------------------------------------------------------------------------------
// Returns the assigned light of this material, which is always 0 (none).
//------------------------------------------------------------------------------
Light* DielectricMaterial::GetLight() const
{
    return light;
}

//------------------------------------------------------------------------------
// Reads and sets the properties of this material from a stringstream.
//------------------------------------------------------------------------------
void DielectricMaterial::ReadProperties(stringstream& ss)
{
    while(!ss.eof())
    {
        string line;
        getline(ss, line);
        stringstream ss2(line);
        ss2 >> m_ior;
    }
}

//------------------------------------------------------------------------------
// Returns the value of the probability distribution function for a sample
// generated in direction out given the intersectioninfo for the orientation
// of the material boundary, etc. The value in this case is always 0, but should
// never be used in practice since it would only cause NaNs in any integrator.
// Hence the actual value is returned as 1, just to avoid any potential issues.
//------------------------------------------------------------------------------
float DielectricMaterial::PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint) const
{
    return 1;
}

void DielectricMaterial::Save(Bytestream& stream) const
{
    stream << (unsigned char) ID_DIELECTRICMATERIAL;
    stream << m_ior;
}

void DielectricMaterial::Load(Bytestream& stream)
{
    stream >> m_ior;
}