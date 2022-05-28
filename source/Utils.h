#pragma once

#include <string>
#include <vector>

std::vector<std::string> split(const std::string& s, char delim);

const auto inf = std::numeric_limits<double>::infinity();
const double eps = 1e-9;

template<typename T> T sgn(T s) {
    return s > 0 ? 1 : s < 0 ? -1 : 0;
}

std::vector<Vector2d> convexHull(std::vector<Vector2d> v);
