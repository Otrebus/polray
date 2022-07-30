#include "LightTracer.h"
#include "AreaLight.h"
#include "PhongMaterial.h" // temp


LightTracer::LightTracer(std::shared_ptr<Scene> scene) : Renderer(scene)
{
}

LightTracer::~LightTracer()
{
}

void LightTracer::Render(Camera& cam, ColorBuffer& colBuf)
{
    const double xres = (double)colBuf.GetXRes();
    const double yres = (double)colBuf.GetYRes();

    Vector3d dummyVector;
    double dummydouble;

    colBuf.Clear(0);

    for(int samples = 0; samples < xres*yres && ! stopping; samples++)
    {
        if(stopping)
            return;

        auto [light, lightWeight] = scene->PickLight(m_random.GetDouble(0.0f, 1.0f));
        auto [ray, pathColor, lightNormal, _, __] = light->SampleRay();

        pathColor *= light->GetArea()*light->GetIntensity(); // First direction is from the light source

        auto [u, v, lensPoint] = cam.SampleAperture();

        // Light going straight from the surface of the light source to the camera
        Vector3d lightToCam = lensPoint - ray.origin;
        Ray lightToCamRay(ray.origin, lightToCam);
        double camRayLength = lightToCam.GetLength();
        lightToCamRay.direction.Normalize();

        double camcos = abs(-lightToCamRay.direction*cam.dir);
        double pixelArea = (double)cam.GetPixelArea();
        double surfcos = abs(lightNormal*lightToCamRay.direction);
        surfcos = abs(surfcos);

        auto [hitCam, xPixel, yPixel] = cam.GetPixelFromRay(lightToCamRay, u, v);
        if(hitCam && TraceShadowRay(lightToCamRay, camRayLength))
            colBuf.AddColor(xPixel, yPixel, light->GetIntensity()*light->GetArea()*surfcos/(camcos*camcos*camcos*camRayLength*camRayLength*pixelArea*xres*yres)/lightWeight);
        do
        {
            const Primitive* minprimitive = nullptr;
            const Light* minlight = nullptr;
            IntersectionInfo info;
            Ray bounceRay;

            // Figure out if and where the current ray segment hits something
            if(scene->Intersect(ray, minprimitive, minlight) < 0.0f)
                break;
            if(minprimitive)
                minprimitive->GenerateIntersectionInfo(ray, info);
            else
                minlight->GenerateIntersectionInfo(ray, info);
            
            // Next event estimation
            auto [u, v, lensPoint] = cam.SampleAperture();

            Ray camRay = Ray(info.GetPosition(), lensPoint - info.GetPosition());
            double camRayLength = camRay.direction.GetLength();
            camRay.direction.Normalize();

            auto sample = info.GetMaterial()->GetSample(info, true);
            auto c = sample.color;
            bounceRay = sample.outRay;

            auto [hitCam, xPixel, yPixel] = cam.GetPixelFromRay(camRay, u, v);
            if(hitCam && TraceShadowRay(camRay, camRayLength))
            {
                double camcos = abs(-camRay.direction*cam.dir);
                double pixelArea = (double)cam.GetPixelArea();
                double surfcos = abs(info.GetGeometricNormal()*camRay.direction);

                Color brdf = info.GetMaterial()->BRDF(info, camRay.direction, sample.component);
                // Flux to radiance and stuff involving probability and sampling of the camera
                Color pixelColor = pathColor*surfcos*brdf/(camcos*camcos*camcos*camRayLength*camRayLength*pixelArea*xres*yres)/lightWeight;
                pixelColor*=abs(info.GetDirection()*info.GetNormal())/abs(info.GetDirection()*info.GetGeometricNormal());
                colBuf.AddColor(xPixel, yPixel, pixelColor);
            }
            
            // Bounce a new ray
            pathColor*= c/0.7f;
            ray = bounceRay;

        } while(m_random.GetDouble(0.f, 1.f) < 0.7f);
    }
}

unsigned int LightTracer::GetType() const
{
    return typeLightTracer;
}