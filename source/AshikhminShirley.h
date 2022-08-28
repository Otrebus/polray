#pragma once

#include "Random.h"
#include "Material.h"
#include "Color.h"

class Light;
class Sample;
class Vector3d;

class AshikhminShirley : public Material
{

public:
	AshikhminShirley();
	~AshikhminShirley();

	Sample GetSample(const IntersectionInfo& info, bool adjoint) const;

	Color BRDF(const IntersectionInfo& info, const Vector3d& out, int component) const;

	Light* GetLight() const;
    
    virtual double PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const;

	void ReadProperties(std::stringstream& ss);

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

	mutable Random rnd;

	Color Rd, Rs;
    int n;
};

	