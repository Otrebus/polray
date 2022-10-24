/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file ObjReader.h
 * 
 * Declarations of functions used to read .obj files.
 */

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
