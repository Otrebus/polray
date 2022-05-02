#include "RayTracer.h"
#include "TriangleMesh.h"
#include "Logger.h"
#include "Main.h"
#include <intrin.h>
#include "Utils.h"

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
RayTracer::RayTracer(std::shared_ptr<Scene> scn) : Renderer(scn)
{
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
RayTracer::~RayTracer()
{
}

//------------------------------------------------------------------------------
// Returns the radiance emitted towards us from the direction of a submitted ray
//------------------------------------------------------------------------------
Color RayTracer::TraceRay(const Ray& ray) const
{
    Color c = TraceRayRecursive(ray, 50, 0, 1.0f);

    return c;
}

void RayTracer::SetSPP(unsigned int spp)
{
}

void RayTracer::Render(Camera& cam, ColorBuffer& colBuf)
{
    int xres = colBuf.GetXRes();
    int yres = colBuf.GetYRes();
    
    for(int y = 0; y < yres; y++)
    {
        for(int x = 0; x < xres; x++)
        {
            if(g_quitting)
                colBuf.SetPixel(x, y, Color(0, 0, 0));

            Color c = TraceRay(cam.GetRayFromPixel(x, y, 0, 0, 0, 0));
            if(!c.IsValid())
                c = Color(0, 0, 0);
            colBuf.SetPixel(x, y, c);
        }
    }
}

//------------------------------------------------------------------------------
// Helper function for TraceRay
//------------------------------------------------------------------------------
Color RayTracer::TraceRayRecursive(Ray ray, int bounces, Primitive* ignore, double contribution) const
{
    if(contribution < 0.005f)
        return Color(0, 0, 0);
        
    Vector3d dir = ray.GetDirection();
    dir.Normalize();
    
    bool objecthit = false;

    if(bounces < 1)
        return Color(1, 0, 0);

    const Primitive* minprimitive = nullptr;
    const Light* minlight = nullptr;

    double t = scene->Intersect(ray, minprimitive, minlight);

    if(t > 0.001f)
        objecthit = true;

    if(objecthit)
    {
        IntersectionInfo info;
        if(minprimitive)
            minprimitive->GenerateIntersectionInfo(ray, info);
        else
            minlight->GenerateIntersectionInfo(ray, info);

        Vector3d intersection = info.GetPosition();
        Vector3d normal = info.GetNormal();
        normal.Normalize();

        if(info.GetMaterial())
        {
            Vector3d light(6, 13, 4);
            light.Normalize();
            double dot = light*(info.GetNormal());
            if(dot > 0)
                return 10*Color(dot, dot, dot);
            else
                return 10*Color(-dot, -dot, -dot);
        }
        return Color(0, 0, 0);
    }
    else
        return Color(0, 0, 0);
}

//------------------------------------------------------------------------------
// Traces ray with t parameter between 0..tmax and checks if occluded
//------------------------------------------------------------------------------
bool RayTracer::TraceShadowRay(const Ray& ray, double tmax) const
{
    Ray& unconstRay = const_cast<Ray&>(ray);
    const Primitive* dummy = nullptr;
    const Light* dummy2 = nullptr;
    double result = scene->Intersect(ray, dummy, dummy2);
    if(result < tmax*(1-eps))
        return false;
    return true;
}

unsigned int RayTracer::GetType() const
{
    return 1;
}

unsigned int RayTracer::GetSPP() const
{
    return 1;
}