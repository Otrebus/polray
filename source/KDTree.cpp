/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file KDTree.cpp
 * 
 * Declaration of the KDTree class used for spatial partitioning.
 */

#pragma once

#include <algorithm>
#include "KDTree.h"
#include "Primitive.h"
#include "Triangle.h"
#include "Utils.h"
#include "Timer.h"
#include "Randomizer.h"

double KDTree::cost_triint;
double KDTree::cost_trav;
double KDTree::cost_boxint;
double KDTree::mint;

/**
 * Creates an event from a primitive, position and type.
 * 
 * @param m The primitive to form the event from.
 * @param p The position to form the event from.
 * @param t The type (start, end, planar) to form the event from.
 */
SAHEvent::SAHEvent(KDPrimitive* m, double p, int t) : primitive(m), position(p), type(t)
{
}

/**
 * The sorting function for events; sorts by position first and then by type
 * 
 * @param a An event.
 * @param b Another event.
 * @returns True if a precedes b.
 */
bool sortFn(const SAHEvent* a, const SAHEvent* b)
{
    return a->position == b->position ? a->type < b->type : a->position < b->position;
}

/**
 * Calculates the cost of splitting the node at the given location.
 * 
 * @param nLeft The number of primitives that would end up in the left node.
 * @param leftarea The surface area of the bounding box of the left node.
 * @param nRight The number of primitives that would end up in the right node.
 * @param rightarea The surface area of the bounding box of the right node.
 * @param nPlanar The number of planar primitives perfectly straddling the nodes.
 * @param side The side to assign the planar primitives to.
 */
double KDNode::SAHCost(int, double, int nLeft, double leftarea, int nRight, double rightarea, int nPlanar, int side)
{
    double cost;
    if(side == KDTree::leftNode)
    {
        cost = (nLeft + nPlanar)*leftarea + nRight*rightarea;
        cost *= KDTree::cost_triint;
        if(nLeft + nPlanar == 0 || nRight == 0)
            cost *= 1.0;
    }
    else if(side == KDTree::rightNode)
    {
        cost = nLeft*leftarea + (nRight + nPlanar)*rightarea;
        cost *= KDTree::cost_triint;
        if(nRight + nPlanar == 0 || nLeft == 0)
            cost *= 1.0;
    }
    else
    {
        __debugbreak();
        return 0; // To appease the compiler
    }
    return cost;
}

/**
 * Estimates the cost of a ray/triangle or a bounding box intersection.
 * 
 * @param type 0 for triangle intersection, 1 for bounding box intersection
 * @param The number of attempts.
 */
double KDTree::CalculateCost(int type, int samples)
{
    Timer counter = Timer();
    Randomizer rnd;

    if(type == 0)
    {
        Triangle t(Vector3d(0.0, 1.0, 1.0), Vector3d(0.0, 0.0, 1.0), Vector3d(1.0, 0.0, 1.0));
        for(int i = 0; i < samples; i++)
        {
            Ray ray(Vector3d(0.5, 0.5, 0), Vector3d(rnd.GetDouble(0, 1), rnd.GetDouble(0, 1), 1));
            t.Intersect(ray);
        }
    }
    if(type == 1)
    {
        BoundingBox box(Vector3d(0, 0, 1), Vector3d(0.5, 1, 2));
        for(int i = 0; i < samples; i++)
        {
            Ray ray(Vector3d(0.5, 0.5, 0), Vector3d(rnd.GetDouble(0, 1), rnd.GetDouble(0, 1), 1));
            double d1, d2;
            box.Intersect(ray, d1, d2);
        }
    }
    return counter.GetTime() / double(samples);
}

/**
 * Constructor.
 */
KDTree::KDTree()
{
    KDTree::cost_triint = CalculateCost(0, 1000);
    KDTree::cost_boxint = CalculateCost(1, 1000);
    KDTree::cost_trav = KDTree::cost_boxint / 2;
    m_root = 0;
}

/**
 * Destructor.
 */
KDTree::~KDTree()
{
    delete m_root;
}

/**
 * Constructor.
 */
KDNode::KDNode()
{
    leftNode = nullptr;
    rightNode = nullptr;
}

/**
 * Destructor.
 */
KDNode::~KDNode()
{
    if(leftNode)
        delete leftNode;
    if(rightNode)
        delete rightNode;
}

//------------------------------------------------------------------------------
// Recursively build a Kd-Tree from this node (Currently uses an O(n(logn)^2) 
// algorithm).
//------------------------------------------------------------------------------
void KDNode::Build()
{
}

