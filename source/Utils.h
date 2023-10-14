/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Utils.h
 * 
 * Declarations and definitions of various utility functions.
 */

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

/**
 * Returns the signum of the argument.
 * 
 * @param a The arguments.
 * @returns The signum of the argument.
 */
template<typename T> T sgn(T s)
{
    return s > 0 ? 1 : s < 0 ? -1 : 0;
}

/**
 * Returns the minimum of the given arguments.
 * 
 * @param a, b The arguments.
 * @returns The minimum of the given arguments.
 */
template<typename T> T min(T a, T b)
{
    return std::min(a, b);
}

/**
 * Returns the minimum of the given arguments.
 * 
 * @param a, b, ... The arguments.
 * @returns The minimum of the given arguments.
 */
template <typename T, typename ...U> T min(T a, T b, U... c)
{
    return min(min(a, b), c...);
}

/**
 * Returns the maximum of the given arguments.
 * 
 * @param a, b The arguments.
 * @returns The maximum of the given arguments.
 */
template<typename T> T max(T a, T b)
{
    return std::max(a, b);
}

/**
 * Returns the maximum of the given arguments.
 * 
 * @param a, b, ... The arguments
 * @returns The maximum of the given arguments.
 */
template <typename T, typename ...U> T max(T a, T b, U... c)
{
    return max(max(a, b), c...);
}
