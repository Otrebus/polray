#include "LightTracer.h"
#include "Sample.h"

/**
 * Constructor.
 * 
 * @param scene The scene that we render.
 */
LightTracer::LightTracer(std::shared_ptr<Scene> scene) : Renderer(scene)
{
}

/**
 * Destructor.
 */
LightTracer::~LightTracer()
{
}

/**
 * Adds a number of estimates of the importance transport equation.
 * 
 * @param cam The horizontal component of the pixel coordinate.
 * @param culBuf The color buffer to add sample estimates to.
 */
void LightTracer::Render(Camera& cam, ColorBuffer& colBuf)
{
    const double xres = (double)colBuf.GetXRes();
    const double yres = (double)colBuf.GetYRes();

    Vector3d dummyVector;

    colBuf.Clear(Color::Black);

    for(int samples = 0; samples < xres*yres && ! stopping; samples++)
    {
        auto [light, lightWeight] = scene->PickLight(m_random.GetDouble(0.0, 1.0));
        auto [ray, pathColor, lightNormal, _, __] = light->SampleRay(m_random);

        pathColor *= light->GetArea()*light->GetIntensity(); // First direction is from the light source

        auto firstU = m_random.GetDouble(0, 1), firstV = m_random.GetDouble(0, 1);
        auto firstLensPoint = cam.SampleAperture(firstU, firstV);

        // Light going straight from the surface of the light source to the camera
        Vector3d lightToCam = firstLensPoint - ray.origin;
        Ray lightToCamRay(ray.origin, lightToCam);
        double camRayLength = lightToCam.Length();
        lightToCamRay.direction.Normalize();

        double camcos = abs(-lightToCamRay.direction*cam.dir);
        double pixelArea = (double)cam.GetPixelArea();
        double surfcos = abs(lightNormal*lightToCamRay.direction);
        surfcos = abs(surfcos);

        auto [firstHitCam, firstXPixel, firstYPixel] = cam.GetPixelFromRay(lightToCamRay, firstU, firstV);
        if(firstHitCam && TraceShadowRay(lightToCamRay, camRayLength))
            colBuf.AddColor(firstXPixel, firstYPixel, light->GetIntensity()*light->GetArea()*surfcos/(camcos*camcos*camcos*camRayLength*camRayLength*pixelArea*xres*yres)/lightWeight);
        do
        {
            IntersectionInfo info;
            Ray bounceRay;

            // Figure out if and where the current ray segment hits something
            auto [t, minprimitive, minlight] = scene->Intersect(ray);
            if(t < 0.0)
                break;
            if(minprimitive)
                minprimitive->GenerateIntersectionInfo(ray, info);
            else
                minlight->GenerateIntersectionInfo(ray, info);
            
            // Next event estimation
            auto u = m_random.GetDouble(0, 1), v = m_random.GetDouble(0, 1);
            auto lensPoint = cam.SampleAperture(u, v);

            Ray camRay = Ray(info.position, lensPoint - info.position);
            camRayLength = camRay.direction.Length();
            camRay.direction.Normalize();

            auto sample = info.material->GetSample(info, m_random, true);
            auto c = sample.color;
            bounceRay = sample.outRay;

            auto [hitCam, xPixel, yPixel] = cam.GetPixelFromRay(camRay, u, v);
            if(hitCam && TraceShadowRay(camRay, camRayLength))
            {
                camcos = abs(-camRay.direction*cam.dir);
                pixelArea = (double)cam.GetPixelArea();
                surfcos = abs(info.geometricnormal*camRay.direction);

                Color brdf = info.material->BRDF(info, camRay.direction, sample.component);
                // Flux to radiance and stuff involving probability and sampling of the camera
                Color pixelColor = pathColor*surfcos*brdf/(camcos*camcos*camcos*camRayLength*camRayLength*pixelArea*xres*yres)/lightWeight;
                pixelColor*=abs(info.direction*info.normal)/abs(info.direction*info.geometricnormal);
                colBuf.AddColor(xPixel, yPixel, pixelColor);
            }
            
            // Bounce a new ray
            pathColor*= c/0.7;
            ray = bounceRay;

        } while(m_random.GetDouble(0.f, 1.f) < 0.7);
    }
}

/**
 * Saves information about the renderer to a bytestream.
 * 
 * @param stream The bytestream to stream to.
 */
void LightTracer::Save(Bytestream& stream) const
{
    stream << ID_LIGHTTRACER;
}

/**
 * Loads the renderer from a bytestream.
 * 
 * @param stream The bytestream to stream from.
 */
void LightTracer::Load(Bytestream& stream)
{
}