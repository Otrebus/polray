#include "BDPT.h"
#include <vector>
#include "AreaLight.h"
#include "Primitive.h"
#include "Material.h"

BDSample::BDSample(int a, int b) : s(a), t(b)
{
}

BDSample::~BDSample()
{
}

BDPT::BDPT(std::shared_ptr<Scene> scene) : Renderer(scene)
{
	m_spp = 1;
	m_weightMethod = WEIGHT_UNIFORM;
}

BDPT::~BDPT()
{
}

int BDPT::BuildEyePath(int x, int y, vector<BDVertex*>& path, 
                       const Camera& cam, vector<BDSample>& samples, 
                       Light* light) const
{
	int depth = 1;
	float rr = 0.7f;

	BDVertex* camPoint = new BDVertex();
    Vector3d lensPoint;
    cam.SampleAperture(lensPoint, camPoint->camU, camPoint->camV);
	camPoint->out = cam.GetRayFromPixel(x, y, m_random.GetFloat(0, 1), 
                                        m_random.GetFloat(0, 1), camPoint->camU,
                                        camPoint->camV);
	camPoint->rr = 1;
	camPoint->alpha = Color::Identity;
	camPoint->info.normal = camPoint->info.geometricnormal = cam.dir;
	camPoint->info.position = camPoint->out.origin;
	camPoint->rpdf = 1;
	float costheta = abs(camPoint->out.direction*cam.dir);
	float lastPdf = camPoint->pdf = 
                    1/(cam.GetPixelArea()*costheta*costheta*costheta);
	Color lastSample = costheta*Color::Identity/lastPdf;
	lastPdf = camPoint->pdf = 1/(4*costheta*costheta*costheta);
	path.push_back(camPoint);

	IntersectionInfo info;

	while(depth < 3 || m_random.GetFloat(0.f, 1.f) < rr)
	{
		const Primitive* hitPrimitive;
		IntersectionInfo info;
		BDVertex* lastV = path.back();

		if(m_tree.Intersect(lastV->out, hitPrimitive) < 0)
			break;
		hitPrimitive->GenerateIntersectionInfo(lastV->out, info);

		BDVertex* newV = new BDVertex();
		newV->info = info;
		Vector3d v = (info.GetPosition() - lastV->out.origin);
		float lSqr = v.GetLengthSquared();
		v.Normalize();
		newV->pdf = lastPdf*(abs(info.GetGeometricNormal()
                         *info.GetDirection()))/(lSqr);
		newV->alpha = lastV->alpha*lastSample;
        Material* mat = info.GetMaterial();
		lastSample = mat->GetSampleE(info, newV->out, lastPdf, lastV->rpdf,
                                     newV->component, false);
		newV->rr = depth < 3 ? 1 : lastV->rr*rr;
        rr = min(lastSample.GetLuminance(), 1);
        if(newV->alpha.GetLuminance() <= 0 && info.GetMaterial()->GetLight() != light)
        {
            delete newV;
            return path.size();
        }
        lastV->rpdf *= abs(lastV->info.GetGeometricNormal()*v)/(lSqr);
		path.push_back(newV);

        if(info.material->GetLight() == light)
		{   // Direct light hit
			samples.push_back(BDSample(0, path.size()));
			return path.size() - 1;
		}

		depth++;
	}
	return path.size();
}

