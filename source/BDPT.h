#pragma once

#ifndef BDPT_H
#define BDPT_H

#include "Renderer.h"
#include "Random.h"
#include "IntersectionInfo.h"
#include "LightTree.h"
#include "Sample.h"

#define WEIGHT_UNIFORM 1
#define WEIGHT_POWER 2

class Ray;
class Primitive;

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
    float rr; // The russian roulette factor
    float pdf; // The forward area pdf
    float rpdf; // The reverse area pdf
    Color alpha; // The unweighted contribution/measurement (cos*brdf/pdf)
    Sample sample; // The material sample
    IntersectionInfo info;
    bool specular;
    float camU, camV; // Only used by the eye point
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
                     ColorBuffer& eyeImage, ColorBuffer& lightImage) const;
    void RenderPart(Camera& cam, ColorBuffer& colBuf) const;

    int BuildPath(std::vector<BDVertex*>& path, std::vector<BDSample>& samples, Light* light, bool lightPath) const;

    int BuildEyePath(int x, int y, vector<BDVertex*>& path, const Camera& cam,
                     vector<BDSample>& samples, Light* light) const;
    int BuildLightPath(vector<BDVertex*>& path, Light* light) const;

    Color EvalPath(vector<BDVertex*>& lightPath, vector<BDVertex*>& eyePath, 
                   int s, int t, Light* light) const;
    float WeighPath(int s, int t, vector<BDVertex*>& lightPath, 
                    vector<BDVertex*>& eyePath, Light* light, Camera* camera) const;

    float UniformWeight(int s, int t, vector<BDVertex*>& lightPath,
                      vector<BDVertex*>& eyePath, Light* light, Camera* camera) const;
    float PowerHeuristic(int s, int t, vector<BDVertex*>& lightPath,
                      vector<BDVertex*>& eyePath, Light* light, Camera* camera) const;

    int m_spp;
    mutable Random m_random;
};

#endif