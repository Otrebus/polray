/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Utils.cpp
 * 
 * Implementation of various utility functions.
 */

#include "Utils.h"
#include "Vector3d.h"
#include <algorithm>

/**
 * String splitting. Returns the components of the given string split by the given delimiter.
 * 
 * @param s The string to split
 * @param delim The delimiter
 * @returns A vector of the intermediate strings between the delimiters.
 */
std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> ret;
    for(size_t i = 0, i2 = 0; i <= s.size(); i = i2 + 1)
    {
        i2 = s.find(delim, i);
        i2 = i2 == -1 ? s.size() : i2;
        ret.push_back(s.substr(i, i2 - i));
    }
    return ret;
}

/**
 * Returns the lowercase version of the string passed
 * 
 * @param s The input string.
 * @returns The input string in lower-case.
 */
std::string lower(const std::string& s)
{
    std::string ret(s.begin(), s.end());
    std::transform(ret.begin(), ret.end(), ret.begin(), [](char a) { return (char) tolower(a); });
    return ret;
}