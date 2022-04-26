#pragma once

#include "Color.h"
#include "Ray.h"

class Sample {
public:
    Sample();
    Sample(const Color&, const Ray&, double pdf, double rpdf, bool specular);

    Color color;
    Ray outRay;
    double pdf, rpdf;
    bool specular;
};