int BDPT::BuildLightPath(vector<BDVertex*>& path, Light* light) const
{
	int depth = 1;
	BDVertex* lightPoint = new BDVertex();
	float rr = 0.7f;
    float lastPdf;
    Color lastSample = light->SampleRay(lightPoint->out, lightPoint->info.normal,
                     lightPoint->pdf, lastPdf);
    lightPoint->alpha = light->GetArea()*light->GetIntensity();
	lightPoint->rr = 1;
    lightPoint->info.normal = lightPoint->info.normal;
    lightPoint->info.geometricnormal = lightPoint->info.normal;
	lightPoint->info.position = lightPoint->out.origin;
//    Color lastSample = Color::Identity*(lightPoint->info.normal
//                                        *lightPoint->out.direction)/lastPdf;
	path.push_back(lightPoint);

	while(depth < 3 || m_random.GetFloat(0.f, 1.f) < rr)
	{
		const Primitive* hitPrimitive;
		IntersectionInfo info;
		BDVertex* lastV = path.back();

		if(m_tree.Intersect(lastV->out, hitPrimitive) < 0)
			break;
		hitPrimitive->GenerateIntersectionInfo(lastV->out, info);

		BDVertex* newV = new BDVertex();
		newV->info = info;
		Vector3d v = (info.GetPosition() - lastV->out.origin);
		float lSqr = v.GetLengthSquared();
		v.Normalize();
		newV->pdf = lastPdf*(abs(info.geometricnormal
                                  *info.direction))/(lSqr);
		newV->alpha = lastV->alpha*lastSample;
        Material* mat = info.material;
		lastSample = mat->GetSampleE(info, newV->out, lastPdf, lastV->rpdf, 
                                     newV->component, true);
        newV->rr = depth < 3 ? 1 : lastV->rr*rr;
        rr = min(newV->alpha.GetLuminance(), 1);
        if(lastSample.GetLuminance() <= 0)
        {
            delete newV;
            return path.size();
        }
        lastV->rpdf *= abs(lastV->info.GetGeometricNormal()*v)/(lSqr);

		path.push_back(newV);
		depth++;
	}
	return path.size();
}

Color BDPT::EvalPath(vector<BDVertex*>& lightPath, vector<BDVertex*>& eyePath,
                     int s, int t, Light* light) const
{
	Color result(1, 1, 1);

	if(t == 0) // 0 eye path vertices not possibe since cam is not part of scene
		return Color(0, 0, 0);
	if(s == 0) // 0 light path vertices, we directly hit the light source
	{          // - has to be handled a little bit differently
		BDVertex* prevE = eyePath[t-1];
        Vector3d lightNormal = eyePath[t-1]->info.normal;
		if(t == 1) // Direct light hit
			if(eyePath[0]->out.direction*lightNormal < 0) 
				return light->GetIntensity();
			else 
                return Color(0, 0, 0);
		else
        {
            BDVertex* lastE = eyePath[t-2]; 
			if(lastE->out.direction*lightNormal < 0)
                return light->GetIntensity()*prevE->alpha/prevE->rr;
			else 
                return Color(0, 0, 0);
        }
	}

    // Build the connecting vertex
	BDVertex* lastL = lightPath[s-1];
	BDVertex* lastE = eyePath[t-1];

	Ray c = Ray(lastE->out.origin, lastL->out.origin - lastE->out.origin);
	float r = c.direction.GetLength();
	c.direction.Normalize();

	if(!TraceShadowRay(c, r) || r < 0.005f)
	    return Color(0, 0, 0);

	result*= abs(lastL->info.GetGeometricNormal()*c.direction)
            *abs(lastE->info.GetNormal()*c.direction)/(r*r);
	result *= lastL->alpha*lastE->alpha;

    // This BRDF is backwards so let's modify it 
    float modifier = abs(lastL->info.direction*lastL->info.normal)
                    /abs(lastL->info.direction*lastL->info.geometricnormal);

	if(s > 1)
		result *= modifier*lastL->info.material->
                  ComponentBRDF(lastL->info, -c.direction, lastL->component)
                  /lastL->rr;
	if(t > 1)
		result *= lastE->info.material->
                  ComponentBRDF(lastE->info, c.direction, lastE->component)
                  /lastE->rr;
	return result;
}

