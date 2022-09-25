#include "RayTracer.h"
#include "TriangleMesh.h"
#include "Logger.h"
#include "Utils.h"
#include "Main.h"

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
    Color c = TraceRayRecursive(ray, 50, 0, 1.0);

    return c;
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
Color RayTracer::TraceRayRecursive(Ray ray, int bounces, Primitive*, double contribution) const
{
    if(contribution < 0.005)
        return Color(0, 0, 0);
        
    Vector3d dir = ray.direction;
    dir.Normalize();
    
    bool objecthit = false;

    if(bounces < 1)
        return Color(1, 0, 0);

    const Primitive* minprimitive = nullptr;
    const Light* minlight = nullptr;

    double t = scene->Intersect(ray, minprimitive, minlight);

    if(t > 0.001)
        objecthit = true;

    if(objecthit)
    {
        IntersectionInfo info;
        if(minprimitive)
            minprimitive->GenerateIntersectionInfo(ray, info);
        else
            minlight->GenerateIntersectionInfo(ray, info);

        Vector3d intersection = info.position;
        Vector3d normal = info.normal;
        normal.Normalize();

        if(info.material)
        {
            Vector3d light(1, 1, 1);
            light.Normalize();
            double dot = light*(info.normal);
            if(dot > 0)
                return 10*Color(0, 0, dot);
            else
                return Color(0.0, 0.0, 0.3);
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
    const Primitive* dummy = nullptr;
    const Light* dummy2 = nullptr;
    double result = scene->Intersect(ray, dummy, dummy2);
    if(result < tmax*(1-eps))
        return false;
    return true;
}


void RayTracer::Save(Bytestream& stream) const
{
    stream << ID_RAYTRACER;
}

void RayTracer::Load(Bytestream&)
{
}
