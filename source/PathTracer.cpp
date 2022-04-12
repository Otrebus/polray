#include "PathTracer.h"
#include "AreaLight.h"
#include "GeometricRoutines.h"
#include "Primitive.h"
#include "Scene.h"
#include "Material.h"
#include <intrin.h>

#define CHECKVALID(v) if(!( v.x > -100 && v.x < 100 && v.y > -100 && v.y < 100 && v.z > -100 && v.z < 100 && v.x < std::numeric_limits<float>::infinity() && v.x > -std::numeric_limits<float>::infinity() && v.x == v.x && v.y < std::numeric_limits<float>::infinity() && v.y > -std::numeric_limits<float>::infinity() && v.y == v.y && v.z < std::numeric_limits<float>::infinity() && v.z > -std::numeric_limits<float>::infinity() && v.z == v.z)) __debugbreak();

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
	IntersectionInfo info;

	if(scene->Intersect(ray, minprimitive) < 0)
		return Color(0, 0, 0);
	minprimitive->GenerateIntersectionInfo(ray, info);

	if(m_random.GetFloat(0, 1) > 0.8f)
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
		dir = Vector3d(m_random.GetFloat(-1, 1), m_random.GetFloat(-1, 1), m_random.GetFloat(-1, 1));

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
	Color c = mod*TracePath(out)/0.8f;
	return c;
}

//Color PathTracer::TracePath(const Ray& ray) const
//{
//	// First, shoot a single primary ray (treated separately just to
//	// make the light to eye case simpler)
//	const Primitive* minprimitive;
//	IntersectionInfo info;
//	Ray outRay, inRay;
//	Color pathColor(1.f, 1.f, 1.f);
//	Color finalColor(0, 0, 0);
//
//	bool sampledLight = true;
//
//	if(m_tree.Intersect(ray, minprimitive) < 0)
//        return Color(0, 0, 0);
//		//return 3*Color(0.9, 1.2, 1.5); // - or the environment map for this direction
//	minprimitive->GenerateIntersectionInfo(ray, info);
//
//	//		if(info.material->IsSpecular(1))
//	//		_asm int 3;
//
//	//if(minshape->material->GetEmissivity().GetIntensity() > 0) 
//    Light* light = minprimitive->GetMaterial()->GetLight();
//    if(light)
//		return (info.GetNormal()*info.GetDirection() < 0 ) ? light->GetIntensity() : 0; // Primary camera ray hit a light, just return intensity
//
//	inRay = ray;
//
//	do
//	{
//
//		// temporary testing stuff
//
//		// Do next event estimation (assumes a single area light for now)
//		Material* material = info.GetMaterial();
//		//AreaLight* light = (AreaLight*) m_lights.front();
//        float lightWeight; // lol
//        float r = m_random.GetFloat(0.0f, 1.0f);
//        Light* light = lightTree->PickLight(r, lightWeight);
//        Vector3d lightNormal;
//		//Vector3d lightNormal = light->GetNormal();
//		//Vector3d normal = info.GetGeometricNormal();
//        Vector3d normal = info.GetNormal();
//        
//		if(info.GetMaterial()->GetLight() == light)
//		{
//			if(sampledLight)
//				return finalColor;
//			else return pathColor*((info.GetNormal()*info.GetDirection() < 0 ) ? light->GetIntensity()/lightWeight : Color(0, 0, 0));
//		}
//
//        Vector3d lightPoint;
//        light->SamplePoint(lightPoint, lightNormal);
//		Vector3d toLight = lightPoint-info.GetPosition();
//
//		if(material->IsSpecular(1))
//			sampledLight = false;
//		else if(toLight*lightNormal < 0)
//		{
//			float d = toLight.GetLength();
//			toLight.Normalize();
//
//			Ray lightRay = Ray(info.GetPosition() + normal*0.0001f, toLight); //(normal*toLight>0 ? normal*0.0001f : normal*-0.0001f), toLight);
//
//			if(TraceShadowRay(lightRay, d))
//			{
//				float cosphi = abs(normal*toLight);
//				float costheta = abs(toLight*lightNormal);
//				finalColor += (lightNormal*toLight < 0) ? pathColor*material->BRDF(info, toLight)*costheta*cosphi*light->GetIntensity()*light->GetArea()/(d*d*lightWeight) : Color(0, 0, 0);
//				sampledLight = true;
//			}
//		}
//		// GetSampleE(info, newVertex->out, lastPdf, newVertex->rpdf, newVertex->component);
//		float dummy1;
//		float dummy2;
//		unsigned char blah = 1;
//		//pathColor*=material->GetSampleE(info, outRay, dummy1, dummy2, blah, false)/0.7f;
//		pathColor*=material->GetSample(info, outRay, false)/0.7f;
//		inRay = outRay;
//
//		if(m_tree.Intersect(inRay, minprimitive) < 0)
//        {
//            //finalColor += pathColor*3*Color(0.9, 1.2, 1.5);
//			break;
//        }
//		minprimitive->GenerateIntersectionInfo(inRay, info);
//		
//	}
//	while(m_random.GetFloat(0, 1) < 0.7f);
//	return finalColor;
//}


