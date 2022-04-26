#ifndef LIGHTTREE_H
#define LIGHTTREE_H
#include <vector>
#include "Light.h"
#include "Random.h"

using namespace std;

class LightStruct
{
public:
    LightStruct(Light* light, double weight);
    ~LightStruct();
    Light* light;
    double weight;
};

class LightNode
{
public:
    LightNode(vector<Light*> lights);
    ~LightNode();
    
    Light* PickLight(double r, double& weight) const;

private:
    LightNode(double weight, vector<LightStruct*> lightStructs);

    LightNode *left, *right;
    Light* light;
    double val;
};

#endif