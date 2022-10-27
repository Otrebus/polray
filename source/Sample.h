/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Sample.h
 * 
 * Declaration of the Sample class.
 */

#pragma once

#include "Color.h"
#include "Ray.h"

class Sample {
public:
    Sample();
    Sample(const Color&, const Ray&, double pdf, double rpdf, bool specular, int component);

    Color color;
    Ray outRay;
    double pdf, rpdf;
    bool specular;
    int component;
};
