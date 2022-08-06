#ifndef MATERIAL_H
#define MATERIAL_H

#include "vector3d.h"
#include "texture.h"
#include "Color.h"
#include "GeometricRoutines.h"
#include "Sample.h"
#include <sstream>

class Light;
class Ray;
class IntersectionInfo;
class Bytestream;

#define F_PI 3.141592653f

#define ID_ASHIKHMINSHIRLEY ((unsigned char)104)
#define ID_DIELECTRICMATERIAL ((unsigned char)102)
#define ID_MIRRORMATERIAL ((unsigned char)103)
#define ID_PHONGMATERIAL ((unsigned char)101)
#define ID_LAMBERTIANMATERIAL ((unsigned char)100)
#define ID_EMISSIVEMATERIAL ((unsigned char)105)
#define ID_COOKTORRANCEMATERIAL ((unsigned char)106)

class ShadingState;

class Material
{
public:
    Material();
    virtual ~Material();

    virtual Sample GetSample(const IntersectionInfo& info, bool adjoint) const = 0;

    virtual Color BRDF(const IntersectionInfo& info, const Vector3d& out, int component) const = 0;

    virtual Light* GetLight() const = 0;

    virtual void ReadProperties(stringstream& ss) = 0;
    virtual double PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const = 0;

    virtual void Save(Bytestream& stream) const = 0;
    virtual void Load(Bytestream& stream) = 0;

    static Material* Create(unsigned char);

    Light* light;
    static int highestId;
    int id;
    Texture* texture;
    Texture* normalmap;
};

#endif