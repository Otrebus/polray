#pragma once

#include "gfx.h"
#include "Estimator.h"
#include "Texture.h"
#include "Cubemap.h"
#include "kdtree.h"
#include "ColorBuffer.h"
#include "Scene.h"
#include "Renderer.h"

void Draw(Gfx* g, ColorBuffer& cb);
void MakeScene(std::shared_ptr<Renderer>& r, std::shared_ptr<Estimator>& e);
//extern int* ptr;
extern Texture* test;
extern Cubemap* cubemap;
extern KDTree* tree;
extern Texture* normalmap;
