#define NOMINMAX
#include "BDPT.h"
#include <vector>
#include "AreaLight.h"
#include "Primitive.h"
#include "Material.h"
#include "Utils.h"

const int N = 20; // Number of samples per ray to be saved for optimized russian roulette

BDSample::BDSample(int a, int b) : s(a), t(b)
{
}

BDSample::~BDSample()
{
}

BDPT::BDPT(std::shared_ptr<Scene> scene) : Renderer(scene)
{
    roulette = new Roulette[XRES*YRES];
}

BDPT::~BDPT()
{
}

Roulette::Roulette() : sum(0), sumSq(0), sumRays(0) {
    m = new std::mutex();
}

Roulette::~Roulette() {
    delete m;
}

void Roulette::AddSample(double sample, int nrays) {
    std::lock_guard<std::mutex> lock(*m);

    sum += sample;
    sumSq += sample*sample;
    sumRays += nrays;
    samples.push_back(sample);
    samplesSq.push_back(sample*sample);
    rays.push_back(nrays);

    if(samples.size() > N) {
        sum -= samples.front();
        sumSq -= samplesSq.front();
        sumRays -= rays.front();

        samples.pop_front();
        samplesSq.pop_front();
        rays.pop_front();
    }
}

double Roulette::GetThreshold() const {
    std::lock_guard<std::mutex> lock(*m);
    int size = samples.size();
    auto ret = std::sqrt((sumSq - (1/double(size))*sum*sum)/double(sumRays));
    return std::isfinite(ret) ? std::abs(ret) : 1;
}

int BDPT::BuildPath(int x, int y, std::vector<BDVertex*>& path, std::vector<BDSample>& samples, Light* light, bool lightPath) const {

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
        newV->specular = newV->sample.specular;

        newV->rr = path.size() < 3 ? 1 : lastV->rr*rr;
        //rr = std::min(1.0, newV->alpha.GetLuminance()/roulette[x+y*XRES].GetThreshold());
        rr = 0.7;

        if(!lightPath) {
            if(!newV->alpha && hitLight != light)
            {
                delete newV;
                return path.size();
            }

            lastV->rpdf = newV->sample.rpdf*abs(lastV->info.GetGeometricNormal()*v)/(lSqr);
            path.push_back(newV);

            if(hitLight == light)
            {   // Direct light hit
                lastV->rpdf = hitLight->Pdf(info, -v)*(abs(lastV->info.GetGeometricNormal()*v))/(lSqr);
                samples.push_back(BDSample(0, path.size()));
                return path.size() - 1;
            }
        } else {
            if(!newV->sample.color)
            {
                lastV->rpdf = newV->sample.rpdf*abs(lastV->info.GetGeometricNormal()*v)/(lSqr);
                path.push_back(newV);
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
    camPoint->rpdf = 1;
    double costheta = abs(camPoint->out.direction*cam.dir);
    double lastPdf = 1/(cam.GetFilmArea()*costheta*costheta*costheta);
    camPoint->pdf = 1/(cam.GetFilmArea());
    Color lastSample = costheta*Color::Identity/lastPdf;

    camPoint->sample = Sample(lastSample, camPoint->out, lastPdf, camPoint->rpdf, false, 0);

    path.push_back(camPoint);
    return BuildPath(x, y, path, samples, light, false);
}

int BDPT::BuildLightPath(int x, int y, vector<BDVertex*>& path, Light* light) const
{
    int depth = 1;
    BDVertex* lightPoint = new BDVertex();
    double rr = 0.7f;
    double lastPdf;
    auto [ray, color, normal, areaPdf, anglePdf] = light->SampleRay();
    lightPoint->out = ray;
    lightPoint->pdf = areaPdf;

    lightPoint->alpha = light->GetArea()*light->GetIntensity();
    lightPoint->rr = 1;
    lightPoint->info.normal = lightPoint->info.geometricnormal = normal;
    lightPoint->info.position = lightPoint->out.origin;

    lightPoint->sample = Sample(color, lightPoint->out, anglePdf, 0, false, 0);
    path.push_back(lightPoint);
    std::vector<BDSample> dummy;

    return BuildPath(x, y, path, dummy, light, true);
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

    result *= abs(lastL->info.GetGeometricNormal()*c.direction)*abs(lastE->info.GetNormal()*c.direction)/(r*r);
    result *= lastL->alpha*lastE->alpha;

    // This BRDF is backwards so let's modify it 
    double modifier = c.direction*lastL->info.geometricnormal > 0 ? abs(c.direction*lastL->info.normal)/abs(c.direction*lastL->info.geometricnormal) : 1;

    if(s > 1)
        result *= modifier*lastL->info.material->
                  BRDF(lastL->info, -c.direction, lastL->sample.component)
                  /lastL->rr;
    if(t > 1)
        result *= lastE->info.material->
                  BRDF(lastE->info, c.direction, lastE->sample.component)
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
     
    // The last forward pdfs are the backward pdfs of the eye path
    for(int i = s+1; i < s+t; i++)
        forwardProbs[i] = eyePath[s+t-i-1]->rpdf;

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
            newPdf = lastL->info.GetMaterial()->PDF(info, out, true, lastL->sample.component);
        else
            newPdf = light->Pdf(lastL->info, out);
        forwardProbs[s] = newPdf*abs(lastE->info.geometricnormal*out)/(lSqr);

        if(t > 2) {
            auto info = eyePath[t-1]->info;
            info.direction = out;
            auto out2 = eyePath[t-2]->info.position - lastE->info.position;
            lSqr = out2.GetLengthSquared();
            out2.Normalize();
            newPdf = lastE->info.GetMaterial()->PDF(info, out2, true, lastE->sample.component);
        
            forwardProbs[s+1] = newPdf*abs(eyePath[t-2]->info.geometricnormal*out2)/(lSqr);
        }
    }

    // Next, calculate all the backward (eye to light) going pdf values
    // The backwards pdf of the light path are readily available
    for(int i = 0; i < s-1; i++)
        backwardProbs[i] = lightPath[i]->rpdf;

    // The last backward pdf values are just the forward pdf values of eye path
    for(int i = s; i < s+t; i++)
        backwardProbs[i] = eyePath[s+t-i-1]->pdf;

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
            newPdf = lastE->info.GetMaterial()->PDF(info, out, false, lastE->sample.component);
        backwardProbs[s-1] = newPdf*abs(lastL->info.geometricnormal*out)/lSqr;
        if(s > 1) {
            auto info = lightPath[s-1]->info;
            info.direction = out;
            auto out2 = lightPath[s-2]->info.position - lastL->info.position;
            lSqr = out2.GetLengthSquared();
            out2.Normalize();
            newPdf = lastL->info.GetMaterial()->PDF(info, out2, false, lastL->sample.component);

            backwardProbs[s-2] = newPdf*abs(lightPath[s-2]->info.geometricnormal*out2)/(lSqr);
        }
    }
    
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

    if(!std::isfinite(weight)) // In case we ever get a stray 0/0 for whatever reason
        return 0;

    return 1.0f/(1.0f+weight);
}

