#pragma once

#include "Color.h"
#include "Ray.h"

class Sample {
public:
    Sample(const Color&, const Ray&, float pdf, float rpdf, bool specular);

    Color color;
    Ray outRay;
    float pdf, rpdf;
    bool specular;
};