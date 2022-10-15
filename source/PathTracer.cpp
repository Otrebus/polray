#include "PathTracer.h"
#include "Primitive.h"
#include "Sample.h"

/**
 * Constructor.
 * 
 * @param scene The scene that we render.
 */
PathTracer::PathTracer(std::shared_ptr<Scene> scene) : Renderer(scene)
{
}

/**
 * Destructor.
 */
PathTracer::~PathTracer()
{
}

/**
 * Calculates the contribution of one sample of the path tracing algorithm using no importance
 * sampling.
 * 
 * @param ray The direction of the ray whose first intersection with the scene we calculate the
 *            rendering equation integral at.
 */
Color PathTracer::TracePathPrimitive(const Ray& ray)
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

	if(m_random.GetDouble(0, 1) > 0.8)
		return Color(0, 0, 0);

    Ray out;
	Vector3d dir;

    Light* l;
    if((l = info.material->GetLight()) && info.geometricnormal * info.direction < 0)
        return l->GetIntensity()/0.8;

    while(true)
	{
        Vector3d N_g = info.geometricnormal;
        if(N_g*info.direction > 0)
            N_g = -N_g;
		dir = Vector3d(m_random.GetDouble(-1, 1), m_random.GetDouble(-1, 1), m_random.GetDouble(-1, 1));

		out.origin = info.position + eps*N_g;
		if(dir.Length() > 1)
			continue;
		else if(dir*N_g < 0)
			dir = -dir;
		dir.Normalize();
		break;
	}

    // TODO: Need to unhardcode the component to make this sort of sampling work
    Color mod = info.material->BRDF(info, dir, 1)*2*pi*abs(info.normal*dir);

	out.direction = dir;
	Color c = mod*TracePathPrimitive(out)/0.8f;
	return c;
}

/**
 * Calculates one sample per pixel using path tracing from the given camera, into the given
 * color buffer.
 * 
 * @param cam The camera from whose perspective we render.
 * @param colBuf The color buffer that we dump pixel contributions into.
 */
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

/**
 * Saves information about the renderer to a bytestream.
 * 
 * @param stream The bytestream to stream to.
 */
void PathTracer::Save(Bytestream& stream) const
{
    stream << ID_PATHTRACER;
}

/**
 * Loads the renderer from a bytestream.
 * 
 * @param stream The bytestream to stream from.
 */
void PathTracer::Load(Bytestream& stream)
{
}

/**
 * Calculates the contribution of one sample of the path tracing algorithm.
 * 
 * @param ray The ray to trace.
 * @returns The contribution of the sample.
 */
Color PathTracer::TracePath(const Ray& ray)
{
    const Primitive* minprimitive = nullptr;
    const Light* minlight = nullptr;
    IntersectionInfo info;
    Ray outRay, inRay;
    Color pathColor(1.f, 1.f, 1.f);
    Color finalColor(0, 0, 0);
    bool sampledLight = false;
    inRay = ray;

    double r = m_random.GetDouble(0.0, 1.0);
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
        }
        else if(info.material->GetLight())
            return finalColor/lightWeight;

        auto sample = material->GetSample(info, m_random, false);
        auto c = sample.color;
        outRay = sample.outRay;

        // No use to sample using next event estimation if the material
        // is specular since the BRDF will be 0
        if(sample.specular)
            sampledLight = false;
        else
        {
            auto [color, lightPoint] = light->NextEventEstimation(this, info, m_random, sample.component);
            finalColor += pathColor*color;
            sampledLight = true;
        }
        pathColor *= c/0.7;
        inRay = outRay;
    }   
    while(m_random.GetDouble(0, 1) < 0.7);
    
    return finalColor/lightWeight;
}
