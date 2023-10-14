/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file BDPT.cpp
 * 
 * Implementation of the BDPT class which renders using bidirectional path tracing.
 */

#define NOMINMAX
#include "BDPT.h"
#include <vector>
#include "Primitive.h"
#include "Material.h"
#include "Utils.h"
#include "Main.h"

const int N = 20; // Number of samples per ray to be saved for optimized russian roulette

/**
 * Constructor for the BDPSample class which is just a pair of numbers holding the number of light
 * path and eye path vertices of a given sample path.
 * 
 * @param s The number of light path vertices.
 * @param t The number of eye path vertices.
 */
BDSample::BDSample(int s, int t) : s(s), t(t)
{
}

/**
 * Constructor of the BDPT renderer.
 * 
 * @param scene The scene to render.
 */
BDPT::BDPT(std::shared_ptr<Scene> scene) : Renderer(scene)
{
    roulette = new Roulette[XRES*YRES];
}

/**
 * Constructor of the efficiency-optimized Russian roulette threshold provider.
 */
Roulette::Roulette() : sum(0), sumSq(0), sumRays(0)
{
    m = new std::mutex();
}

/**
 * Destructor.
 */
Roulette::~Roulette()
{
    delete m;
}

/**
 * Adds a sample to calibrate the efficiency-optimized Russian roulette.
 * 
 * @param sample The estimator of the sample.
 * @param nrays The number of rays used to construct the sample.
 */
void Roulette::AddSample(double sample, int nrays)
{
    std::lock_guard<std::mutex> lock(*m);

    sum += sample;
    sumSq += sample*sample;
    sumRays += nrays;
    samples.push_back(sample);
    samplesSq.push_back(sample*sample);
    rays.push_back(nrays);

    if(samples.size() > N)
    {
        sum -= samples.front();
        sumSq -= samplesSq.front();
        sumRays -= rays.front();

        samples.pop_front();
        samplesSq.pop_front();
        rays.pop_front();
    }
}

/**
 * Returns the threshhold of efficiency-optimized Russian roulette.
 * 
 * @returns The threshhold of efficiency-optimized Russian roulette.
 */
double Roulette::GetThreshold() const
{
    std::lock_guard<std::mutex> lock(*m);
    auto size = samples.size();
    auto ret = std::sqrt((sumSq - (1/double(size))*sum*sum)/double(sumRays));
    return std::isfinite(ret) ? std::abs(ret) : 1;
}

/**
 * Constructs a light/eye path in the scene and calculates its partial estimates along the way
 * together with its pdf evaluations and handles the path extension logic, including Russian
 * roulette for path termination.
 * 
 * @param path The light/eye path to build.
 * @param samples A vector to push a (0, t) sample if a light was hit.
 * @param light The light that the light path is/was built from.
 * @param lightPath Specifies whether we are constructing the light path (as opposed to the eye path).
 * @returns The number of vertices on the light/eye portion of the path.
 */
