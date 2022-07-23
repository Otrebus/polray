#pragma once

#ifndef BDPT_H
#define BDPT_H

#include "Renderer.h"
#include "Random.h"
#include "IntersectionInfo.h"
#include "Sample.h"
#include <mutex>

#define WEIGHT_UNIFORM 1
#define WEIGHT_POWER 2

class Ray;
class Primitive;

class Roulette {
public:
    Roulette();
    ~Roulette();
    void AddSample(double sample, int rays);
    double GetThreshold() const;
    
private:
    std::mutex* m;
    std::list<double> samples;
    std::list<double> samplesSq;
    std::list<int> rays;

    double sum, sumSq;
    int sumRays;
};

class BDSample
{
public:
    ~BDSample();
    friend class BDPT;
private:
    BDSample(int s, int t);
    int s, t;
};

class BDVertex
{
public:
    friend class BDPT;
private:
    Ray out;
    double rr; // The russian roulette factor
    double pdf; // The area pdf of this vertex as if sampled from the previous
    double rpdf; // The area pdf of this vertex as if sampled from the next
    Color alpha; // The unweighted contribution/measurement (cos*brdf/pdf)
    Sample sample; // The material sample
    IntersectionInfo info;
    bool specular;
    double camU, camV; // Only used by the eye point
};

class BDPT : public Renderer
{
public:
    BDPT(std::shared_ptr<Scene> scene);
    virtual ~BDPT();

    void Render(Camera& cam, ColorBuffer& colBuf);

    void SetSPP(unsigned int spp);
    unsigned int GetSPP() const;

    unsigned int GetType() const;

protected:
    void RenderPixel(int x, int y, Camera& cam,
                     ColorBuffer& eyeImage, ColorBuffer& lightImage);
    void RenderPart(Camera& cam, ColorBuffer& colBuf) const;

    int BuildPath(int x, int y, std::vector<BDVertex*>& path, std::vector<BDSample>& samples, Light* light, bool lightPath) const;

    int BuildEyePath(int x, int y, vector<BDVertex*>& path, const Camera& cam,
                     vector<BDSample>& samples, Light* light) const;
    int BuildLightPath(int x, int y, vector<BDVertex*>& path, Light* light) const;

    Color EvalPath(vector<BDVertex*>& lightPath, vector<BDVertex*>& eyePath, 
                   int s, int t, Light* light) const;
    double WeighPath(int s, int t, vector<BDVertex*>& lightPath, 
                    vector<BDVertex*>& eyePath, Light* light, Camera* camera) const;

    double UniformWeight(int s, int t, vector<BDVertex*>& lightPath,
                      vector<BDVertex*>& eyePath, Light* light, Camera* camera) const;
    double PowerHeuristic(int s, int t, vector<BDVertex*>& lightPath,
                      vector<BDVertex*>& eyePath, Light* light, Camera* camera) const;

    Roulette* roulette;
    
    mutable Random m_random;
};

#endif