#pragma once

#define NOMINMAX
#include <string>
#include <vector>

class Vector3d;

std::vector<std::string> split(const std::string& s, char delim);
std::string lower(const std::string& s);

const double inf = std::numeric_limits<double>::infinity();
const double pi = 3.141592653589793238462643383279502884;
const double eps = 1e-9;

typedef double AreaPdf;
typedef double AnglePdf;

typedef Vector3d Point;
typedef Vector3d Normal;

// #undef min
// #undef max
#define min min
#define max max

template<typename T> T sgn(T s)
{
    return s > 0 ? 1 : s < 0 ? -1 : 0;
}

template<typename T> T min(T a, T b)
{
    return std::min(a, b);
}

template <typename T, typename ...U> T min(T a, T b, U... c)
{
    return min(min(a, b), c...);
}

template<typename T> T max(T a, T b)
{
    return std::max(a, b);
}

template <typename T, typename ...U> T max(T a, T b, U... c)
{
    return max(max(a, b), c...);
}