//Color PathTracer::TracePath(const Ray& ray) const
//{
//    const Primitive* minprimitive;
//    IntersectionInfo info;
//    Ray outRay, inRay;
//    Color pathColor(1.f, 1.f, 1.f);
//    Color finalColor(0, 0, 0);
//	bool sampledLight = false;
//	inRay = ray;
//    float rr = 1.0f;
//    float lastPdf;
//    float dummy;
//    unsigned char lastComp;
//    bool firstRay = true;
//    IntersectionInfo lastInfo;
//
//    float lightWeight;
//    float r = m_random.GetFloat(0.0f, 1.0f);
//    Light* light = lightTree->PickLight(r, lightWeight);
//
//	do
//	{
//        //CHECKVALID(inRay.origin);
//        CHECKVALID(inRay.direction);
//        if(m_tree.Intersect(inRay, minprimitive) < 0)
//        {
//            if(scene->GetEnvironmentLight())
//              return finalColor + pathColor*scene->GetEnvironmentLight()->GetRadiance(outRay.direction);
//			return Color(0, 0, 0);
//        }
//		minprimitive->GenerateIntersectionInfo(inRay, info);
//		Material* material = info.GetMaterial();
//
//        // Randomly interesected a light source
//		if(info.GetMaterial()->GetLight() == light)
//		{
//			if(!firstRay)
//                if(sampledLight)
//                    return info.GetNormal()*info.GetDirection() < 0 ? finalColor + pathColor*light->DirectHitMIS(this, lastInfo, info, lastComp)/lightWeight : finalColor;
//                else
//                    return info.GetNormal()*info.GetDirection() < 0 ? finalColor + pathColor*light->GetIntensity()/lightWeight : finalColor;
//            else
//                return info.GetNormal()*info.GetDirection() < 0 
//                       ? light->GetIntensity()/lightWeight : Color(0, 0, 0);
//		}
//
//        Color sample = material->GetSampleE(info, outRay, lastPdf, dummy, lastComp, false);
//        // No use to sample using next event estimation if the material
//        // is specular since the BRDF will be 0
//        if(material->IsSpecular(lastComp))
//			sampledLight = false;
//		else
//        {
//            finalColor += pathColor*light->NextEventEstimationMIS(this, info, lastComp)/lightWeight;
//            //finalColor += pathColor*light->NextEventEstimation(this, info, lastComp)/lightWeight;
//            sampledLight = true;
//        }
//        lastInfo = info;
//        pathColor *= sample/0.7f;
//        firstRay = false;
//        inRay = outRay;
//	}
//    while(m_random.GetFloat(0, 1) < 0.7f);
//	
//    if(finalColor.IsValid())
//        return finalColor;
//    else
//        return Color(0, 0, 0);
//}

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
                    float q = m_random.GetFloat(0, 1);
                    float p = m_random.GetFloat(0, 1);
                    float u, v;
                    Vector3d pos;
                    cam.SampleAperture(pos, u, v);
                    Ray outRay = cam.GetRayFromPixel(x, y, q, p, u, v);
                    result += TracePath(outRay);
                }
            }
            colBuf.SetPixel(x, y, result/(float)m_SPP);
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
    IntersectionInfo info;
    Ray outRay, inRay;
    Color pathColor(1.f, 1.f, 1.f);
    Color finalColor(0, 0, 0);
    bool sampledLight = false;
    inRay = ray;
    float rr = 1.0f;

    float lightWeight;
    float r = m_random.GetFloat(0.0f, 1.0f);
    Light* light = lightTree->PickLight(r, lightWeight);

    do
    {
        if(scene->Intersect(inRay, minprimitive) < 0)
        {
        //    if(scene->GetEnvironmentLight())
        //      return scene->GetEnvironmentLight()->GetRadiance(outRay.direction);
        //    3*Color(0.9, 1.2, 1.5) was the original sky color
            break;
        }
        minprimitive->GenerateIntersectionInfo(inRay, info);
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
            finalColor += pathColor*light->NextEventEstimation(this, info)/lightWeight;
            sampledLight = true;
        }
        pathColor *= c/0.7f;
        inRay = outRay;
    }   
    while(m_random.GetFloat(0, 1) < 0.7f);
    
    return finalColor;
}