int BDPT::BuildPath(std::vector<BDVertex*>& path, std::vector<BDSample>& samples, Light* light, bool lightPath)
{
    double rr = 0.7;

    while(path.size() < 3 || m_random.GetDouble(0.f, 1.f) < rr)
    {
        BDVertex* lastV = path.back();

        auto [t, hitPrimitive, hitLight] = scene->Intersect(lastV->out);
        if(t < 0)
            break;

        IntersectionInfo info;
        if(hitPrimitive)
            hitPrimitive->GenerateIntersectionInfo(lastV->out, info);
        else
            hitLight->GenerateIntersectionInfo(lastV->out, info);

        Vector3d v = (info.position - lastV->out.origin);
        double lSqr = v.Length2();
        v.Normalize();

        BDVertex* newV = new BDVertex();
        newV->info = info;
        newV->pdf = lastV->sample.pdf*(abs(info.geometricnormal*info.direction))/(lSqr);
        newV->alpha = lastV->alpha*lastV->sample.color;
        newV->sample = info.material->GetSample(info, m_random, lightPath);
        newV->out = newV->sample.outRay;
        newV->specular = newV->sample.specular;

        newV->rr = path.size() < 3 ? 1 : lastV->rr*rr;
        //rr = std::min(1.0, newV->alpha.GetLuminance()/roulette[x+y*XRES].GetThreshold());
        rr = 0.7;

        if(!lightPath)
        {
            if(!newV->alpha || hitLight && hitLight != light)
            {
                delete newV;
                return (int) path.size();
            }

            path.push_back(newV);

            if(hitLight == light)
            {   // Direct light hit
                lastV->rpdf = hitLight->Pdf(info, -v)*(abs(lastV->info.geometricnormal*v))/(lSqr);
                samples.push_back(BDSample(0, (int) path.size()));
                return (int) path.size() - 1;
            }
            else
                lastV->rpdf = newV->sample.rpdf*abs(lastV->info.geometricnormal*v)/(lSqr);
        }
        else
        {
            if(!newV->sample.color)
            {
                lastV->rpdf = newV->sample.rpdf*abs(lastV->info.geometricnormal*v)/(lSqr);
                path.push_back(newV);
                return (int) path.size();
            }
            lastV->rpdf = newV->sample.rpdf*abs(lastV->info.geometricnormal*v)/(lSqr);
            path.push_back(newV);
        }
    }
    return (int) path.size();
}

/**
 * Constructs an eye path, by sampling the light source and then the rest of the path by
 * calling BuildPath.
 * 
 * @param x The horizontal coordinate of the pixel being rendered.
 * @param y The vertical coordinate of the pixel being rendered.
 * @param path The eye path to build.
 * @param cam The camera the path extends from.
 * @param samples A vector to push a (0, t) sample if a light was hit.
 * @param light The light that the light path will be built from.
 * @returns The number of vertices on the path.
 */
int BDPT::BuildEyePath(int x, int y, std::vector<BDVertex*>& path, 
                       const Camera& cam, std::vector<BDSample>& samples, 
                       Light* light)
{
    BDVertex* camPoint = new BDVertex();
    camPoint->camU = m_random.GetDouble(0, 1), camPoint->camV = m_random.GetDouble(0, 1);
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
    return BuildPath(path, samples, light, false);
}

/**
 * Constructs an eye path, by sampling the camera and then the rest of the path it by calling
 * BuildPath.
 * 
 * @param path The light path to build.
 * @param light A pointer to the light the path extends from.
 * @returns The number of vertices on the path.
 */
int BDPT::BuildLightPath(std::vector<BDVertex*>& path, Light* light)
{
    BDVertex* lightPoint = new BDVertex();
    auto [ray, color, normal, areaPdf, anglePdf] = light->SampleRay(m_random);
    lightPoint->out = ray;
    lightPoint->pdf = areaPdf;

    lightPoint->alpha = light->GetArea()*light->GetIntensity();
    lightPoint->rr = 1;
    lightPoint->info.normal = lightPoint->info.geometricnormal = normal;
    lightPoint->info.position = lightPoint->out.origin;

    lightPoint->sample = Sample(color, lightPoint->out, anglePdf, 0, false, 0);
    path.push_back(lightPoint);
    std::vector<BDSample> dummy;

    return BuildPath(path, dummy, light, true);
}

/**
 * Returns the Monte-Carlo estimator of a specific connection of prefixes of the light and eye
 * paths.
 * 
 * @param lightPath All the vertices of the light path.
 * @param eyePath All the vertices of the eye path.
 * @param s The number of vertices from lightPath used to construct the path.
 * @param t The number of vertices from eyePath used to construct the path.
 * @param light A pointer to the light used.
 * @returns The estimated value of the path.
 */
