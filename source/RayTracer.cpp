#include "RayTracer.h"
#include "TriangleMesh.h"
#include "Logger.h"
#include "Main.h"
#include <intrin.h>

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
/*
//------------------------------------------------------------------------------
// Sets up the KD tree of the renderer
//------------------------------------------------------------------------------
void RayTracer::Setup(const vector<const Primitive*>& primitives, const vector<Light*>& lights)
{
    m_tree.Build(primitives);
    m_lights = lights;
}*/

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
    
    frames++;
    
    //#pragma omp parallel for schedule(dynamic)
    for(int y = 0; y < yres; y++)
    {
        for(int x = 0; x < xres; x++)
        {
            if(g_quitting)
                colBuf.SetPixel(x, y, Color(0, 0, 0));
#ifdef _DEBUG
            if(x == 77 && y == 237)
                __debugbreak();

#endif
//			else
            //{
                Color c = TraceRay(cam.GetRayFromPixel(x, y, 0, 0, 0, 0));
                if(!c.IsValid())
                    c = Color(0, 0, 0);
                colBuf.SetPixel(x, y, c);
//				if(0.2*c.g > c.r && 0.2*c.b > c.r)
//					_asm int 3
            //}
        }
    }
    //#pragma omp barrier
}

//------------------------------------------------------------------------------
// Helper function for TraceRay
//------------------------------------------------------------------------------
Color RayTracer::TraceRayRecursive(Ray ray, int bounces, Primitive* ignore, float contribution) const
{
    if(contribution < 0.005f)
        return Color(0, 0, 0);
        
    Vector3d dir = ray.GetDirection();
    dir.Normalize();
    
    bool objecthit = false;

    if(bounces < 1)
        return Color(1, 0, 0);

    const Primitive* minprimitive = 0;

    int numint = numintersects;
    float t = scene->Intersect(ray, minprimitive);

    numint = numintersects - numint;

    if(t > 0.001f)
        objecthit = true;

    if(objecthit)
    {
        //return Color(1000, 1000, 1000);
        //return Color(0, numint/255.0f, 0);
        IntersectionInfo info;
        minprimitive->GenerateIntersectionInfo(ray, info);

//		if(((MeshTriangle*)minshape)->id == 6036)
//			_asm int 3

        //return Color(100*info.GetNormal().x, 100*info.GetNormal().y, 100*info.GetNormal().z);

        //return Color(abs(info.GetGeometricNormal()*info.GetDirection())/abs(info.GetNormal()*info.GetDirection()), 1, 1);

        Vector3d intersection = info.GetPosition();
        Vector3d normal = info.GetNormal();
        normal.Normalize();
        //return Color(1000, 1000, 1000);

        //if(minshape->GetType() == Shape::type_sphere)
            //_asm int 3;

        if(minprimitive->GetMaterial())
        {
            // First, calculate the irradiance due to direct lighting
            // If there are no lights in the scene, simulate a light source and do lambertian

//			if(minshape->material->emissivity.GetIntensity() > 0.001f)
//				return minshape->material->emissivity;

        //	if(m_lights.empty())
        //	{
                Vector3d light(6, 13, 4);
                light.Normalize();
                float dot = light*(info.GetNormal());
               // return ray.direction*info.GetNormal() < 0 ? Color(255, 0 ,0) : Color(0, 255, 0);

                /*if(((MeshTriangle*)minshape)->id == 6036)
                {

                    stringstream sstr;
                    sstr << dot;
                                    logger.Box(sstr.str().c_str());
                }*/

//				return Color(100*info.GetNormal().x, 100*info.GetNormal().y, 100*info.GetNormal().z);
            //return Color(0, numint, 0);
                if(dot > 0)
                    return 10*Color(dot, dot, dot);
                else
                    return 10*Color(-dot, -dot, -dot);
        //	}

            //Color lightsum(0, 0, 0);

            //for(vector<Light*>::const_iterator it = m_lights.begin(); it < m_lights.end(); it++)
            //{
                //lightsum += (*it)->CalculateSurfaceRadiance(ray.origin, info);
            //}
            //return lightsum;
        }
        return Color(0, 0, 0);
    }
    else
        return Color(0, 0, 0);
}

//------------------------------------------------------------------------------
// Traces ray with t parameter between 0..tmax and checks if occluded
//------------------------------------------------------------------------------
bool RayTracer::TraceShadowRay(const Ray& ray, float tmax) const
{
    Ray& unconstRay = const_cast<Ray&>(ray);
    const Primitive* dummy = nullptr;
    float result = scene->Intersect(ray, dummy);
    if(result < tmax - tmax*0.00001f)
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