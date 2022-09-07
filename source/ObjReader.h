#pragma once

#define NOMINMAX
#include <string>
#include <tuple>
#include <vector>
#include <map>

class Material;
class TriangleMesh;
class MeshLight;

std::tuple<bool, TriangleMesh*, std::vector<MeshLight*>> ReadFromFile(const std::string& file, Material* meshMat);
bool ReadMaterialFile(const std::string& matfilestr, std::map<std::string, Material*>& materials);