/**
 * Calculates the size of the Kd-Tree.
 * 
 * @param events The sorted event list.
 * @param add The unsorted event list.
 * @param primitive The primitive that caused the event.
 * @param add The event list to add to.
 * @param merge The merged list.
 */
BoundingBox KDTree::CalculateExtents(const std::vector<const Primitive*>& shapes)
{
    BoundingBox resultbox{ { inf, inf, inf }, { -inf, -inf, -inf } };

    for(auto& s : shapes)
    {
        const auto& bbox = s->GetBoundingBox();
        for(int i = 0; i < 3; i++)
        {
            resultbox.c1[i] = min(bbox.c1[i], resultbox.c1[i]);
            resultbox.c2[i] = max(bbox.c2[i], resultbox.c2[i]);
        }
    }
    return resultbox;
}

/**
 * Checks if the node is a leaf.
 * 
 * @returns True if the node is a leaf.
 */
bool KDNode::IsLeaf() const
{
    return !(leftNode || rightNode);
}

/**
 * Merges two lists of events, of which one is unsorted.
 * 
 * @param events The sorted event list.
 * @param add The unsorted event list.
 * @param primitive The primitive that caused the event.
 * @param add The event list to add to.
 * @param merge The merged list.
 */
std::vector<SAHEvent*> MergeEvents(std::vector<SAHEvent*>& events, std::vector<SAHEvent*>& add)
{
    std::vector<SAHEvent*> merged;
    sort(add.begin(), add.end(), sortFn);
    std::merge(events.begin(), events.end(), add.begin(), add.end(), std::back_inserter(merged), sortFn);
    return merged;
}

/**
 * Adds an event into an event list.
 * 
 * @param minpoint The minimum point of the primitive along the axis.
 * @param maxpoint The maximum point of the primitive along the axis.
 * @param primitive The primitive that caused the event.
 * @param add The event list to add to.
 */
void AddEvent(double& minpoint, double& maxpoint, KDPrimitive*& primitive, std::vector<SAHEvent*>& add)
{
    // Planar primitive - insert a planar event into the queue
    if(minpoint == maxpoint)
        add.push_back(new SAHEvent(primitive, minpoint, SAHEvent::planar));
    else
    {
        add.push_back(new SAHEvent(primitive, minpoint, SAHEvent::start));
        add.push_back(new SAHEvent(primitive, maxpoint, SAHEvent::end));
    }
}

/**
 * Builds a node of the K-d tree given an event list and a set of primitives that are part of the
 * node, using the surface-area heuristic.
 * 
 * @param bbox The bounding box of the node.
 * @param events The events for each dimension.
 * @param shapes The primitives to build the partition structure for.
 * @param depth The depth of the node.
 * @param badsplits The number of bad split attempts allowed.
 */
