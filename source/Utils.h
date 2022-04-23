#pragma once

#include <string>
#include <vector>

std::vector<std::string> split(const std::string& s, char delim);

const auto inf = std::numeric_limits<float>::infinity();
const float eps = 1e-7;

template<typename T> T sgn(T s) {
    return s > 0 ? 1 : s < 0 ? -1 : 0;
}
