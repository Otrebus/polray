#include "BDPT.h"
#include <vector>
#include "AreaLight.h"
#include "Primitive.h"
#include "Material.h"
#include "Utils.h"

BDSample::BDSample(int a, int b) : s(a), t(b)
{
}

BDSample::~BDSample()
{
}

BDPT::BDPT(std::shared_ptr<Scene> scene) : Renderer(scene)
{
    m_spp = 1;
}

BDPT::~BDPT()
{
}

int BDPT::BuildPath(std::vector<BDVertex*>& path, std::vector<BDSample>& samples, Light* light, bool lightPath) const {

    double rr = 0.7;

    while(path.size() < 3 || m_random.GetDouble(0.f, 1.f) < rr) {
        BDVertex* lastV = path.back();
        double lastPdf = lastV->sample.pdf;
        auto lastSample = lastV->sample.color;

        const Primitive* hitPrimitive;
        const Light* hitLight;
        auto t = scene->Intersect(lastV->out, hitPrimitive, hitLight);
        if(t < 0)
            break;

        IntersectionInfo info;
        if(hitPrimitive)
            hitPrimitive->GenerateIntersectionInfo(lastV->out, info);
        else
            hitLight->GenerateIntersectionInfo(lastV->out, info);

        BDVertex* newV = new BDVertex();
        newV->info = info;
        Vector3d v = (info.GetPosition() - lastV->out.origin);
        double lSqr = v.GetLengthSquared();
        v.Normalize();
        newV->pdf = lastPdf*(abs(info.GetGeometricNormal()*info.GetDirection()))/(lSqr);
        newV->alpha = lastV->alpha*lastSample;
        Material* mat = info.GetMaterial();

        newV->sample = mat->GetSample(info, lightPath);
        newV->out = newV->sample.outRay;
        newV->rpdf = newV->sample.rpdf;
        newV->specular = newV->sample.specular;
        newV->rr = path.size() < 3 ? 1 : lastV->rr*rr;
        rr = min(lastSample.GetLuminance(), 1);

        if(!lightPath) {
            if(newV->alpha.GetLuminance() <= 0 && info.GetMaterial()->GetLight() != light)
            {
                delete newV;
                return path.size();
            }

            lastV->rpdf = newV->sample.rpdf*abs(lastV->info.GetGeometricNormal()*v)/(lSqr);
            path.push_back(newV);

            if(info.material->GetLight() == light)
            {   // Direct light hit
                samples.push_back(BDSample(0, path.size()));
                return path.size() - 1;
            }
        } else {
            if(lastSample.GetLuminance() <= 0)
            {
                delete newV;
                return path.size();
            }
            lastV->rpdf = newV->sample.rpdf*abs(lastV->info.GetGeometricNormal()*v)/(lSqr);
            path.push_back(newV);
        }
    }
    return path.size();
}

int BDPT::BuildEyePath(int x, int y, vector<BDVertex*>& path, 
                       const Camera& cam, vector<BDSample>& samples, 
                       Light* light) const
{
    BDVertex* camPoint = new BDVertex();
    Vector3d lensPoint;
    cam.SampleAperture(lensPoint, camPoint->camU, camPoint->camV);
    camPoint->out = cam.GetRayFromPixel(x, y, m_random.GetDouble(0, 1), 
                                        m_random.GetDouble(0, 1), camPoint->camU,
                                        camPoint->camV);
    camPoint->rr = 1;
    camPoint->alpha = Color::Identity;
    camPoint->info.normal = camPoint->info.geometricnormal = cam.dir;
    camPoint->info.position = camPoint->out.origin;
    camPoint->specular = true;
    camPoint->rpdf = 1;
    double costheta = abs(camPoint->out.direction*cam.dir);
    double lastPdf = 1/(cam.GetFilmArea()*costheta*costheta*costheta);
    camPoint->pdf = 1/(cam.GetFilmArea());
    Color lastSample = costheta*Color::Identity/lastPdf;

    camPoint->sample = Sample(lastSample, camPoint->out, lastPdf, camPoint->rpdf, false);

    path.push_back(camPoint);
    return BuildPath(path, samples, light, false);
}

int BDPT::BuildLightPath(vector<BDVertex*>& path, Light* light) const
{
    int depth = 1;
    BDVertex* lightPoint = new BDVertex();
    double rr = 0.7f;
    double lastPdf;
    Color lastSample = light->SampleRay(lightPoint->out, lightPoint->info.normal, lightPoint->pdf, lastPdf);
    lightPoint->alpha = light->GetArea()*light->GetIntensity();
    lightPoint->rr = 1;
    lightPoint->info.normal = lightPoint->info.normal;
    lightPoint->info.geometricnormal = lightPoint->info.normal;
    lightPoint->info.position = lightPoint->out.origin;

    lightPoint->sample = Sample(lastSample, lightPoint->out, lastPdf, lightPoint->rpdf, false);
    path.push_back(lightPoint);
    std::vector<BDSample> dummy;

    return BuildPath(path, dummy, light, true);
}

