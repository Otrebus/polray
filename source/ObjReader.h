#pragma once

#define NOMINMAX
#include <string>
#include <tuple>
#include <vector>
#include <map>

class Material;
class TriangleMesh;
class MeshLight;

std::pair<TriangleMesh*, std::vector<MeshLight*>> ReadFromFile(const std::string& file, Material* meshMat);
std::map<std::string, Material*> ReadMaterialFile(const std::string& matfilestr);
