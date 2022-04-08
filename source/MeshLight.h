#ifndef MESHLIGHT_H
#define MESHLIGHT_H

#include "Light.h"

class TriangleMesh;
class MeshTriangle;
class Material;
class IntersectionInfo;
class Vector3d;
class Random;
class Renderer;
class Scene;
class Matrix3d;

class TriangleNode
{
    friend class MeshLight;

    float cutoff;
    TriangleNode* leftChild, *rightChild;
    MeshTriangle* triangle;
};

class MeshLight : public Light
{
public:
    MeshLight(Color intensity, std::string fileName);
    ~MeshLight();
    virtual Color SampleRay(Ray& ray, Vector3d& Normal, float& areaPdf, float& anglePdf) const;
    float SamplePoint(Vector3d& point, Vector3d& Normal) const;

    virtual float Pdf(const IntersectionInfo& info, const Vector3d& out) const;
    Color GetIntensity() const;

    virtual Color NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, unsigned int component) const;
    virtual Color NextEventEstimationMIS(const Renderer* renderer, const IntersectionInfo& info, unsigned int component) const;
    virtual Color DirectHitMIS(const Renderer* renderer, 
                               const IntersectionInfo& lastInfo, 
                               const IntersectionInfo& thisInfo, 
                               unsigned int component) const;

    virtual float GetArea() const;
    virtual void AddToScene(std::shared_ptr<Scene>);

    virtual void Save(Bytestream& s) const;
    virtual void Load(Bytestream& s);

    void Transform(const Matrix3d& m);

    static Light* Create(unsigned char);

    void AddPortal(const Vector3d& pos, const Vector3d& v1, const Vector3d& v2);

protected:
    TriangleNode* BuildTree(int from, int to, float area, float cutoff);
    MeshTriangle* PickRandomTriangle() const;
    float area_;
    TriangleMesh* mesh_;
    Material* mat_;
    mutable Random r;
    TriangleNode* triangleTree_;
};

#endif