Color BDPT::EvalPath(vector<BDVertex*>& lightPath, vector<BDVertex*>& eyePath,
                     int s, int t, Light* light) const
{
    Color result(1, 1, 1);

    if(t == 0) // 0 eye path vertices not possible since cam is not part of scene
        return Color(0, 0, 0);

    if(s == 0) // 0 light path vertices, we directly hit the light source
    {          // - has to be handled a little bit differently
        BDVertex* prevE = eyePath[t-1];
        Vector3d lightNormal = eyePath[t-1]->info.normal;
        if(t == 1) // Direct light hit
        {
            if(eyePath[0]->out.direction*lightNormal < 0) 
                return light->GetIntensity();
            else 
                return Color(0, 0, 0);
        }
        BDVertex* lastE = eyePath[t-2]; 
		if(lastE->out.direction*lightNormal < 0)
            return light->GetIntensity()*prevE->alpha/prevE->rr;
		else 
            return Color(0, 0, 0);
    }

    // Build the connecting vertex
    BDVertex* lastL = lightPath[s-1];
    BDVertex* lastE = eyePath[t-1];

    Ray c = Ray(lastE->out.origin, lastL->out.origin - lastE->out.origin);
    double r = c.direction.GetLength();
    c.direction.Normalize();

    if(!TraceShadowRay(c, (1-eps)*r) || r < eps)
        return Color(0, 0, 0);

    result*= abs(lastL->info.GetGeometricNormal()*c.direction)
            *abs(lastE->info.GetNormal()*c.direction)/(r*r);
    result *= lastL->alpha*lastE->alpha;

    // This BRDF is backwards so let's modify it 
    double modifier = abs(lastL->info.direction*lastL->info.normal)
                    /abs(lastL->info.direction*lastL->info.geometricnormal);

    if(s > 1)
        result *= modifier*lastL->info.material->
                  BRDF(lastL->info, -c.direction)
                  /lastL->rr;
    if(t > 1)
        result *= lastE->info.material->
                  BRDF(lastE->info, c.direction)
                  /lastE->rr;

    return result;
}

double BDPT::UniformWeight(int s, int t, vector<BDVertex*>& lightPath,
                      vector<BDVertex*>& eyePath, Light*, Camera*) const
{
    double weight = double(s+t);
    bool wasSpec = false;
    for(auto it = lightPath.cbegin() + 1; it < lightPath.cbegin() + s; it++)
    {
        BDVertex* v = *it;
        if(v->specular)
        {
            weight -= 1;
            wasSpec = true;
        }
        else if(wasSpec)
        {
            wasSpec = false;
            weight -= 1;
        }
    }
    for(auto it = eyePath.cbegin() + (t - 1); it > eyePath.cbegin(); it--)
    {
        BDVertex* v = *it;
        if(v->specular)
        {
            weight -= 1;
            wasSpec = true;
        }
        else if(wasSpec)
        {
            wasSpec = false;
            weight -= 1;
        }
    }
    if(wasSpec)
        weight -= 1;
    return 1/weight;
}

double BDPT::PowerHeuristic(int s, int t, vector<BDVertex*>& lightPath,
                           vector<BDVertex*>& eyePath, Light* light, Camera* cam) const
{
    double weight = 0;
    std::vector<double> forwardProbs(s+t);
    std::vector<double> backwardProbs(s+t);
    std::vector<bool> specular(s+t);

    for(int i = 0; i < s+t; i++)
        specular[i] = false;

    // Tag specular vertices
    for(int i = 1; i < s+t-1; i++)
    {
        BDVertex* v = i < s ? lightPath[i] : eyePath[s+t-i-1];
        if(v->specular)
            specular[i] = true;
    }

    // First off, calculate all the forward (light to eye) going pdf values
    forwardProbs[0] = 1/light->GetArea(); // (For direct light hit, s == 0)
    for(int i = 0; i < s; i++) // The first part is readily available
        forwardProbs[i] = lightPath[i]->pdf;

    // The forward pdf value of the connecting edge is calculated next
    // Basically, just project the angle pdf to area pdf at the next surface
    if(s > 0)
    {
        BDVertex* lastE = eyePath[t-1], *lastL = lightPath[s-1];
        IntersectionInfo info = lightPath[s-1]->info;
        Vector3d out = lastE->info.GetPosition() - lastL->info.GetPosition();
        double lSqr = out.GetLengthSquared();
        out.Normalize();
        double newPdf;
        if(s > 1)
            newPdf = lastL->info.GetMaterial()->
                     PDF(info, out, true);
        else
            newPdf = light->Pdf(lastL->info, out);
        forwardProbs[s] = newPdf*abs(lastE->info.geometricnormal*out)/(lSqr);
    }

    // The last forward pdfs are the backward pdfs of the eye path
    for(int i = s+1; i < s+t; i++)
        forwardProbs[i] = eyePath[s+t-i-1]->rpdf;

    // Next, calculate all the backward (eye to light) going pdf values
    // The backwards pdf of the light path are readily available
    for(int i = 0; i < s-1; i++)
        backwardProbs[i] = lightPath[i]->rpdf;

    // Calculate the backwards pdf value of the connecting edge
    if(s > 0)
    {
        BDVertex* lastE = eyePath[t-1], *lastL = lightPath[s-1];
        IntersectionInfo info = lastE->info;
        Vector3d out = lastL->info.GetPosition()-lastE->info.GetPosition();
        double lSqr = out.GetLengthSquared();
        out.Normalize();
        double newPdf;
        double costheta = abs(lastE->info.geometricnormal*out);
        if(t == 1)
            newPdf = 1/(cam->GetFilmArea()*costheta*costheta*costheta);
        else
            newPdf = lastE->info.GetMaterial()->PDF(info, out, false);
        backwardProbs[s-1] = newPdf*abs(lastL->info.geometricnormal*out)/lSqr;
    }

    // The last backward pdf values are just the forward pdf values of eye path
    for(int i = s; i < s+t; i++)
        backwardProbs[i] = eyePath[s+t-i-1]->pdf;
    
    // Sum the actual weights of the paths together
    double l = 1;
    for(int i = s; i < s+t-1; i++)
    {
        l *= forwardProbs[i]/backwardProbs[i];
        if(!(specular[i] || specular[i+1]))
            weight += l*l;
    }
    l = 1;
    for(int i = s-1; i >= 0; i--)
    {
        l*= backwardProbs[i]/forwardProbs[i];
        if(!(specular[i] || (i > 0 && specular[i-1])))
            weight += l*l;
    }

    if(weight!=weight) // In case we ever get a stray 0/0 for whatever reason
        return 0;

    return 1.0f/(1.0f+weight);
}