void KDNode::Build(BoundingBox& bbox, std::vector<SAHEvent*>* events, const std::vector<KDPrimitive*>& shapes, int depth, int badsplits)
{
    if(depth > 20 || shapes.size() < 4) // TODO: fix
    {
        for(auto& s : shapes)
            m_primitives.push_back(s->p);
        return;
    }

    double bestsplit = 0;
    int bestsplitdir = 0;
    char bestside = 0;
    double bestcost = inf;
    double boxarea = 2*(bbox.c2.z-bbox.c1.z)*(bbox.c2.y-bbox.c1.y) + 2*(bbox.c2.x-bbox.c1.x)*(bbox.c2.z-bbox.c1.z) + 2*(bbox.c2.x-bbox.c1.x)*(bbox.c2.y-bbox.c1.y);

    for(int u = 0; u < 3; u++)
    {
        int v = (u + 1) % 3, w = (u + 2) % 3;

        // First, initiate the primitive counting variables
        int nLeft = 0, nRight = (int) shapes.size();

        double leftarea, rightarea;
        double sidearea = (bbox.c2[v]-bbox.c1[v])*(bbox.c2[w]-bbox.c1[w]);
        double vLength = bbox.c2[v]-bbox.c1[v], wLength = bbox.c2[w]-bbox.c1[w];
        int pp = 0, pe = 0, ps = 0; // Event counters for current position

        // Sweep through all events
        for(auto it = events[u].begin(); it < events[u].end(); )
        {
            double sweeppos = (*it)->position;

            leftarea = 2*sidearea + 2*(sweeppos-bbox.c1[u])*(vLength + wLength);
            rightarea = 2*sidearea + 2*(bbox.c2[u]-sweeppos)*(vLength + wLength);

            // Go through all events on this position
            for(ps = pp = pe = 0; it < events[u].end() && (*it)->position == sweeppos; it++)
                if((*it)->type == SAHEvent::end)
                    pe++;
                else if((*it)->type == SAHEvent::start)
                    ps++;
                else
                    pp++;

            nRight -= pp + pe;

            // Calculate the costs for a split at this location
            double leftcost = KDNode::SAHCost((int) shapes.size(), boxarea, nLeft, leftarea, nRight, rightarea, pp, KDTree::leftNode);
            double rightcost = KDNode::SAHCost((int) shapes.size(), boxarea, nLeft, leftarea, nRight, rightarea, pp, KDTree::rightNode);

            if(bestcost > min(leftcost, rightcost)) 
            {
                bestcost = min(leftcost, rightcost);
                bestsplitdir = u;
                bestsplit = sweeppos;
                bestside = leftcost < rightcost ? KDTree::leftNode : KDTree::rightNode;
            }

            nLeft += pp + ps;
        }
    }

    // It's not worth it to split this node, make it a leaf
    if(bestcost + boxarea*KDTree::cost_trav > KDTree::cost_triint*shapes.size()*boxarea)
    {
        if(badsplits <= 0 || bestcost == inf)
        {
            for(auto& s : shapes)
                m_primitives.push_back(s->p);
            return;
        }
        else
            badsplits--;
    }

    // Ok, now we've found the best split location, it's time to split and prepare for recursion
    int a = bestsplitdir;

    std::vector<Primitive*> nodeprimitives;

    leftNode = new KDNode();
    rightNode = new KDNode();

    m_splitpos = bestsplit;
    splitdir = bestsplitdir;

    std::vector<KDPrimitive*> leftprimitives, rightprimitives;
    std::vector<SAHEvent*> leftevents[3], rightevents[3], addleft[3], addright[3];

    const int left = 0, both = 1, right = 2;

    // Mark all primitives as straddling for now
    for(auto& s : shapes)
        s->side = both;

    for(auto& e : events[a])
    {
        if(e->position <= bestsplit && e->type == SAHEvent::end)
            e->primitive->side = left;
        else if(e->position >= bestsplit && e->type == SAHEvent::start)
            e->primitive->side = right;
        else if(e->position == bestsplit && e->type == SAHEvent::planar)
        {
            if(bestside == KDTree::leftNode)
                e->primitive->side = left;
            else
                e->primitive->side = right;
        }
    }

    for(int u = 0; u < 3; u++)
        for(auto& e : events[u])
            if(e->primitive->side == left)
                leftevents[u].push_back(e);
            else if(e->primitive->side == right)
                rightevents[u].push_back(e);

    BoundingBox leftbbox = bbox, rightbbox = bbox;
    leftbbox.c2[a] = bestsplit;
    rightbbox.c1[a] = bestsplit;

    for(auto s : shapes)
    {
        if(s->side == left)
            leftprimitives.push_back(s);
        if(s->side == right)
            rightprimitives.push_back(s);

        if(s->side == both)
        {
            // Get the bounding boxes clipped to the box halves
            auto [isinleft, leftclippedbox] = s->p->GetClippedBoundingBox(leftbbox);
            if(isinleft)
                leftprimitives.push_back(s);

            auto [isinright, rightclippedbox] = s->p->GetClippedBoundingBox(rightbbox);
            if(isinright)
                rightprimitives.push_back(s);
            
            for(int u = 0; u < 3; u++)
            {
                if(isinleft)
                    AddEvent(leftclippedbox.c1[u], leftclippedbox.c2[u], s, addleft[u]);

                if(isinright)
                    AddEvent(rightclippedbox.c1[u], rightclippedbox.c2[u], s, addright[u]);
            }
        }
    }
    std::vector<SAHEvent*> mergedLeft[3], mergedRight[3];

    // Merge the events of the clipped triangles to the respective event lists
    for(int u = 0; u < 3; u++)   
        mergedLeft[u] = MergeEvents(leftevents[u], addleft[u]);

    for(int u = 0; u < 3; u++)
        mergedRight[u] = MergeEvents(rightevents[u], addright[u]);

    leftNode->Build(leftbbox, mergedLeft, leftprimitives, depth + 1, badsplits);
    leftprimitives.clear();
    for(int u = 0; u < 3; u++)
    {
        for(auto& e : addleft[u])
            delete e;
        addleft[u].clear();
        mergedLeft[u].clear();
    }

    rightNode->Build(rightbbox, mergedRight, rightprimitives, depth+1, badsplits);
    for(int u = 0; u < 3; u++)
        for(auto& e : addright[u])
            delete e;
}

