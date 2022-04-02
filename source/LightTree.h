#ifndef LIGHTTREE_H
#define LIGHTTREE_H
#include <vector>
#include "Light.h"
#include "Random.h"

using namespace std;

class LightStruct
{
public:
    LightStruct(Light* light, float weight);
    ~LightStruct();
    Light* light;
    float weight;
};

class LightNode
{
public:
    LightNode(vector<Light*> lights);
    ~LightNode();
    
    Light* PickLight(float r, float& weight) const;

private:
    LightNode(float weight, vector<LightStruct*> lightStructs);

    LightNode *left, *right;
    Light* light;
    float val;
};

#endif