Color BDPT::EvalPath(const std::vector<BDVertex*>& lightPath, const std::vector<BDVertex*>& eyePath,
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
    double r = c.direction.Length();
    c.direction.Normalize();

    result *= abs(lastL->info.geometricnormal*c.direction)*abs(lastE->info.normal*c.direction)/(r*r);
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

/**
 * Calculates the weight of a given construction of a path using equal weighting.
 * 
 * @param s The number of vertices from lightPath used to construct the path.
 * @param t The number of vertices from eyePath used to construct the path.
 * @param lightPath All the vertices of the light path.
 * @param eyePath All the vertices of the eye path.
 * @param light A pointer to the light used.
 * @param cam A pointer to the camera used.
 * @returns The weight of the path.
 */
double BDPT::UniformWeight(int s, int t, std::vector<BDVertex*>& lightPath,
                      std::vector<BDVertex*>& eyePath, Light*, Camera*) const
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

/**
 * Calculates the weight of a given construction of a path, using the power heuristic.
 * 
 * @param s The number of vertices from lightPath used to construct the path.
 * @param t The number of vertices from eyePath used to construct the path.
 * @param lightPath All the vertices of the light path.
 * @param eyePath All the vertices of the eye path.
 * @param light A pointer to the light used.
 * @param cam A pointer to the camera used.
 * @returns The weight of the path.
 */
double BDPT::PowerHeuristic(int s, int t, std::vector<BDVertex*>& lightPath,
                           std::vector<BDVertex*>& eyePath, Light* light, Camera* cam) const
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
        Vector3d out = lastE->info.position - lastL->info.position;
        double lSqr = out.Length2();
        out.Normalize();
        double newPdf;
        if(s > 1)
            newPdf = lastL->info.material->PDF(info, out, true, lastL->sample.component);
        else
            newPdf = light->Pdf(lastL->info, out);
        forwardProbs[s] = newPdf*abs(lastE->info.geometricnormal*out)/(lSqr);

        if(t > 2)
        {
            info = eyePath[t-1]->info;
            info.direction = out;
            out = eyePath[t-2]->info.position - lastE->info.position;
            lSqr = out.Length2();
            out.Normalize();
            newPdf = lastE->info.material->PDF(info, out, true, lastE->sample.component);
        
            forwardProbs[s+1] = newPdf*abs(eyePath[t-2]->info.geometricnormal*out)/(lSqr);
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
        Vector3d out = lastL->info.position-lastE->info.position;
        double lSqr = out.Length2();
        out.Normalize();
        double newPdf;
        double costheta = abs(lastE->info.geometricnormal*out);
        if(t == 1)
            newPdf = 1/(cam->GetFilmArea()*costheta*costheta*costheta);
        else
            newPdf = lastE->info.material->PDF(info, out, false, lastE->sample.component);
        backwardProbs[s-1] = newPdf*abs(lastL->info.geometricnormal*out)/lSqr;
        if(s > 1)
        {
            info = lightPath[s-1]->info;
            info.direction = out;
            out = lightPath[s-2]->info.position - lastL->info.position;
            lSqr = out.Length2();
            out.Normalize();
            newPdf = lastL->info.material->PDF(info, out, false, lastL->sample.component);

            backwardProbs[s-2] = newPdf*abs(lightPath[s-2]->info.geometricnormal*out)/(lSqr);
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

    return 1.0/(1.0+weight);
}

/**
 * Returns the weight of a particular eye path/light path decomposition of a given path. The sum
 * of all the possible decompositions of a specific path should equal 1. Currently uses the power
 * heuristic to calculate the weight.
 * 
 * @param s The number of vertices from lightPath used to construct the path.
 * @param t The number of vertices from eyePath used to construct the path.
 * @param lightPath All the vertices of the light path.
 * @param eyePath All the vertices of the eye path.
 * @param light A pointer to the light used.
 * @param cam A pointer to the camera used.
 * @returns The weight of the path.
 */
double BDPT::WeighPath(int s, int t, std::vector<BDVertex*>& lightPath,
                      std::vector<BDVertex*>& eyePath, Light* light, Camera* camera) const
{
    return PowerHeuristic(s, t, lightPath, eyePath, light, camera);
}

/**
 * Renders a single pixel by building a light path, an eye path, connecting them together in every
 * possible way and weighing each estimate using the power heuristic.
 * 
 * @param x The x coordinate of the pixel.
 * @param y The y coordinate of the pixel.
 * @param cam The camera used to capture the scene.
 * @param eyeImage The color buffer containing the evaluations of paths containing two or more
 *                 eye vertices.
 * @param lightImage The color buffer that holds the evaluations of paths with a single eye vertex.
 */
void BDPT::RenderPixel(int x, int y, Camera& cam, 
                       ColorBuffer& eyeImage, ColorBuffer& lightImage)
{
    std::vector<BDSample> samples;
    std::vector<BDVertex*> eyePath, lightPath;

    auto [light, lightWeight] = scene->PickLight(m_random.GetDouble(0.0, 1.0));

    int lLength = BuildLightPath(lightPath, light);
    int eLength = BuildEyePath(x, y, eyePath, cam, samples, light);

    int rays = lLength + eLength;

    for(int s = 1; s <= lLength; s++)
        for(int t = 1; t <= eLength; t++)
            samples.push_back(BDSample(s, t));

    for(auto sample : samples)
    {
        Color eval = EvalPath(lightPath, eyePath, sample.s, sample.t, light);
        if(!eval)
            continue;

        double weight = WeighPath(sample.s, sample.t, lightPath, eyePath, light, &cam);
        eval *= weight;

        int s = sample.s, t = sample.t;
        // Build the connecting vertex
        if(s > 0)
        {
            BDVertex* lastL = lightPath[s-1];
            BDVertex* lastE = eyePath[t-1];

            Ray c = Ray(lastE->out.origin, lastL->out.origin - lastE->out.origin);
            double r = c.direction.Length();
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
            Ray camRay(lightPath[sample.s-1]->out.origin, 
                       cam.pos - lightPath[sample.s-1]->out.origin);
            camRay.direction.Normalize();

            auto [hitCam, camx, camy] = cam.GetPixelFromRay(camRay, eyePath[0]->camU, eyePath[0]->camV);
            if(!hitCam)
                continue;
            double costheta = abs(cam.dir*camRay.direction);
            double mod = costheta*costheta*costheta*costheta*cam.GetFilmArea();
            Color result = eval/mod/lightWeight;
            if(result.IsValid())
                lightImage.AddColor(camx, camy, result);
        }
        else
        {
            double costheta = abs(cam.dir*eyePath[0]->out.direction);
            double mod = costheta*costheta*costheta*costheta*(cam.GetFilmArea());
            Color result = eval/mod/lightWeight;
            if(result.IsValid())
                eyeImage.AddColor(x, y, result);
        }
    }

    roulette[x+y*XRES].AddSample((eyeImage.GetPixel(x, y) + lightImage.GetPixel(x, y)).GetLuma(), rays);

    for(unsigned int s = 1; s < lightPath.size() + 1; s++)
        delete lightPath[s-1];
    for(unsigned int t = 1; t < eyePath.size() + 1; t++)
        delete eyePath[t-1];
}

/**
 * Renders the scene to a color buffer.
 * 
 * @param cam The camera used to capture the scene.
 * @param colBuf The color buffer to render to.
 */
void BDPT::Render(Camera& cam, ColorBuffer& colBuf)
{
    ColorBuffer lightImage(colBuf.GetXRes(), colBuf.GetYRes(), Color::Black);
    ColorBuffer eyeImage(colBuf.GetXRes(), colBuf.GetYRes(), Color::Black);

    for(int x = 0; x < colBuf.GetXRes(); x++)
        for(int y = 0; y < colBuf.GetYRes() && !stopping; y++)
            RenderPixel(x, y, cam, eyeImage, lightImage);

    for(int x = 0; x < colBuf.GetXRes(); x++)
        for(int y = 0; y < colBuf.GetYRes(); y++)
            colBuf.AddColor(x, y, eyeImage.GetPixel(x, y) + lightImage.GetPixel(x, y));
}

/**
 * Saves information about the renderer to a bytestream.
 * 
 * @param stream The bytestream to stream to.
 */
void BDPT::Save(Bytestream& stream) const
{
    stream << ID_BDPT;
}

/**
 * Loads the renderer from a bytestream.
 * 
 * @param stream The bytestream to stream from.
 */
void BDPT::Load(Bytestream& stream)
{
}
