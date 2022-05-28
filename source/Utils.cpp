#include "Utils.h"
#include "Vector3d.h"
#include <algorithm>

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> ret;
    for (int i = 0, i2 = 0; i <= s.size(); i = i2 + 1) {
        i2 = s.find(delim, i);
        i2 = i2 == -1 ? s.size() : i2;
        ret.push_back(s.substr(i, i2 - i));
    }
    return ret;
}

bool turnsRight(Vector2d a, Vector2d b, Vector2d c) {
    return (b.x-a.x)*(c.y-a.y) - (c.x-a.x)*(b.y-a.y) < 0;
}

std::vector<Vector2d> convexHull(std::vector<Vector2d> v) {
    std::vector<Vector2d> ps, ps2, ps3; // Partial and full convex hulls and the output

    std::sort(v.begin(), v.end());

    for(auto it = v.begin(); it < v.end(); ps.push_back(*it), it++) // Lower hull part
        for(int i = ps.size() - 2; i >= 0 && turnsRight(ps[i], ps[i+1], *it); i--)
            ps.pop_back();

    for(auto it = v.rbegin(); it < v.rend(); ps2.push_back(*it), it++) // Upper hull part
        for(int i = ps2.size() - 2; i >= 0 && turnsRight(ps2[i], ps2[i+1], *it); i--)
            ps2.pop_back();

    // Merge the convex hull parts
    std::merge(ps.begin(), ps.end(), ps2.rbegin(), ps2.rend(), std::back_inserter(ps3));
    return ps3;
}
