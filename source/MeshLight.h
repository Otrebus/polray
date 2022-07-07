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

    double cutoff;
    TriangleNode* leftChild, *rightChild;
    MeshTriangle* triangle;
};

class MeshLight : public Light
{
public:
    MeshLight(Color intensity, std::string fileName);
    MeshLight(Color intensity);
    ~MeshLight();
    virtual Color SampleRay(Ray& ray, Vector3d& Normal, double& areaPdf, double& anglePdf) const;
    void SamplePoint(Vector3d& point, Vector3d& Normal) const;

    double Intersect(const Ray& ray) const;
    bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    virtual double Pdf(const IntersectionInfo& info, const Vector3d& out) const;
    Color GetIntensity() const;

    virtual Color NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Vector3d& lightPoint, Vector3d& lightNormal, int component) const;
    virtual Color NextEventEstimationMIS(const Renderer* renderer, const IntersectionInfo& info, int component) const;
    virtual Color DirectHitMIS(const Renderer* renderer, 
                               const IntersectionInfo& lastInfo, 
                               const IntersectionInfo& thisInfo, int component) const;

    virtual double GetArea() const;

    virtual void Save(Bytestream& s) const;
    virtual void Load(Bytestream& s);

    void Transform(const Matrix3d& m);

    static Light* Create(unsigned char);

    void AddPortal(const Vector3d& pos, const Vector3d& v1, const Vector3d& v2);

    TriangleMesh* mesh;
protected:
    mutable bool builtTree;
    friend class Scene;
    virtual void AddToScene(Scene*);

    TriangleNode* BuildTree(int from, int to, double area, double cutoff) const;
    MeshTriangle* PickRandomTriangle() const;
    mutable double area_;
    mutable Random r;
    mutable TriangleNode* triangleTree_;
};

#endif