#pragma once

#include <string>
#include <map>
#include "Material.h"
#include "TriangleMesh.h"
#include <tuple>

std::tuple<bool, TriangleMesh, std::vector<MeshLight*>> ReadFromFile(std::string file, Material* meshMat);
bool ReadMaterialFile(std::string matfilestr, std::map<std::string, Material*>& materials);
