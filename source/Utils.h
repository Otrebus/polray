#pragma once

#include <string>
#include <vector>
#include "Vector3d.h"

std::vector<std::string> split(const std::string& s, char delim);

const double inf = std::numeric_limits<double>::infinity();
const double pi = 3.141592653589793238462643383279502884;
const double eps = 1e-9;

typedef double AreaPdf;
typedef double AnglePdf;

typedef Vector3d Point;
typedef Vector3d Normal;

template<typename T> T sgn(T s)
{
    return s > 0 ? 1 : s < 0 ? -1 : 0;
}
