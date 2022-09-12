#include "PathTracer.h"
#include "Primitive.h"
#include "Sample.h"

PathTracer::PathTracer(std::shared_ptr<Scene> scene) : Renderer(scene)
{
    //m_random.Seed(GetTickCount());
#ifdef DETERMINISTIC
    m_random.Seed();
#endif
}

PathTracer::~PathTracer()
{
}

//Color PathTracer::TracePathPrimitive(const Ray& ray) const
//{
//	const Primitive* minprimitive = nullptr;
//    const Light* minlight = nullptr;
//	IntersectionInfo info;
//
//	if(scene->Intersect(ray, minprimitive, minlight) < 0)
//		return Color(0, 0, 0);
//    if(minprimitive)
//	    minprimitive->GenerateIntersectionInfo(ray, info);
//    else
//        minlight->GenerateIntersectionInfo(ray, info);
//
//	if(m_random.GetDouble(0, 1) > 0.8f)
//		return Color(0, 0, 0);
//
//    Ray out;
//	Vector3d dir;
//
//    Light* l;
//    if((l = info.material->GetLight()) && info.geometricnormal * info.direction < 0)
//        return l->GetIntensity()/0.8f;
//
//    while(true)
//	{
//        Vector3d N_g = info.geometricnormal;
//        if(N_g*info.direction > 0)
//            N_g = -N_g;
//		dir = Vector3d(m_random.GetDouble(-1, 1), m_random.GetDouble(-1, 1), m_random.GetDouble(-1, 1));
//
//		out.origin = info.position + 0.0001f*N_g;
//		if(dir.GetLength() > 1)
//			continue;
//		else if(dir*N_g < 0)
//			dir = -dir;
//		dir.Normalize();
//		break;
//	}
//    Color mod = info.material->BRDF(info, dir)*2*pi*abs(info.normal*dir);
//
//	out.direction = dir;
//	Color c = mod*TracePathPrimitive(out)/0.8f;
//	return c;
//}

void PathTracer::Render(Camera& cam, ColorBuffer& colBuf)
{
    int xres = colBuf.GetXRes();
    int yres = colBuf.GetYRes();

    for(int y = 0; y < yres; y++)
    {
        for(int x = 0; x < xres && !stopping; x++)
        {
            double q = m_random.GetDouble(0, 1), p = m_random.GetDouble(0, 1);
            auto [u, v, pos] = cam.SampleAperture();
            Ray outRay = cam.GetRayFromPixel(x, y, q, p, u, v);
            Color result = TracePath(outRay);
            colBuf.SetPixel(x, y, result);
        }
    }
}

void PathTracer::Save(Bytestream& stream) const
{
    stream << ID_PATHTRACER;
}

void PathTracer::Load(Bytestream&)
{
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

    double r = m_random.GetDouble(0.0f, 1.0f);
    auto [light, lightWeight] = scene->PickLight(r);

    do
    {
        minprimitive = nullptr;
        minlight = nullptr;
        if(scene->Intersect(inRay, minprimitive, minlight) < 0)
            break;

        if(minprimitive)
            minprimitive->GenerateIntersectionInfo(inRay, info);
        else
            minlight->GenerateIntersectionInfo(inRay, info);
        Material* material = info.material;
        //if(!info.material)
        //    break;
       
        // Randomly interesected a light source
        if(info.material->GetLight() == light)
        {
            if(sampledLight) // We already sampled with next event estimation
                return finalColor/lightWeight;
            else return pathColor*( info.normal*info.direction < 0
                        ? light->GetIntensity()/lightWeight : Color(0, 0, 0) );
        } else if(info.material->GetLight()) {
            return finalColor/lightWeight;
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
            auto [color, lightPoint] = light->NextEventEstimation(this, info, sample.component);
            finalColor += pathColor*color;
            sampledLight = true;
        }
        pathColor *= c/0.7f;
        inRay = outRay;
    }   
    while(m_random.GetDouble(0, 1) < 0.7f);
    
    return finalColor/lightWeight;
}
