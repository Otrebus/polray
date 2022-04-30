#include "PathTracer.h"
#include "AreaLight.h"
#include "GeometricRoutines.h"
#include "Primitive.h"
#include "Scene.h"
#include "Material.h"
#include <intrin.h>

#define CHECKVALID(v) if(!( v.x > -100 && v.x < 100 && v.y > -100 && v.y < 100 && v.z > -100 && v.z < 100 && v.x < std::numeric_limits<double>::infinity() && v.x > -std::numeric_limits<double>::infinity() && v.x == v.x && v.y < std::numeric_limits<double>::infinity() && v.y > -std::numeric_limits<double>::infinity() && v.y == v.y && v.z < std::numeric_limits<double>::infinity() && v.z > -std::numeric_limits<double>::infinity() && v.z == v.z)) __debugbreak();

PathTracer::PathTracer(std::shared_ptr<Scene> scene) : Renderer(scene)
{
    //m_random.Seed(GetTickCount());
#ifdef DETERMINISTIC
    m_random.Seed(0);
#else
    m_random.Seed(GetTickCount() + int(this));
#endif
    m_SPP = 1;
}

PathTracer::~PathTracer()
{
}

void PathTracer::SetSPP(unsigned int s)
{
    m_SPP = s;
}

unsigned int PathTracer::GetSPP() const
{
    return m_SPP;
}

Color PathTracer::TracePathPrimitive(const Ray& ray) const
{
	const Primitive* minprimitive = nullptr;
    const Light* minlight = nullptr;
	IntersectionInfo info;

	if(scene->Intersect(ray, minprimitive, minlight) < 0)
		return Color(0, 0, 0);
    if(minprimitive)
	    minprimitive->GenerateIntersectionInfo(ray, info);
    else
        minlight->GenerateIntersectionInfo(ray, info);

	if(m_random.GetDouble(0, 1) > 0.8f)
		return Color(0, 0, 0);

    Ray out;
	Vector3d dir;

    Light* l;
    if((l = info.GetMaterial()->GetLight()) && info.GetGeometricNormal() * info.GetDirection() < 0)
        return l->GetIntensity()/0.8f;

    while(true)
	{
        Vector3d N_g = info.GetGeometricNormal();
        if(N_g*info.GetDirection() > 0)
            N_g = -N_g;
		dir = Vector3d(m_random.GetDouble(-1, 1), m_random.GetDouble(-1, 1), m_random.GetDouble(-1, 1));

		out.origin = info.GetPosition() + 0.0001f*N_g;
		if(dir.GetLength() > 1)
			continue;
		else if(dir*N_g < 0)
			dir = -dir;
		dir.Normalize();
		break;
	}
    Color mod = info.GetMaterial()->BRDF(info, dir)*2*F_PI*abs(info.GetNormal()*dir);

	out.direction = dir;
	Color c = mod*TracePathPrimitive(out)/0.8f;
	return c;
}

void PathTracer::Render(Camera& cam, ColorBuffer& colBuf)
{
    int xres = colBuf.GetXRes();
    int yres = colBuf.GetYRes();

    for(int y = 0; y < yres; y++)
    {
        for(int x = 0; x < xres; x++)
        {
            Color result(0, 0, 0);
            for(int i = 0; i < m_SPP; i++)
            {
                if(stopping)
                    colBuf.SetPixel(x, y, Color(0, 0, 0));
                else
                {
                    double q = m_random.GetDouble(0, 1);
                    double p = m_random.GetDouble(0, 1);
                    double u, v;
                    Vector3d pos;
                    cam.SampleAperture(pos, u, v);
                    Ray outRay = cam.GetRayFromPixel(x, y, q, p, u, v);
                    result += TracePath(outRay);
                }
            }
            colBuf.SetPixel(x, y, result/(double)m_SPP);
        }
    }
}

unsigned int PathTracer::GetType() const
{
    return typePathTracer;
}


Color PathTracer::TracePath(const Ray& ray) const
{
    const Primitive* minprimitive = nullptr;
    const Light* minlight = nullptr;
    IntersectionInfo info;
    Ray outRay, inRay;
    Color pathColor(1.f, 1.f, 1.f);
    Color finalColor(0, 0, 0);
    bool sampledLight = false;
    inRay = ray;
    double rr = 1.0f;

    double lightWeight;
    double r = m_random.GetDouble(0.0f, 1.0f);
    Light* light = lightTree->PickLight(r, lightWeight);

    do
    {
        if(scene->Intersect(inRay, minprimitive, minlight) < 0)
        {
        //    if(scene->GetEnvironmentLight())
        //      return scene->GetEnvironmentLight()->GetRadiance(outRay.direction);
        //    3*Color(0.9, 1.2, 1.5) was the original sky color
            break;
        }
        if(minprimitive)
            minprimitive->GenerateIntersectionInfo(inRay, info);
        else
            minlight->GenerateIntersectionInfo(inRay, info);
        Material* material = info.GetMaterial();
       
        // Randomly interesected a light source
        if(info.GetMaterial()->GetLight() == light)
        {
            if(sampledLight) // We already sampled with next event estimation
                return finalColor;
            else return pathColor*( info.GetNormal()*info.GetDirection() < 0
                        ? light->GetIntensity()/lightWeight : Color(0, 0, 0) );
        }

        auto sample = material->GetSample(info, false);
        auto c = sample.color;
        outRay = sample.outRay;

        // No use to sample using next event estimation if the material
        // is specular since the BRDF will be 0
        if(sample.specular)
            sampledLight = false;
        else
        {
            Vector3d dummy;
            finalColor += pathColor*light->NextEventEstimation(this, info, dummy, dummy)/lightWeight;
            sampledLight = true;
        }
        pathColor *= c/0.7f;
        inRay = outRay;
    }   
    while(m_random.GetDouble(0, 1) < 0.7f);
    
    return finalColor;
}