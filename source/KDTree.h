#ifndef KDTREE_H
#define KDTREE_H

#include "BoundingBox.h"
class Primitive;

#include <cmath>
#include <vector>

class SAHEvent;

class KDNode
{
public:
    KDNode();
    ~KDNode();
    void Build();
    KDNode *left, *right;
    vector<const Primitive*> m_primitives;
    bool Split(int, int, float);
    float Intersect(const Ray&, const Primitive*&) const;
    static float SAHCost(int nPrimitives, float area, int nLeft, float leftarea, int nRight, float rightarea, int nPlanar, int side);
    float IntersectIter(const Ray& ray, const Primitive* &minprimitive, float tmin, float tmax) const;
    float IntersectRec(const Ray& ray, const Primitive* &minprimitive, float tmin, float tmax) const;
    bool Intersect(const Ray&, float tmax) const;
    bool IsLeaf() const;

    float GetSplitPos() const;
    int GetSplitDir() const;
    KDNode* GetLeftNode() const;
    KDNode* GetRightNode() const;

    //bool m_isLeaf;

    //BoundingBox m_bbox;
    float m_splitpos;
    //float m_depth;
    int splitdir;
};

class KDTree
{
public:
    static float CalculateCost(int type, int samples);
    vector<const Primitive*> primitives;
    KDNode* m_root;
    KDTree();
    ~KDTree();
    void Build(vector<const Primitive*>);
    bool Intersect(const Ray&, float tmax) const;
    float Intersect(const Ray&, const Primitive*&) const;
    float Intersect(const Ray& ray, const Primitive* &primitive, float tmin, float tmax) const;
    BoundingBox CalculateExtents(vector<const Primitive*>& primitives);
    
    void BuildNode(KDNode* node, BoundingBox& bbox, vector<SAHEvent*>* events, vector<const Primitive*>& primitives, int depth, int badsplits);

    BoundingBox m_bbox;

    static float mint;
    static float cost_triint;
    static float cost_trav;
    static float cost_boxint;
    static const int yzplane = 0;
    static const int xzplane = 1;
    static const int xyplane = 2;
    static const int left = 0;
    static const int right = 1;
};

class SAHEvent
{
public:
    SAHEvent(const Primitive*, float, int);
    const Primitive* triangle;
    float position;
    int type;
    static const char end = 0;
    static const char planar = 1;
    static const char start = 2;
};

#endif
