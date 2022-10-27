/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file BDPT.h
 * 
 * Declaration of the BDPT class.
 */

#pragma once

#include "Renderer.h"
#include "Randomizer.h"
#include "IntersectionInfo.h"
#include "Sample.h"
#include <mutex>

class Ray;
class Primitive;

class Roulette
{
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

protected:
    void RenderPixel(int x, int y, Camera& cam, ColorBuffer& eyeImage, ColorBuffer& lightImage);

    int BuildPath(std::vector<BDVertex*>& path, std::vector<BDSample>& samples, Light* light, bool lightPath);

    int BuildEyePath(int x, int y, std::vector<BDVertex*>& path, const Camera& cam,
                     std::vector<BDSample>& samples, Light* light);
    int BuildLightPath(std::vector<BDVertex*>& path, Light* light);

    Color EvalPath(const std::vector<BDVertex*>& lightPath, const std::vector<BDVertex*>& eyePath, 
                   int s, int t, Light* light) const;
    double WeighPath(int s, int t, std::vector<BDVertex*>& lightPath, 
                    std::vector<BDVertex*>& eyePath, Light* light, Camera* camera) const;

    double UniformWeight(int s, int t, std::vector<BDVertex*>& lightPath,
                      std::vector<BDVertex*>& eyePath, Light* light, Camera* camera) const;
    double PowerHeuristic(int s, int t, std::vector<BDVertex*>& lightPath,
                      std::vector<BDVertex*>& eyePath, Light* light, Camera* camera) const;
    
    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    Roulette* roulette;
    
    Randomizer m_random;
};
