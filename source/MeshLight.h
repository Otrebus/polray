#pragma once

#include "Light.h"
#include "KDTree.h"

class TriangleMesh;
class MeshTriangle;
class Material;
class IntersectionInfo;
class Vector3d;
class Random;
class Renderer;
class Scene;
class Matrix3d;

struct TriangleNode
{
    double cutoff;
    TriangleNode* leftChild, *rightChild;
    MeshTriangle* triangle;
};

class MeshLight : public Light
{
public:
    MeshLight(Color intensity, std::string fileName);
    MeshLight(Color intensity);
    MeshLight();
    ~MeshLight();
    std::tuple<Ray, Color, Vector3d, AreaPdf, AnglePdf> SampleRay() const;
    std::tuple<Point, Normal> SamplePoint() const;

    double Intersect(const Ray& ray) const;
    bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    virtual double Pdf(const IntersectionInfo& info, const Vector3d& out) const;
    Color GetIntensity() const;

    std::tuple<Color, Vector3d> NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, int component) const;

    virtual double GetArea() const;

    virtual void Save(Bytestream& s) const;
    virtual void Load(Bytestream& s);

    void Transform(const Matrix3d& m);

    void AddPortal(const Vector3d& pos, const Vector3d& v1, const Vector3d& v2);

    TriangleMesh* mesh;
protected:
    mutable bool builtTree;
    mutable KDTree tree;
    friend class Scene;
    virtual void AddToScene(Scene*);

    TriangleNode* BuildTree(int from, int to, double area, double cutoff) const;
    MeshTriangle* PickRandomTriangle() const;
    mutable double area_;
    mutable Random r;
    mutable TriangleNode* triangleTree_;
};
