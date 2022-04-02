#ifndef BDPT_H
#define BDPT_H

#include "Renderer.h"
#include "Random.h"
#include "IntersectionInfo.h"
#include "LightTree.h"

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
	float rr, pdf, rpdf, brdf, rbrdf;
	Color alpha;
	IntersectionInfo info;
	unsigned char component;
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

    int BuildEyePath(int x, int y, vector<BDVertex*>& path, const Camera& cam,
                     vector<BDSample>& samples, Light* light) const;
	int BuildLightPath(vector<BDVertex*>& path, Light* light) const;

    Color EvalPath(vector<BDVertex*>& lightPath, vector<BDVertex*>& eyePath, 
                   int s, int t, Light* light) const;
	float WeighPath(int s, int t, vector<BDVertex*>& lightPath, 
                    vector<BDVertex*>& eyePath, Light* light) const;

    float UniformWeight(int s, int t, vector<BDVertex*>& lightPath,
                      vector<BDVertex*>& eyePath, Light* light) const;
    float PowerHeuristic(int s, int t, vector<BDVertex*>& lightPath,
                      vector<BDVertex*>& eyePath, Light* light) const;

	int m_weightMethod;
	int m_spp;
	mutable Random m_random;
};

#endif