#ifndef KDTREE_H
#define NOMINMAX
#include "kdtree.h"
#include "trianglemesh.h"
#include "triangle.h"
#include <algorithm>
#include <stack>
#include "Main.h"
#include <intrin.h>
#include "Utils.h"
#define CHECKVALID(v) if(!( v.x < std::numeric_limits<double>::infinity() && v.x > -std::numeric_limits<double>::infinity() && v.x == v.x && v.y < std::numeric_limits<double>::infinity() && v.y > -std::numeric_limits<double>::infinity() && v.y == v.y && v.z < std::numeric_limits<double>::infinity() && v.z > -std::numeric_limits<double>::infinity() && v.z == v.z)) _asm int 3;
double KDTree::cost_triint;
double KDTree::cost_trav;
double KDTree::cost_boxint;
double KDTree::mint;

SAHEvent::SAHEvent(const Primitive* m, double p, int t) : triangle(m), position(p), type(t)
{
}

double KDNode::SAHCost(int nPrimitives, double area, int nLeft, double leftarea, int nRight, double rightarea, int nPlanar, int side)
{
    double cost;
    if(side == KDTree::left)
    {
        cost = (nLeft + nPlanar)*leftarea + nRight*rightarea;
        cost *= KDTree::cost_triint;
        if(nLeft + nPlanar == 0 || nRight == 0)
            cost *= 1.0f;
    }
    else if(side == KDTree::right)
    {
        cost = nLeft*leftarea + (nRight + nPlanar)*rightarea;
        cost *= KDTree::cost_triint;
        if(nRight + nPlanar == 0 || nLeft == 0)
            cost *= 1.0f;
    }
    else
        __debugbreak();
    return cost;
}

