#pragma once

class Primitive;
class SpatialPartitioning;

#include "BoundingBox.h"
#include "SpatialPartitioning.h"
#include <cmath>
#include <vector>

class SAHEvent;

class KDPrimitive
{
public:
    const Primitive* p;
    int side;
};

class KDNode
{
public:
    KDNode();
    ~KDNode();
    void Build();
    std::vector<const Primitive*> m_primitives;
    static double SAHCost(int nPrimitives, double area, int nLeft, double leftarea, int nRight, double rightarea, int nPlanar, int side);

    std::pair<double, const Primitive*> IntersectRec(const Ray& ray, double tmin, double tmax, bool returnPrimitive) const;

    void Build(BoundingBox& bbox, std::vector<SAHEvent*>* events, const std::vector<KDPrimitive*>& primitives, int depth, int badsplits);
    bool IsLeaf() const;

    KDNode *leftNode, *rightNode;
    double m_splitpos;
    int splitdir;
};

class KDTree : public SpatialPartitioning
{
public:
    static double CalculateCost(int type, int samples);
    std::vector<const Primitive*> primitives;
    KDNode* m_root;
    KDTree();
    ~KDTree();
    void Build(const std::vector<const Primitive*>&);
    double Intersect(const Ray& ray, const Primitive* &primitive, double tmin, double tmax, bool returnPrimitive) const;
    BoundingBox CalculateExtents(const std::vector<const Primitive*>& primitives);

    BoundingBox m_bbox;

    static double mint;
    static double cost_triint, cost_trav, cost_boxint;
    static const int leftNode = 0, rightNode = 1;
};

class SAHEvent
{
public:
    SAHEvent(KDPrimitive*, double, int);

    KDPrimitive* primitive;
    double position;
    int type;
    static const char end = 0, planar = 1, start = 2;
};