double BDPT::WeighPath(int s, int t, vector<BDVertex*>& lightPath,
                      vector<BDVertex*>& eyePath, Light* light, Camera* camera) const
{
    return PowerHeuristic(s, t, lightPath, eyePath, light, camera);
}

void BDPT::RenderPixel(int x, int y, Camera& cam, 
                       ColorBuffer& eyeImage, ColorBuffer& lightImage) const
{
    vector<BDSample> samples;
    vector<BDVertex*> eyePath;
    vector<BDVertex*> lightPath;

    double lightWeight;
    double r = m_random.GetDouble(0.0f, 1.0f);
    Light* light = lightTree->PickLight(r, lightWeight);

    int lLength = BuildLightPath(lightPath, light);
    int eLength = BuildEyePath(x, y, eyePath, cam, samples, light);

    for(int s = 1; s <= lLength; s++)
        for(int t = 1; t <= eLength; t++)
            samples.push_back(BDSample(s, t));

    for(auto it = samples.cbegin(); it < samples.end(); it++)
    {
        BDSample sample = *it;
        const int xres = lightImage.GetXRes(), yres = lightImage.GetYRes();
        double weight = WeighPath(sample.s, sample.t, lightPath, eyePath, light, &cam);
        Color eval = EvalPath(lightPath, eyePath, sample.s, sample.t, light);

        if(sample.t == 1) // These samples end up on the light image
        {
            int camx, camy;
            Ray camRay(lightPath[sample.s-1]->out.origin, 
                       cam.pos - lightPath[sample.s-1]->out.origin);
            camRay.direction.Normalize();
            if(!cam.GetPixelFromRay(camRay, camx, camy, 
                                    eyePath[0]->camU, eyePath[0]->camV))
                continue;
            double costheta = abs(cam.dir*camRay.direction);
            double mod = costheta*costheta*costheta*costheta*cam.GetFilmArea()*lightWeight;
            Color result = weight*eval/mod;
            lightImage.AddColor(camx, camy, result);
        }
        else
        {
            double costheta = abs(cam.dir*eyePath[0]->out.direction);
            double mod = costheta*costheta*costheta*costheta*(cam.GetFilmArea())*lightWeight;
            Color result = weight*eval/mod;
            eyeImage.AddColor(x, y, result);
        }
    }
    for(unsigned int s = 1; s < lightPath.size() + 1; s++)
        delete lightPath[s-1];
    for(unsigned int t = 1; t < eyePath.size() + 1; t++)
        delete eyePath[t-1];
}

void BDPT::Render(Camera& cam, ColorBuffer& colBuf)	
{	
    ColorBuffer lightImage = colBuf;	
    ColorBuffer eyeImage = colBuf;	
    lightImage.Clear(0);	
    eyeImage.Clear(0);	
    for(int i = 0; i < m_spp; i++)	
    {	
        for(int x = 0; x < colBuf.GetXRes(); x++)	
        {	
            for(int y = 0; y < colBuf.GetYRes(); y++)	
            {	
                if(stopping)	
                    return;	
                RenderPixel(x, y, cam, eyeImage, lightImage);	
            }	
        }	
    }	
    for(int x = 0; x < colBuf.GetXRes(); x++)	
        for(int y = 0; y < colBuf.GetYRes(); y++)	
            colBuf.AddColor(x, y, (eyeImage.GetPixel(x, y)	
                                   + lightImage.GetPixel(x, y))/m_spp);	
}

void BDPT::SetSPP(unsigned int spp)
{
    m_spp = spp;
}

unsigned int BDPT::GetSPP() const
{
    return m_spp;
}

unsigned int BDPT::GetType() const
{
    return typeBDPT;
}