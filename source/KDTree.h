#pragma once

#include "BoundingBox.h"
#include "SpatialPartitioning.h"
#include "Timer.h"
class Primitive;

#include <cmath>
#include <vector>

class SAHEvent;

struct IntResult {
    double t;
    const Primitive* primitive;
};

class KDNode
{
public:
    KDNode();
    ~KDNode();
    void Build();
    KDNode *left, *right;
    vector<const Primitive*> m_primitives;
    bool Split(int, int, double);
    double Intersect(const Ray&, const Primitive*&) const;
    static double SAHCost(int nPrimitives, double area, int nLeft, double leftarea, int nRight, double rightarea, int nPlanar, int side);
    IntResult IntersectRec(const Ray& ray, double tmin, double tmax, bool returnPrimitive) const;
    double IntersectIter(const Ray& _ray, const Primitive* &minprimitive, double tmin, double tmax) const;
    bool IsLeaf() const;

    //bool m_isLeaf;

    //BoundingBox m_bbox;
    double m_splitpos;
    //double m_depth;
    int splitdir;
};

class KDTree : public SpatialPartitioning
{
public:
    static double CalculateCost(int type, int samples);
    vector<const Primitive*> primitives;
    KDNode* m_root;
    KDTree();
    ~KDTree();
    void Build(vector<const Primitive*>);
    double Intersect(const Ray& ray, const Primitive* &primitive, double tmin, double tmax, bool returnPrimitive) const;
    BoundingBox CalculateExtents(vector<const Primitive*>& primitives);
    
    void BuildNode(KDNode* node, BoundingBox& bbox, vector<SAHEvent*>* events, vector<const Primitive*>& primitives, int depth, int badsplits);

    BoundingBox m_bbox;

    static double mint;
    static double cost_triint;
    static double cost_trav;
    static double cost_boxint;
    static const int yzplane = 0;
    static const int xzplane = 1;
    static const int xyplane = 2;
    static const int left = 0;
    static const int right = 1;
};

class SAHEvent
{
public:
    SAHEvent(const Primitive*, double, int);
    const Primitive* triangle;
    double position;
    int type;
    static const char end = 0;
    static const char planar = 1;
    static const char start = 2;
};
