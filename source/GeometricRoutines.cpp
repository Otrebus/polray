#include "geometricroutines.h"
#include "Ray.h"
#include "Utils.h"
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

//-----------------------------------------------------------------------------
// Clips a polygon to an axis aligned plane (apparently this is the
// Sutherman-Hodgeman algorithm)
//-----------------------------------------------------------------------------
void ClipPolygonToAAP(int axis, bool side, double pos, vector<Vector3d>& input)
{
    vector<Vector3d> output;
    output.reserve(6);
    const int& a = axis;

    for(vector<Vector3d>::iterator it = input.begin(); it < input.end(); it++)
    {
        Vector3d s = *it, e; // Start and end vectors for this line segment
        e = it + 1 == input.end() ? input.front() : it[1];

        if(s[a] == pos && e[a] == pos)
            output.push_back(e); // Planar segment, keep it on the plane

        else if(side && pos <= s[a] && pos <= e[a] || !side && pos >= s[a] && pos >= e[a])
            // Both start and end vectors are inside the clip region, add the end vector to output
            output.push_back(e);

        else if(!(side && pos >= s[a] && pos > e[a] || !side && pos <= s[a] && pos < e[a]))
        {   // The line segment is being clipped, determine which points to add to output
            double t = (pos - s[a])/(e[a]-s[a]);
            Vector3d splitpoint = t*(e - s) + s;

            if(side && s[a] < pos || !side && s[a] > pos)
                output.insert(output.end(), { splitpoint, e });

            else if(side && s[a] > pos || !side && s[a] < pos)
                output.push_back(splitpoint);
        }
    }
    input = output;
}

Vector3d Reflect(const Vector3d& incident, const Vector3d& normal)
{
    return incident - normal*(incident*normal)*2;
}

std::tuple<Vector3d, Vector3d> MakeBasis(const Vector3d& givenVector)
{
    auto v2 = givenVector^Vector3d(1, 0, 0);

    if(v2.GetLengthSquared() < 0.0001f)
        v2 = givenVector^Vector3d(0, 0, 1);

    return { v2.Normalized(), (givenVector^v2).Normalized() };
}

Vector3d SampleHemisphereCos(double r1, double r2, const Vector3d& apex)
{
    // See also: https://twitter.com/mmalex/status/1550765798263758848
    auto [right, forward] = MakeBasis(apex);

    return forward*cos(r1*2*pi)*sqrt(r2) + right*sin(r1*2*pi)*sqrt(r2) + apex*sqrt(1-r2) + apex*eps;
}

Vector3d SampleHemisphereUniform(double r1, double r2, const Vector3d& apex)
{
    auto [right, forward] = MakeBasis(apex);

    return forward*cos(r1*2*pi)*sqrt(1-r2*r2) + right*sin(r1*2*pi)*sqrt(1-r2*r2) + apex*r2;
}

Vector3d SampleSphereUniform(double r1, double r2)
{
    auto z = (r1 - 0.5)*2;
    double r = sqrt(1 - z*z);
    double u = r2*2*pi;
    return Vector3d(r*cos(u), r*sin(u), z);
}

double IntersectSphere(const Vector3d& position, double radius, const Ray& ray)
{
    Vector3d dir(ray.direction);
    Vector3d vec(ray.origin - position);

    double C = vec*vec - radius*radius;
    double B = 2*(vec*dir);
    double A = dir*dir;
    double D = (B*B/(4*A) - C)/A;

    double t = -B/(2*A) - sqrt(D);

    if(D > 0) {
        if(t < eps)
            return -B/(2*A) + sqrt(D) > 0 ? t = -B/(2*A) + sqrt(D) : -inf;
        return t;
    }
    return -inf;
}


double IntersectTriangle(const Vector3d& v0, const Vector3d& v1, const Vector3d& v2, const Ray& ray)
{
    double u, v, t;
	const Vector3d& D = ray.direction;

	Vector3d E1 = v1-v0, E2 = v2-v0;
	Vector3d T = ray.origin - v0;

	Vector3d P = E2^T, Q = E1^D;

	double det = E2*Q;
	if(!det)
		return -inf;
	u = ray.direction*P/det;

	if(u > 1 || u < 0)
		return -inf;

	v = T*Q/det;

	if(u+v > 1 || u < 0 || v < 0)
		return -inf;

	t = E1*P/det;

	return t <= 0 ? -inf : t;
}


std::vector<Vector2d> convexHull(std::vector<Vector2d> v)
{
    std::vector<Vector2d> ps, ps2; // Partial and full convex hulls and the output

    auto sortFn = [] (const Vector2d& a, const Vector2d& b) { return std::make_pair(a.x, a.y) < std::make_pair(b.x, b.y); };
    auto turnsRight = [] (Vector2d a, Vector2d b, Vector2d c) { return (b.x-a.x)*(c.y-a.y) - (c.x-a.x)*(b.y-a.y) < 0; };

    std::sort(v.begin(), v.end(), sortFn);

    for(auto it = v.begin(); it < v.end(); ps.push_back(*it), it++) // Lower hull part
        for(int i = (int) ps.size() - 2; i >= 0 && turnsRight(ps[i], ps[i+1], *it); i--)
            ps.pop_back();

    for(auto it = v.rbegin(); it < v.rend(); ps2.push_back(*it), it++) // Upper hull part
        for(int i = (int) ps2.size() - 2; i >= 0 && turnsRight(ps2[i], ps2[i+1], *it); i--)
            ps2.pop_back();

    // Merge the convex hull parts
    ps.insert(ps.end(), ps2.begin()+1, ps2.end()-1);

    return ps;
}
