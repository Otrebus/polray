#include "geometricroutines.h"
#include <vector>

using namespace std;

//-----------------------------------------------------------------------------
// Clips a polygon to an axis aligned plane (apparently this is the
// Sutherman-Hodgeman algorithm)
//-----------------------------------------------------------------------------
void ClipPolygonToAAP(int axis, int side, double position, vector<Vector3d>& input)
{
    vector<Vector3d> output;
    output.reserve(6);

    if(side == 1) // Clip away the negative side of the plane
    {
        for(vector<Vector3d>::iterator it = input.begin(); it < input.end(); it++)
        {
            Vector3d s, e; // Start and end vectors for this line segment
            if(it + 1 == input.end())
            {
                s = *it;
                e = input.front();
            }
            else
            {
                s = *it;
                e = it[1];
            }
            if(s[axis] == position && e[axis] == position)
            {   // Planar segment, keep it on the plane
                output.push_back(e);
                continue;
            }

            else if(position <= s[axis] && position <= e[axis])
            {   // Both start and end vectors are inside the clip region, add the end
                // vector to the output (the for loop has already taken care of the first part
                // or will revisit it in the first if statement above as the last point)
                output.push_back(e);
                continue;
            }
            else if(position >= s[axis] && position > e[axis])
                continue; // Both are to be clipped away

            // The line segment is being clipped, determine which points to add to output
            double t = (position - s[axis])/(e[axis]-s[axis]);
            Vector3d splitpoint = t*(e - s) + s;
            if(s[axis] < position)
            {
                output.push_back(splitpoint);
                output.push_back(e);
            }
            else if(s[axis] > position)
                output.push_back(splitpoint);
        }
    }
    if(side == -1)
    {
        for(vector<Vector3d>::iterator it = input.begin(); it < input.end(); it++)
        {
            Vector3d s, e;
            if(it + 1 == input.end())
            {
                s = *it;
                e = input.front();
            }
            else
            {
                s = *it;
                e = it[1];
            }
            if(s[axis] == position && e[axis] == position)
            {   
                output.push_back(e);
                continue;
            }
            else if(position >= s[axis] && position >= e[axis])
            {
                output.push_back(e);
                continue;
            }
            else if(position <= s[axis] && position < e[axis])
                continue;

            double t = (position - s[axis])/(e[axis]-s[axis]);
            Vector3d splitpoint = t*(e - s) + s;
            if(s[axis] > position)
            {
                output.push_back(splitpoint);
                output.push_back(e);
            }
            else if(s[axis] < position)
            {
                output.push_back(splitpoint);
            }
        }
    }
    input = output;
}

Vector3d Reflect(const Vector3d& incident, const Vector3d& normal)
{
    return incident - normal*(incident*normal)*2;
}

double Refract(double n1, double n2, Vector3d& _normal, Vector3d& incident, Vector3d& refraction)
{
    double R;
    Vector3d& normal = _normal;
    double cosi = normal*incident*-1;
    double d = 1-(n1/n2)*(n1/n2)*(1-cosi*cosi);
    if(d < 0)
        return 1.0f;
    refraction = incident * (n1/n2) + normal * ( cosi*(n1/n2) - sqrt(d) );
    double cost = -(refraction*(normal));

    refraction.Normalize();

    double Rs = (n1 * cosi - n2 * cost)/(n1 * cosi + n2*cost);
    Rs*=Rs;
    double Rp = (n1 * cost - n2 * cosi)/(n1 * cost + n2*cosi);
    Rp*=Rp;
    R = (Rs+Rp)/2.0f;
    return R;
}

void MakeBasis(const Vector3d& givenVector, Vector3d& v2, Vector3d& v3)
{
    v2 = givenVector^Vector3d(1, 0, 0);

    if(v2.GetLengthSquared() < 0.0001f)
        v2 = givenVector^Vector3d(0, 0, 1);

    v2.Normalize();
    v3 = givenVector^v2;
    v3.Normalize();
}

void SampleHemisphereCos(double r1, double r2, const Vector3d& apex, Vector3d& sample)
{
    Vector3d right, forward;
    MakeBasis(apex, right, forward);

    sample = forward*cos(r1)*sqrt(r2) + right*sin(r1)*sqrt(r2) 
             + apex*sqrt(1-r2);
}

void SampleHemisphereUniform(double r1, double r2, const Vector3d& apex, Vector3d& sample)
{
    Vector3d right, forward;
    MakeBasis(apex, right, forward);

    sample = forward*cos(r1)*sqrt(1-r2*r2) + right*sin(r1)*sqrt(1-r2*r2) 
             + apex*r2;
}