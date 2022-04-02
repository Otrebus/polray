#ifndef DRAW_H
#define DRAW_H

#include "gfx.h"
#include "Texture.h"
#include "Cubemap.h"
#include "kdtree.h"
#include "ColorBuffer.h"
#include "Scene.h"
#include "Renderer.h"

void Draw(Gfx* g, ColorBuffer& cb);
void MakeScene(std::shared_ptr<Renderer>& r);
//extern int* ptr;
extern Texture* test;
extern Cubemap* cubemap;
extern KDTree* tree;
extern Texture* normalmap;
extern Texture* bl;

#endif