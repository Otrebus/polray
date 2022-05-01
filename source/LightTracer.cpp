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

/*void LightTracer::Setup(const std::vector<Primitive*>& primitives, const std::vector<Light*>& lights)
{
    m_tree.Build(primitives);
    m_lights = lights;
    lightTree = new LightNode(lights);
}*/

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

        double weight;
        Light* light = lightTree->PickLight(m_random.GetDouble(0.0f, 1.0f), weight);
        Ray ray; 
        Vector3d lightNormal;

        Color pathColor = light->SampleRay(ray, lightNormal, dummydouble, dummydouble)*light->GetArea()*light->GetIntensity()/weight; // First direction is from the light source
        ray.direction.Normalize();

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
            colBuf.AddColor(xpixel, ypixel, light->GetIntensity()*surfcos/(camcos*camcos*camcos*camRayLength*camRayLength*pixelArea*xres*yres*m_SPP));

        // pathColor*=abs(ray.direction*light->GetNormal());

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

            ////

            //xpixel = colBuf.GetXRes();
            //ypixel = colBuf.GetYRes();

            double u, v;
            Vector3d lensPoint;
            cam.SampleAperture(lensPoint, u, v);

            Ray camRay = Ray(info.GetPosition(), lensPoint - info.GetPosition());
            double camRayLength = camRay.direction.GetLength();
            camRay.direction.Normalize();

            /*double u = m_random.Getdouble(0, 2*F_PI);
            double v = sqrt(m_random.Getdouble(0, 1));

            Vector3d camLeft = cam.up^cam.dir;
            Vector3d lensPoint = cam.pos+0.15f*v*(camLeft*cos(u)+cam.up*sin(u));

            Vector3d toCam = lensPoint - info.GetPosition();
            double camRayLength = toCam.GetLength();
            toCam.Normalize();
            toCam *= 2.75f/(-toCam*cam.dir);
            Vector3d focalPoint = lensPoint - toCam;
            Ray centerRay(focalPoint, cam.pos - focalPoint);
            Ray camRay(info.GetPosition() + info.GetNormal()*0.00001f, toCam);
            camRay.direction.Normalize();
            centerRay.direction.Normalize();*/

            auto sample = info.GetMaterial()->GetSample(info, true);
            auto c = sample.color;
            bounceRay = sample.outRay;

            if(TraceShadowRay(camRay, camRayLength) && cam.GetPixelFromRay(camRay, xpixel, ypixel, u, v))
            {
                double camcos = abs(-camRay.direction*cam.dir);
                double pixelArea = (double)cam.GetPixelArea();
                double surfcos = abs(info.GetGeometricNormal()*camRay.direction);
                //double surfcos = info.GetNormal()*camRay.direction;
                surfcos = abs(surfcos);

                Color brdf = info.GetMaterial()->BRDF(info, camRay.direction);
                // Flux to radiance and stuff involving probability and sampling of the camera
                Color pixelColor = pathColor*surfcos*brdf/(camcos*camcos*camcos*camRayLength*camRayLength*pixelArea*xres*yres*m_SPP);
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

/*
LightTracer::LightTracer()
{
    m_SPP = 1;
}

LightTracer::~LightTracer()
{
}

void LightTracer::Setup(const std::vector<Shape*>& shapes, const std::vector<Light*>& lights)
{
    m_tree.Build(shapes);
    m_lights = lights;
}

void LightTracer::RenderPart(Camera& cam, ColorBuffer& colBuf) const
{
    const double xres = (double)colBuf.GetXRes();
    const double yres = (double)colBuf.GetYRes();

    colBuf.Clear(0);

    for(int samples = 0; samples < xres*yres*m_SPP && ! g_quitting; samples++)
    {
        int xpixel = (int)xres;
        int ypixel = (int)yres;
        AreaLight* light = (AreaLight*)m_lights.front(); // just a single area light allowed at the moment
        Ray ray; 

        Color pathColor = light->Sample(ray); // First direction is from the light source
        ray.direction.Normalize();

        // Light going straight from the surface of the light source to the camera
        Vector3d lightToCam = cam.pos - ray.origin;
        Ray lightToCamRay(ray.origin, lightToCam);
        double length = lightToCam.GetLength();
        lightToCamRay.direction.Normalize();

        if(TraceShadowRay(lightToCamRay, length) && cam.GetPixelFromRay(lightToCamRay, xpixel, ypixel))
            colBuf.SetPixel(xpixel, ypixel, light->intensity);

        pathColor*=abs(ray.direction*light->GetNormal());

        do
        {
            Shape* minshape;
            IntersectionInfo info;
            Ray bounceRay;

            // Figure out if and where the current ray segment hits something
            if(m_tree.Intersect(ray, minshape) < 0.0f)
                break;
            minshape->GenerateIntersectionInfo(ray, info);
            
            // Next event estimation
            Ray camRay;
            camRay.origin = info.GetPosition();// + (info.GetGeometricNormal()*camRay.direction>0 ? info.GetGeometricNormal()*0.0001f : info.GetGeometricNormal()*-0.0001f);
            camRay.direction = cam.pos - camRay.origin;

            double camRayLength = camRay.GetDirection().GetLength();
            camRay.direction.Normalize();

            camRay.origin += camRay.direction*0.0001f;

            xpixel = colBuf.GetXRes();
            ypixel = colBuf.GetYRes();

            if(TraceShadowRay(camRay, camRayLength) && cam.GetPixelFromRay(Ray(info.GetPosition(), camRay.direction), xpixel, ypixel))
            {
                double camcos = -camRay.direction*cam.dir;
                double pixelArea = (double)cam.GetPixelArea(colBuf.GetXRes(), colBuf.GetYRes());
                double surfcos = info.GetGeometricNormal()*camRay.direction;
                //double surfcos = info.GetNormal()*camRay.direction;
                surfcos = abs(surfcos);

                Color brdf = info.GetMaterial()->BRDF(info, camRay.direction);
                // Flux to radiance and stuff involving probability and sampling of the camera
                Color pixelColor = pathColor*surfcos*brdf/(camcos*camcos*camcos*camRayLength*camRayLength*pixelArea*xres*yres*m_SPP);
                pixelColor*=abs(info.GetDirection()*info.GetNormal())/abs(info.GetDirection()*info.GetGeometricNormal());
                colBuf.AddColor(xpixel, ypixel, pixelColor);
            }

            // Bounce a new ray
            pathColor*=info.GetMaterial()->GetSample(info, bounceRay, true)/0.7f;
            ray = bounceRay;

        } while(m_random.Getdouble(0.f, 1.f) < 0.7f);
    }
}

void LightTracer::Render(Camera& cam, ColorBuffer& colBuf) const
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);

    int nCores = sysinfo.dwNumberOfProcessors;
    ColorBuffer** partBuf = new ColorBuffer*[nCores];

    colBuf.Clear(0);

#pragma omp parallel for
    for(int i = 0; i < nCores; i++)
    {
        partBuf[i] = new ColorBuffer(colBuf.GetXRes(), colBuf.GetYRes());
        LightTracer::RenderPart(cam, *partBuf[i]);
    }
    for(int i = 0; i < nCores; i++)
        for(int x = 0; x < colBuf.GetXRes(); x++)
            for(int y = 0; y < colBuf.GetYRes(); y++)
                    colBuf.AddColor(x, y, partBuf[i]->GetPixel(x, y)/(double)nCores);
}

void LightTracer::SetSPP(int spp)
{
    m_SPP = spp;
}*/

unsigned int LightTracer::GetType() const
{
    return typeLightTracer;
}