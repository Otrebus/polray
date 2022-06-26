#include "LightTree.h"

LightStruct::LightStruct(Light* light, double weight) : light(light), weight(weight)
{
}

LightStruct::~LightStruct()
{
}

LightNode::LightNode(vector<Light*> lights)
{
    vector<LightStruct*> lsLeft;
    vector<LightStruct*> lsRight;
    double total = 0.0f;

    // First, calculate the total sum of weights
    for(auto it = lights.begin(); it < lights.end(); it++)
    {
        Light* light = *it;
        total += 1;
    }

    // Create the lightstructs, and fill them with the normalized weights
    auto it = lights.begin();
    double rightWeight = 0.0f;
    int index = 0;
    for(; it < lights.end(); it++)
    {
        Light* light = *it;
        LightStruct* s = new LightStruct(light, 1);
        lsLeft.push_back(s);
        rightWeight += s->weight;
        if(++index >= lights.size()/2)
            break;
    }
    if(it < lights.end()) // This was commented out earlier but doesn't work with 0 lights - if single lights or multiple lights doesn't work as it should, comment this out again
       it++;
    if(lsLeft.size() > 0)
        left = new LightNode(0, lsLeft);
    else
        left = 0;
    for(; it < lights.end(); it++)
    {
        Light* light = *it;
        LightStruct* s = new LightStruct(light, 1);
        lsRight.push_back(s);
    }
    if(lsRight.size() > 0)
        right = new LightNode(rightWeight, lsRight);
    else
        right = 0;
    val = rightWeight;
}

LightNode::LightNode(double weight, vector<LightStruct*> lightStructs)
{
    if(lightStructs.size() == 1)
    {
        right = 0;
        left = 0;
        light = lightStructs[0]->light;
        val = lightStructs[0]->weight;
        return;
    }
 
    vector<LightStruct*> lsLeft;
    vector<LightStruct*> lsRight;
    auto it = lightStructs.begin();
    double rightWeight = weight;
    int index = 0;
    for(; it < lightStructs.end(); it++)
    {
        LightStruct* s = *it;
        rightWeight += s->weight;
        lsLeft.push_back(s);
        if(++index >= lightStructs.size()/2)
            break;
    }
    for(; it < lightStructs.end(); it++)
        lsRight.push_back(*it);

    left = new LightNode(weight, lsLeft);
    if(lsRight.size() > 0)
        right = new LightNode(weight + rightWeight, lsRight);
    else
        right = 0;
    val = weight + rightWeight;
}

LightNode::~LightNode()
{
}

Light* LightNode::PickLight(double r, double& weight) const
{
    if(!left)
    {
        weight = val;
        return light;
    }
    if(!right || r <= val)
        return left->PickLight(r, weight);
    else
        return right->PickLight(r, weight);
}