/**
 * Builds a K-d tree from a set of primitives.
 * 
 * @param shapes The primitives to build the partition structure for.
 */
void KDTree::Build(const std::vector<const Primitive*>& shapes)
{
    m_root = new KDNode();
    m_bbox = CalculateExtents(shapes);
    std::vector<SAHEvent*> eventlist[3];

    std::vector<KDPrimitive*> primitives;
    for(auto& s : shapes)
        primitives.push_back(new KDPrimitive{s, 0});

    // Loop through each axis - u is the primary axis
    for(int u = 0; u < 3; u++)
    {
        // Create event lists from the objects
        for(auto& s : primitives)
        {
            // Get the bounding box of the primitive culled by the bounding box
            auto [hasBox, clippedbox] = s->p->GetClippedBoundingBox(m_bbox);
            if(hasBox)
                AddEvent(clippedbox.c1[u], clippedbox.c2[u], s, eventlist[u]);
        }

        // Sort the event list
        std::sort(eventlist[u].begin(), eventlist[u].end(), sortFn);
    }

    m_root->Build(m_bbox, eventlist, primitives, 0, 3);

    for(int u = 0; u < 3; u++)
        for(auto& e : eventlist[u])
            delete e;

    for(auto p : primitives)
        delete p;
}

/**
 * Intersects the contents of the K-d tree with a ray.
 * 
 * @param ray The ray to intersect with.
 * @param tmin The smallest distance along the ray to find intersections.
 * @param tmax The greatest distance along the ray to find intersections.
 * @param returnPrimitive Whether to find the smallest t and return the primitive that was intersected.
 * @returns The distance along the ray that the intersection happened, or -inf if
 *          no intersection happened, along with the primitive that was intersected.
 */
std::pair<double, const Primitive*> KDNode::IntersectRec(const Ray& ray, double tmin, double tmax, bool returnPrimitive=false) const
{
    if(tmin > tmax)
        return { -inf, nullptr };

    int a = splitdir;
    double locmint = inf;
    const Primitive* minprimitive = nullptr;
    if(IsLeaf())
    {
        for(auto& s : m_primitives)
        {
            double t = s->Intersect(ray);
            if(t >= tmin && t <= tmax)
            {
                if(!returnPrimitive)
                    return { t, nullptr };
                if(t < locmint)
                    minprimitive = s, locmint = t;
            }
        }        
        if(minprimitive)
            return { locmint, minprimitive };
        return { -inf, nullptr };
    }

    double tint = (m_splitpos - ray.origin[a])/ray.direction[a];

    KDNode* nearnode = ray.direction[a] > 0 ? leftNode : rightNode;
    KDNode* farnode = nearnode == leftNode ? rightNode : leftNode;

    if(tint <= tmin)
    {
        auto [t, primitive] = farnode->IntersectRec(ray, std::max(tmin, tint - eps), tmax, returnPrimitive);
        if(t != -inf)
            return { t, primitive };
    }
    else
    {
        auto [t1, primitive1] = nearnode->IntersectRec(ray, tmin, std::min(tint + eps, tmax), returnPrimitive);
        if(t1 != -inf)
            return { t1, primitive1 };

        auto [t2, primitive2] = farnode->IntersectRec(ray, std::max(tmin, tint - eps), tmax, returnPrimitive);
        if(t2 != -inf)
            return { t2, primitive2 };
    }
    return { -inf, nullptr };
}

/**
 * Intersects the contents of the K-d tree with a ray.
 * 
 * @param ray The ray to intersect with.
 * @param primitive The intersected triangle.
 * @param tmin The smallest distance along the ray to find intersections.
 * @param tmax The greatest distance along the ray to find intersections.
 * @param returnPrimitive Whether to find the smallest distance along the ray that the
                          primitive was intersected and return the primitive that was
                          intersected, or just reporting any distance and returning no
                          primitive.
 * @returns The distance along the ray that the intersection happened, or -inf if
 *          no intersection happened.
 */
std::tuple<double, const Primitive*> KDTree::Intersect(const Ray& ray, double tmin, double tmax, bool returnPrimitive = true) const
{
    auto [t, prim] = m_root->IntersectRec(ray, tmin, tmax, returnPrimitive);
    if(t != -inf)
        return { t, prim };
    return { -inf, nullptr };
}
