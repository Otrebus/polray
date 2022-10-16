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
            auto u = m_random.GetDouble(0, 1), v = m_random.GetDouble(0, 1);

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
    IntersectionInfo info;
    Ray outRay, inRay = ray;
    Color pathColor = Color::Identity, finalColor = Color::Black;
    bool sampledLight = false;

    double r = m_random.GetDouble(0.0, 1.0);
    auto [light, lightWeight] = scene->PickLight(r);

    do
    {
        auto [t, minprimitive, minlight] = scene->Intersect(inRay);
        if(t < 0)
            break;

        if(minprimitive)
            minprimitive->GenerateIntersectionInfo(inRay, info);
        else
            minlight->GenerateIntersectionInfo(inRay, info);
       
        // Randomly interesected a light source
        if(info.material->GetLight() == light)
        {
            if(sampledLight) // We already sampled with next event estimation
                return finalColor/lightWeight;
            else return pathColor*( info.normal*info.direction < 0
                        ? light->GetIntensity()/lightWeight : Color::Black );
        }
        else if(info.material->GetLight())
            return finalColor/lightWeight;

        auto sample = info.material->GetSample(info, m_random, false);
        if(sample.specular) // Next event estimation would be zero since BRDF is an impulse function
            sampledLight = false;
        else
        {
            auto [color, lightPoint] = light->NextEventEstimation(this, info, m_random, sample.component);
            finalColor += pathColor*color;
            sampledLight = true;
        }
        pathColor *= sample.color/0.7;
        inRay = sample.outRay;
    }   
    while(m_random.GetDouble(0, 1) < 0.7);
    
    return finalColor/lightWeight;
}
