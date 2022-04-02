#ifndef ASHIKHMINSHIRLEY_H
#define ASHIKHMINSHIRLEY_H

#define _USE_MATH_DEFINES

#include "Material.h"
#include "Vector3d.h"
#include "Color.h"
#include "GeometricRoutines.h"
#include "IntersectionInfo.h"
#include "Ray.h"
#include "Random.h"
#include <Windows.h>
#include <math.h>
#include <functional>
#include "Renderer.h"

class AshikhminShirley : public Material
{

public:
	AshikhminShirley();
	~AshikhminShirley();

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

	Texture* texture;
	Texture* normalmap;

	mutable Random rnd;

	Color Rd, Rs;
    int n;
};

#endif
	