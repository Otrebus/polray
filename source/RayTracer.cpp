#include "RayTracer.h"
#include "TriangleMesh.h"
#include "Logger.h"
#include "Utils.h"
#include "Main.h"

/**
 * Constructor.
 * 
 * @param scn The scene that we are rendering.
 */
RayTracer::RayTracer(std::shared_ptr<Scene> scn) : Renderer(scn)
{
}

/**
 * Destructor.
 */
RayTracer::~RayTracer()
{
}

/**
 * Traces a ray through the scene from the camera and returns some helpful information (currently
 * used for debugging).
 * 
 * @param ray The ray to trace.
 */
Color RayTracer::TraceRay(const Ray& ray) const
{
    Color c = TraceRayRecursive(ray, 50, 0, 1.0);

    return c;
}

/**
 * Renders the scene.
 * 
 * @param cam The camera to render the scene from.
 * @param colBuf The color buffer to render to.
 */
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

/**
 * Helper function for TraceRay. The purpose of this function right now is just for debug information
 * and its functionality changes.
 * 
 * @param ray The ray to trace.
 * @param bounces The number of bounces of the ray to trace, or something.
 * @param contribution This used to be the intensity of the color traced so far or something, for
 *                     early termination.
 * @returns Whatever we're currently using as debug info.
 */
Color RayTracer::TraceRayRecursive(Ray ray, int bounces, Primitive*, double contribution) const
{
    if(contribution < eps)
        return Color(0, 0, 0);
        
    Vector3d dir = ray.direction;
    dir.Normalize();
    
    bool objecthit = false;

    if(bounces < 1)
        return Color(1, 0, 0);

    auto [t, minprimitive, minlight] = scene->Intersect(ray);

    if(t > eps)
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

/**
 * Traces a shadow ray and checks of it's occluded.
 * 
 * @param ray The ray to trace.
 * @param tmax The maximum distance along the ray to trace.
 * @returns True if the shadow ray hits something before tmax.
 */
bool RayTracer::TraceShadowRay(const Ray& ray, double tmax) const
{
    const Primitive* dummy = nullptr;
    const Light* dummy2 = nullptr;
    auto [t, minprimitive, minlight] = scene->Intersect(ray);
    if(t < tmax*(1-eps))
        return false;
    return true;
}

/**
 * Saves information about the renderer to a bytestream.
 * 
 * @param stream The bytestream to serialize to.
 */
void RayTracer::Save(Bytestream& stream) const
{
    stream << ID_RAYTRACER;
}

/**
 * Loads information about the renderer process from a file.
 * 
 * @param stream The bytestream to deserialize from.
 */
void RayTracer::Load(Bytestream& bytestream)
{
}
