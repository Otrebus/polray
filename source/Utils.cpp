#include "Utils.h"
#include "Vector3d.h"
#include <algorithm>

std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> ret;
    for (size_t i = 0, i2 = 0; i <= s.size(); i = i2 + 1)
    {
        i2 = s.find(delim, i);
        i2 = i2 == -1 ? s.size() : i2;
        ret.push_back(s.substr(i, i2 - i));
    }
    return ret;
}

std::string lower(const std::string& s)
{
    std::string ret(s.begin(), s.end());
    std::transform(ret.begin(), ret.end(), ret.begin(), [](char a) { return (char) tolower(a); });
    return ret;
}