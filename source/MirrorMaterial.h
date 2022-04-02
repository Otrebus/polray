#ifndef MIRRORMATERIAL_H
#define MIRRORMATERIAL_H

#include "Material.h"
#include "Vector3d.h"
#include "Color.h"
#include "GeometricRoutines.h"
#include "IntersectionInfo.h"
#include "Ray.h"
#include "Random.h"
#include "Texture.h"
#include <Windows.h>
#include <math.h>

class Ray;

#define F_PI 3.141592653f

class MirrorMaterial : public Material
{
public:
	MirrorMaterial();
	~MirrorMaterial();

	Color GetSample(const IntersectionInfo& info, Ray& out, bool adjoint) const;
	Color GetSampleE(const IntersectionInfo& info, Ray& out, float& pdf, float& rpdf, unsigned char& component, bool adjoint) const;

	Color BRDF(const IntersectionInfo& info, const Vector3d& out) const;
	Color ComponentBRDF(const IntersectionInfo& info, const Vector3d& out, unsigned char component) const;

	Light* GetLight() const;
	bool IsSpecular(unsigned char component) const;
    
    	virtual float PDF(const IntersectionInfo& info, const Vector3d& out, unsigned char component, bool adjoint) const;

	void ReadProperties(stringstream& ss);

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

	static int highestId;
	int id;
	Texture* texture;
	Texture* normalmap;
};

#endif
	