float BDPT::UniformWeight(int s, int t, vector<BDVertex*>& lightPath,
                      vector<BDVertex*>& eyePath, Light*) const
{
	float weight = float(s+t);
	bool wasSpec = false;
	for(auto it = lightPath.cbegin() + 1; it < lightPath.cbegin() + s; it++)
	{
		BDVertex* v = *it;
		if(v->info.GetMaterial()->IsSpecular(v->component))
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
		if(v->info.GetMaterial()->IsSpecular(v->component))
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

float BDPT::PowerHeuristic(int s, int t, vector<BDVertex*>& lightPath,
                           vector<BDVertex*>& eyePath, Light* light) const
{
	float weight = 0;
	std::vector<float> forwardProbs(s+t);
	std::vector<float> backwardProbs(s+t);
	std::vector<bool> specular(s+t);

	for(int i = 0; i < s+t; i++)
		specular[i] = false;

    // Tag specular vertices
    for(int i = 1; i < s+t-1; i++)
    {
        BDVertex* v = i < s ? lightPath[i] : eyePath[s+t-i-1];
		if(v->info.GetMaterial()->IsSpecular(v->component))
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
		float lSqr = out.GetLengthSquared();
		out.Normalize();
        float newPdf;
        if(s > 1)
            newPdf = lastL->info.GetMaterial()->
                     PDF(info, out, lastL->component, true);
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
		float lSqr = out.GetLengthSquared();
		out.Normalize();
		float newPdf;
        float costheta = abs(lastE->info.geometricnormal*out);
        if(t == 1)
            newPdf = 1/(4*costheta*costheta*costheta);
        else
            newPdf = lastE->info.GetMaterial()->PDF(info, out, 
                                                    lastE->component, false);
		backwardProbs[s-1] = newPdf*abs(lastL->info.geometricnormal*out)/lSqr;
    }

    // The last backward pdf values are just the forward pdf values of eye path
    for(int i = s; i < s+t; i++)
	    backwardProbs[i] = eyePath[s+t-i-1]->pdf;
	
    // Sum the actual weights of the paths together
    float l = 1;
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

float BDPT::WeighPath(int s, int t, vector<BDVertex*>& lightPath,
                      vector<BDVertex*>& eyePath, Light* light) const
{
	return PowerHeuristic(s, t, lightPath, eyePath, light);
}

void BDPT::RenderPixel(int x, int y, Camera& cam, 
                       ColorBuffer& eyeImage, ColorBuffer& lightImage) const
{
	vector<BDSample> samples;
	vector<BDVertex*> eyePath;
	vector<BDVertex*> lightPath;

    float lightWeight;
    float r = m_random.GetFloat(0.0f, 1.0f);
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
        float weight = WeighPath(sample.s, sample.t, lightPath, eyePath, light);
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
			float costheta = abs(cam.dir*camRay.direction);
            float mod = (costheta*costheta*costheta*costheta*
                         cam.GetPixelArea()*xres*yres*lightWeight);
            Color result = 	weight*eval/mod;
			WeighPath(sample.s, sample.t, lightPath, eyePath, light);
			lightImage.AddColor(camx, camy, result);
		}
		else
		{
            float costheta = abs(cam.dir*eyePath[0]->out.direction);
            float mod = (costheta*costheta*costheta*costheta*
                         cam.GetPixelArea()*lightWeight);
            Color result = weight*eval/mod;
			eyeImage.AddColor(x, y, result);
		}
	}
	for(unsigned int s = 1; s < lightPath.size() + 1; s++)
		delete lightPath[s-1];
	for(unsigned int t = 1; t < eyePath.size() + 1; t++)
		delete eyePath[t-1];
}

void BDPT::RenderPart(Camera& cam, ColorBuffer& colBuf) const
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

void BDPT::Render(Camera& cam, ColorBuffer& colBuf) 
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
		partBuf[i]->Clear(0);
		RenderPart(cam, *partBuf[i]);
	}
	for(int i = 0; i < nCores; i++)
    {
		for(int x = 0; x < colBuf.GetXRes(); x++)
			for(int y = 0; y < colBuf.GetYRes(); y++)
				colBuf.AddColor(x, y, partBuf[i]->GetPixel(x, y)/nCores);
        delete partBuf[i];
    }
    delete[] partBuf;

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