double BDPT::WeighPath(int s, int t, vector<BDVertex*>& lightPath,
                      vector<BDVertex*>& eyePath, Light* light, Camera* camera) const
{
    return PowerHeuristic(s, t, lightPath, eyePath, light, camera);
}

void BDPT::RenderPixel(int x, int y, Camera& cam, 
                       ColorBuffer& eyeImage, ColorBuffer& lightImage)
{
    vector<BDSample> samples;
    vector<BDVertex*> eyePath;
    vector<BDVertex*> lightPath;

    double r = m_random.GetDouble(0.0f, 1.0f);
    auto [light, lightWeight] = scene->PickLight(r);

    int lLength = BuildLightPath(x, y, lightPath, light);
    int eLength = BuildEyePath(x, y, eyePath, cam, samples, light);

    int rays = lLength + eLength;

    for(int s = 1; s <= lLength; s++)
        for(int t = 1; t <= eLength; t++)
            samples.push_back(BDSample(s, t));

    for(auto sample : samples)
    {
        Color eval = EvalPath(lightPath, eyePath, sample.s, sample.t, light);
        // TODO: break early if zero eval

        double weight = WeighPath(sample.s, sample.t, lightPath, eyePath, light, &cam);
        eval *= weight;

        int s = sample.s, t = sample.t;
        // Build the connecting vertex
        if(s > 0) {
            BDVertex* lastL = lightPath[s-1];
            BDVertex* lastE = eyePath[t-1];

            Ray c = Ray(lastE->out.origin, lastL->out.origin - lastE->out.origin);
            double r = c.direction.GetLength();
            c.direction.Normalize();

            /*auto q = std::min(1.0, eval.GetLuminance()/roulette[x+y*XRES].GetThreshold());

            if(m_random.GetDouble(0, 1) > q)
                continue;
            eval /= q;*/

            if(!TraceShadowRay(c, (1-eps)*r) || r < eps)
                continue;
        }

        rays += 1;

        if(sample.t == 1) // These samples end up on the light image
        {
            int camx, camy;
            Ray camRay(lightPath[sample.s-1]->out.origin, 
                       cam.pos - lightPath[sample.s-1]->out.origin);
            camRay.direction.Normalize();
            if(!cam.GetPixelFromRay(camRay, camx, camy, eyePath[0]->camU, eyePath[0]->camV))
                continue;
            double costheta = abs(cam.dir*camRay.direction);
            double mod = costheta*costheta*costheta*costheta*cam.GetFilmArea();
            Color result = eval/mod/lightWeight;
            lightImage.AddColor(camx, camy, result);
        }
        else
        {
            double costheta = abs(cam.dir*eyePath[0]->out.direction);
            double mod = costheta*costheta*costheta*costheta*(cam.GetFilmArea());
            Color result = eval/mod/lightWeight;
            eyeImage.AddColor(x, y, result);
        }
    }

    roulette[x+y*XRES].AddSample((eyeImage.GetPixel(x, y) + lightImage.GetPixel(x, y)).GetLuminance(), rays);

    for(unsigned int s = 1; s < lightPath.size() + 1; s++)
        delete lightPath[s-1];
    for(unsigned int t = 1; t < eyePath.size() + 1; t++)
        delete eyePath[t-1];
}

void BDPT::Render(Camera& cam, ColorBuffer& colBuf)	
{	
    ColorBuffer lightImage(colBuf.GetXRes(), colBuf.GetYRes(), 0);
    ColorBuffer eyeImage(colBuf.GetXRes(), colBuf.GetYRes(), 0);

    for(int x = 0; x < colBuf.GetXRes(); x++)	
        for(int y = 0; y < colBuf.GetYRes() && !stopping; y++)	
            RenderPixel(x, y, cam, eyeImage, lightImage);	

    for(int x = 0; x < colBuf.GetXRes(); x++)	
        for(int y = 0; y < colBuf.GetYRes(); y++)	
            colBuf.AddColor(x, y, eyeImage.GetPixel(x, y)+ lightImage.GetPixel(x, y));
}

unsigned int BDPT::GetType() const
{
    return typeBDPT;
}