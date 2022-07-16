#include "LightTracer.h"
#include "AreaLight.h"
#include "PhongMaterial.h" // temp


LightTracer::LightTracer(std::shared_ptr<Scene> scene) : Renderer(scene)
{
    m_SPP = 1;
}

LightTracer::~LightTracer()
{
}

void LightTracer::RenderPart(Camera& cam, ColorBuffer& colBuf) const
{
    const double xres = (double)colBuf.GetXRes();
    const double yres = (double)colBuf.GetYRes();

    Vector3d dummyVector;
    double dummydouble;

    colBuf.Clear(0);

    for(int samples = 0; samples < xres*yres*m_SPP && ! stopping; samples++)
    {
        if(stopping)
            return;
        int xpixel = (int)xres;
        int ypixel = (int)yres;

        auto [light, lightWeight] = scene->PickLight(m_random.GetDouble(0.0f, 1.0f));
        auto [ray, pathColor, lightNormal, _, __] = light->SampleRay();

        pathColor *= light->GetArea()*light->GetIntensity(); // First direction is from the light source

        double u, v;
        Vector3d lensPoint;
        cam.SampleAperture(lensPoint, u, v);

        // Light going straight from the surface of the light source to the camera
        Vector3d lightToCam = lensPoint - ray.origin;
        Ray lightToCamRay(ray.origin, lightToCam);
        double camRayLength = lightToCam.GetLength();
        lightToCamRay.direction.Normalize();

        double camcos = abs(-lightToCamRay.direction*cam.dir);
        double pixelArea = (double)cam.GetPixelArea();
        double surfcos = abs(lightNormal*lightToCamRay.direction);
        surfcos = abs(surfcos);

        if(TraceShadowRay(lightToCamRay, camRayLength) && cam.GetPixelFromRay(lightToCamRay, xpixel, ypixel, u, v))
            colBuf.AddColor(xpixel, ypixel, light->GetIntensity()*light->GetArea()*surfcos/(camcos*camcos*camcos*camRayLength*camRayLength*pixelArea*xres*yres*m_SPP)/lightWeight);

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
            double u, v;
            Vector3d lensPoint;
            cam.SampleAperture(lensPoint, u, v);

            Ray camRay = Ray(info.GetPosition(), lensPoint - info.GetPosition());
            double camRayLength = camRay.direction.GetLength();
            camRay.direction.Normalize();

            auto sample = info.GetMaterial()->GetSample(info, true);
            auto c = sample.color;
            bounceRay = sample.outRay;

            if(TraceShadowRay(camRay, camRayLength) && cam.GetPixelFromRay(camRay, xpixel, ypixel, u, v))
            {
                double camcos = abs(-camRay.direction*cam.dir);
                double pixelArea = (double)cam.GetPixelArea();
                double surfcos = abs(info.GetGeometricNormal()*camRay.direction);

                Color brdf = info.GetMaterial()->BRDF(info, camRay.direction, sample.component);
                // Flux to radiance and stuff involving probability and sampling of the camera
                Color pixelColor = pathColor*surfcos*brdf/(camcos*camcos*camcos*camRayLength*camRayLength*pixelArea*xres*yres*m_SPP)/lightWeight;
                pixelColor*=abs(info.GetDirection()*info.GetNormal())/abs(info.GetDirection()*info.GetGeometricNormal());
                colBuf.AddColor(xpixel, ypixel, pixelColor);
            }
            
            // Bounce a new ray
            pathColor*= c/0.7f;
            ray = bounceRay;

        } while(m_random.GetDouble(0.f, 1.f) < 0.7f);
    }
}

void LightTracer::Render(Camera& cam, ColorBuffer& colBuf)
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);

    int nCores = sysinfo.dwNumberOfProcessors;
    ColorBuffer** partBuf = new ColorBuffer*[nCores];

    colBuf.Clear(0);

    stopping = false;

#pragma omp parallel for
    for(int i = 0; i < nCores; i++)
    {
        partBuf[i] = new ColorBuffer(colBuf.GetXRes(), colBuf.GetYRes());
        LightTracer::RenderPart(cam, *partBuf[i]);
    }
    if(stopping)
        return;
    for(int i = 0; i < nCores; i++)
        for(int x = 0; x < colBuf.GetXRes(); x++)
            for(int y = 0; y < colBuf.GetYRes(); y++)
                    colBuf.AddColor(x, y, partBuf[i]->GetPixel(x, y)/(double)nCores);
    for(int i = 0; i < nCores; i++)
        delete partBuf[i];
}

void LightTracer::SetSPP(unsigned int spp)
{
    m_SPP = spp;
}

unsigned int LightTracer::GetSPP() const
{
    return m_SPP;
}

unsigned int LightTracer::GetType() const
{
    return typeLightTracer;
}