double KDTree::CalculateCost(int type, int samples)
{
    Timer counter = Timer();

    if(type == 0)
    {
        Triangle t(0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
        for(int i = 0; i < samples; i++)
        {
            Ray ray(Vector3d(0.5f, 0.5f, 0), Vector3d(double(rand()%10000)/10000.0f, double(rand()%10000)/10000.0f, 1));
            t.Intersect(ray);
        }
    }
    if(type == 1)
    {
        BoundingBox box(Vector3d(0, 0, 1), Vector3d(0.5, 1, 2));
        for(int i = 0; i < samples; i++)
        {
            Ray ray(Vector3d(0.5f, 0.5f, 0), Vector3d(double(rand()%10000)/10000.0f, double(rand()%10000)/10000.0f, 1));
            box.Intersects(ray);
        }
    }
    return counter.GetTime() / double(samples);
}

KDTree::KDTree()
{
    KDTree::cost_triint = CalculateCost(0, 1000);
    KDTree::cost_boxint = CalculateCost(1, 1000);
    KDTree::cost_trav = KDTree::cost_boxint / 2;
    m_root = 0;
}

KDTree::~KDTree()
{
    delete m_root;
}

KDNode::KDNode()
{
    left = 0;
    right = 0;
    splitdir = KDTree::xyplane;
}

KDNode::~KDNode()
{
    if(left)
        delete left;
    if(right)
        delete right;
}

//------------------------------------------------------------------------------
// Recursively build a KD-Tree from this node (Currently uses an O(n(logn)^2) 
// algorithm).
//------------------------------------------------------------------------------
void KDNode::Build()
{
}

BoundingBox KDTree::CalculateExtents(vector<const Primitive*>& primitives)
{
    const auto inf = numeric_limits<double>::infinity();
    double maxx = -inf, maxy = -inf, maxz = -inf, minx = inf, miny = inf, minz = inf;

    for(auto& s : primitives)
    {
        double smaxx, smaxy, smaxz, sminx, sminy, sminz;

        smaxx = s->GetBoundingBox().c2.x;
        smaxy = s->GetBoundingBox().c2.y;
        smaxz = s->GetBoundingBox().c2.z;

        sminx = s->GetBoundingBox().c1.x;
        sminy = s->GetBoundingBox().c1.y;
        sminz = s->GetBoundingBox().c1.z;

        if(maxx < smaxx)
            maxx = smaxx;
        if(maxy < smaxy)
            maxy = smaxy;
        if(maxz < smaxz)
            maxz = smaxz;

        if(minx > sminx)
            minx = sminx;
        if(miny > sminy)
            miny = sminy;
        if(minz > sminz)
            minz = sminz;
    }
    return BoundingBox(Vector3d(minx, miny, minz), Vector3d(maxx, maxy, maxz));
}

bool KDNode::IsLeaf() const
{
    return !(left || right);
    //return m_isLeaf;
}

void KDTree::BuildNode(KDNode* node, BoundingBox& bbox, vector<SAHEvent*>* events, vector<const Primitive*>& primitives, int depth, int badsplits)
{
    if(depth > 20 || primitives.size() < 4) // TODO: fix
    {
        node->m_primitives = primitives;
        //node->m_isLeaf = true;
        return;
    }

    double bestsplit;
    int bestsplitdir;
    char bestside;
    double bestcost = numeric_limits<double>::infinity();
    double boxarea = 2*(bbox.c2.z-bbox.c1.z)*(bbox.c2.y-bbox.c1.y) + 2*(bbox.c2.x-bbox.c1.x)*(bbox.c2.z-bbox.c1.z) + 2*(bbox.c2.x-bbox.c1.x)*(bbox.c2.y-bbox.c1.y);

    for(int u = 0; u < 3; u++)
    {
        int v = (u + 1) % 3;
        int w = (u + 2) % 3;

        if(g_quitting)
            return;

        // First, initiate the primitive counting variables
        int nLeft = 0;
        int nPlanar = 0;
        int nRight = primitives.size();
        int side = 0;
        double leftarea, rightarea;
        double sidearea = (bbox.c2[v]-bbox.c1[v])*(bbox.c2[w]-bbox.c1[w]);
        double height = bbox.c2[v]-bbox.c1[v];
        double depth = bbox.c2[w]-bbox.c1[w];
        int pp = 0, pe = 0, ps = 0; // Event counters for current position

        // Sweep through all events
        vector<SAHEvent*>::iterator it = events[u].begin();
        
        while(it < events[u].end())
        {
            SAHEvent* e = *it;
            double sweeppos = e->position;

            leftarea = 2*sidearea + 2*(sweeppos-bbox.c1[u])*(height + depth);
            rightarea = 2*sidearea + 2*(bbox.c2[u]-sweeppos)*(height + depth);

            // Go through all events on this position
            ps = pp = pe = 0;

            while(it < events[u].end() && (*it)->position == sweeppos)
            {
                SAHEvent* e2 = *it;
                if(e2->type == SAHEvent::planar)
                    pp++;
                else if(e2->type == SAHEvent::end)
                    pe++;
                else if(e2->type == SAHEvent::start)
                    ps++;
//				delete e2;
                it++;
            }

            nRight -= pp;
            nRight -= pe;

            // Calculate the costs for a split at this location
            double leftcost = KDNode::SAHCost(primitives.size(), boxarea, nLeft, leftarea, nRight, rightarea, pp, KDTree::left);
            double rightcost = KDNode::SAHCost(primitives.size(), boxarea, nLeft, leftarea, nRight, rightarea, pp, KDTree::right);

            if(bestcost > min(leftcost, rightcost)) 
            {
                bestcost = min(leftcost, rightcost);
                bestsplitdir = u;
                bestsplit = sweeppos;
                bestside = leftcost < rightcost ? KDTree::left : KDTree::right;
            }

            nLeft += pp;
            nLeft += ps;

            if(it == events[u].end())
                break;
        }
    }

    // It's not worth it to split this node, make it a leaf
    if(bestcost + boxarea*KDTree::cost_trav > KDTree::cost_triint*primitives.size()*boxarea)
    {
        if(badsplits <= 0 || bestcost == numeric_limits<double>::infinity())
        {
            node->m_primitives = primitives;
            //node->m_isLeaf = true;
            return;
        }
        else
            badsplits--;

    }

    // Ok, now we've found the best split location, it's time to split and prepare for recursion
    int a = bestsplitdir;
    int b = (a + 1) % 3;
    int c = (b + 2) % 3;

    vector<Primitive*> nodeprimitives;

    node->left = new KDNode();
    node->right = new KDNode();
    
    BoundingBox leftbbox;
    leftbbox.c1 = bbox.c1;
    leftbbox.c2 = bbox.c2;
    leftbbox.c2[a] = bestsplit;

    BoundingBox rightbbox;
    rightbbox.c1 = bbox.c1;
    rightbbox.c1[a] = bestsplit;
    rightbbox.c2 = bbox.c2;

    node->m_splitpos = bestsplit;
    node->splitdir = bestsplitdir;

    vector<const Primitive*> leftprimitives, rightprimitives;
    vector<SAHEvent*> leftevents[3], rightevents[3], addleft[3], addright[3];

    const int leftonly = 0;
    const int both = 1;
    const int rightonly = 2;

    // Mark all primitives as straddling for now
    for(auto& s : primitives)
        s->side = both;
    for(auto& e : events[a])
    {
        if(e->position <= bestsplit && e->type == SAHEvent::end)
            e->triangle->side = leftonly;
        else if(e->position >= bestsplit && e->type == SAHEvent::start)
            e->triangle->side = rightonly;
        else if(e->position == bestsplit && e->type == SAHEvent::planar)
        {
            if(bestside == KDTree::left)
                e->triangle->side = leftonly;
            else
                e->triangle->side = rightonly;
        }
    }

    for(int u = 0; u < 3; u++)
    {
        for(auto& e : events[u])
        {
            const Primitive* s = e->triangle;

            if(s->side == leftonly)
                leftevents[u].push_back(e);
            else if(s->side == rightonly)
                rightevents[u].push_back(e);
        }
    }

    for(auto s : primitives)
    {
        if(s->side == leftonly)
            leftprimitives.push_back(s);
        if(s->side == rightonly)
            rightprimitives.push_back(s);

        if(s->side == both)
        {
            BoundingBox leftclippedbox, rightclippedbox;

            // Get the bounding boxes clipped to the box halves
            bool isinleft = s->GetClippedBoundingBox(leftbbox, leftclippedbox);
            if(isinleft)
                leftprimitives.push_back(s);
            bool isinright = s->GetClippedBoundingBox(rightbbox, rightclippedbox);
            if(isinright)
                rightprimitives.push_back(s);
            
            for(int u = 0; u < 3; u++)
            {
                double maxpoint, minpoint;

                if(isinleft)
                {
                    minpoint = leftclippedbox.c1[u];
                    maxpoint = leftclippedbox.c2[u];

                    // Planar primitive - insert a planar event into the queue
                    if(minpoint == maxpoint)
                    {
                        SAHEvent* e = new SAHEvent(s, minpoint, SAHEvent::planar);
                        addleft[u].push_back(e);
                    }

                    else
                    {
                        SAHEvent* e1 = new SAHEvent(s, minpoint, SAHEvent::start);
                        SAHEvent* e2 = new SAHEvent(s, maxpoint, SAHEvent::end);
                        addleft[u].push_back(e1);
                        addleft[u].push_back(e2);
                    }
                }

                if(isinright)
                {
                    minpoint = rightclippedbox.c1[u];
                    maxpoint = rightclippedbox.c2[u];

                    // Planar primitive - insert a planar event into the queue
                    if(minpoint == maxpoint)
                    {
                        SAHEvent* e = new SAHEvent(s, minpoint, SAHEvent::planar);
                        addright[u].push_back(e);
                    }

                    else
                    {
                        SAHEvent* e1 = new SAHEvent(s, minpoint, SAHEvent::start);
                        SAHEvent* e2 = new SAHEvent(s, maxpoint, SAHEvent::end);
                        addright[u].push_back(e1);
                        addright[u].push_back(e2);
                    }
                }
            }
        }
    }
    vector<SAHEvent*> mergedLeft[3], mergedRight[3];

    // Merge the events of the clipped triangles to the respective event lists
    for(int u = 0; u < 3; u++)
    {
        sort(addleft[u].begin(), addleft[u].end(), 
            [] (SAHEvent* e1, SAHEvent* e2) -> bool 
        { return e1->position == e2->position ? e1->type < e2->type : e1->position < e2->position; });
        
        vector<SAHEvent*>::iterator evIt = leftevents[u].begin();
        vector<SAHEvent*>::iterator addIt = addleft[u].begin();

        while(true)
        {
            if(evIt == leftevents[u].end())
            {
                while(addIt < addleft[u].end())
                {
                    mergedLeft[u].push_back(*addIt);
                    addIt++;
                }
                break;
            }
            else if(addIt == addleft[u].end())
            {
                while(evIt < leftevents[u].end())
                {
                    mergedLeft[u].push_back(*evIt);
                    evIt++;
                }
                break;
            }

            SAHEvent* leftEv = *evIt;
            SAHEvent* addEv = *addIt;

            if(leftEv->position < addEv->position)
            {
                mergedLeft[u].push_back(leftEv);
                evIt++;
                continue;
            }
            else if(leftEv->position > addEv->position)
            {
                mergedLeft[u].push_back(addEv);
                addIt++;
                continue;
            }
            else
            {
                if(leftEv->type < addEv->type)
                {
                    mergedLeft[u].push_back(leftEv);
                    evIt++;
                }
                else
                {
                    mergedLeft[u].push_back(addEv);
                    addIt++;
                }
            }
        }
    }

    for(int u = 0; u < 3; u++)
    {
        std::sort(addright[u].begin(), addright[u].end(), [] (SAHEvent* e1, SAHEvent* e2) {
            return e1->position == e2->position ? e1->type < e2->type : e1->position < e2->position;
        });

        vector<SAHEvent*>::iterator evIt = rightevents[u].begin();
        vector<SAHEvent*>::iterator addIt = addright[u].begin();

        while(true)
        {
            if(evIt == rightevents[u].end())
            {
                while(addIt < addright[u].end())
                {
                    mergedRight[u].push_back(*addIt);
                    addIt++;
                }
                break;
            }
            else if(addIt == addright[u].end())
            {
                while(evIt < rightevents[u].end())
                {
                    mergedRight[u].push_back(*evIt);
                    evIt++;
                }
                break;
            }

            SAHEvent* rightEv = *evIt;
            SAHEvent* addEv = *addIt;

            if(rightEv->position < addEv->position)
            {
                mergedRight[u].push_back(rightEv);
                evIt++;
                continue;
            }
            else if(rightEv->position > addEv->position)
            {
                mergedRight[u].push_back(addEv);
                addIt++;
                continue;
            }
            else
            {
                if(rightEv->type < addEv->type)
                {
                    mergedRight[u].push_back(rightEv);
                    evIt++;
                }
                else
                {
                    mergedRight[u].push_back(addEv);
                    addIt++;
                }
            }
        }
    }

    BuildNode(node->left, leftbbox, mergedLeft, leftprimitives, depth+1, badsplits);
    leftprimitives.clear();

    for(int u = 0; u < 3; u++)
    {
        for(auto& e : addleft[u])
            delete e;
        addleft[u].clear();
        mergedLeft[u].clear();
    }

    BuildNode(node->right, rightbbox, mergedRight, rightprimitives, depth+1, badsplits);
    for(int u = 0; u < 3; u++)
        for(auto& e : addright[u])
            delete e;
}

//------------------------------------------------------------------------------
// Builds a KD-Tree from the supplied vector of primitives
//------------------------------------------------------------------------------
void KDTree::Build(vector<const Primitive*> primitives)
{
    m_root = new KDNode();
    m_bbox = CalculateExtents(primitives);
    vector<SAHEvent*> eventlist[3];

    // Loop through each axis - u is the primary axis
    for(int u = 0; u < 3; u++)
    {
        int v = (u + 1) % 3;
        int w = (u + 2) % 3;

        if(g_quitting)
            return;

        // Create event lists from the objects
        for(auto& s : primitives)
        {
            // Get the bounding box of the to this bounding box culled primitive
            BoundingBox clippedbox;
            if(!s->GetClippedBoundingBox(m_bbox, clippedbox))
                continue;
            
            double minpoint = clippedbox.c1[u];
            double maxpoint = clippedbox.c2[u];

            // Planar primitive - insert a planar event into the queue
            if(minpoint == maxpoint)
            {
                SAHEvent* e = new SAHEvent(s, minpoint, SAHEvent::planar);
                eventlist[u].push_back(e);
            }

            else
            {
                SAHEvent* e1 = new SAHEvent(s, minpoint, SAHEvent::start);
                SAHEvent* e2 = new SAHEvent(s, maxpoint, SAHEvent::end);
                eventlist[u].push_back(e1);
                eventlist[u].push_back(e2);
            }
        }

        // Sort the event list
        std::sort(eventlist[u].begin(), eventlist[u].end(), 
            [] (SAHEvent* e1, SAHEvent* e2) -> bool 
        { return e1->position < e2->position;});
    }

    BuildNode(m_root, m_bbox, eventlist, primitives, 0, 3);

    for(int u = 0; u < 3; u++)
        for(auto& e : eventlist[u])
            delete e;
}

IntResult KDNode::IntersectRec(const Ray& ray, double tmin, double tmax, bool returnPrimitive=false) const
{
    if(tmin > tmax)
        return { -inf, nullptr };

    int a = splitdir;
    double locmint = numeric_limits<double>::infinity();
    const Primitive* minprimitive = nullptr;
    if(!left && !right) {
        for(auto& s : m_primitives) {
            double t = s->Intersect(ray);
            if(t >= tmin && t <= tmax) {
                if(!returnPrimitive)
                    return { t, nullptr };
                if(t < locmint)
                    minprimitive = s, locmint = t;
            }
        }        
        return minprimitive ? IntResult { locmint, minprimitive } : IntResult { -inf, nullptr };
    }

    double k = ray.direction[a];
    double tint = (m_splitpos - ray.origin[a])/k;    

    KDNode* nearnode = k > 0 ? left : right;
    KDNode* farnode = nearnode == left ? right : left;

    if(tint <= tmin)
    {
        auto res = farnode->IntersectRec(ray, std::max(tmin, tint-eps), tmax, returnPrimitive);
        if(res.t != -inf)
            return res;
    } else {
        auto res = nearnode->IntersectRec(ray, tmin, std::min(tint+eps, tmax), returnPrimitive);
        if(res.t != -inf)
            return res;

        res = farnode->IntersectRec(ray, std::max(tmin, tint-eps), tmax, returnPrimitive);
        if(res.t != -inf)
            return res;
    }
    return { -inf, nullptr };
}

double KDTree::Intersect(const Ray& ray, const Primitive* &primitive, double tmin, double tmax, bool returnPrimitive=true) const
{
    auto res = m_root->IntersectRec(ray, tmin, tmax, returnPrimitive);
    if(res.t != -inf) {
        primitive = res.primitive;
        return res.t;
    }
    return -inf;
}


#endif