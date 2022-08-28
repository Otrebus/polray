#pragma once

#define NOMINMAX
#include "Texture.h"
#include <memory>
class Renderer;
class Estimator;
class Gfx;
class ColorBuffer;
class Cubemap;

void Draw(Gfx* g, ColorBuffer& cb);
void MakeScene(std::shared_ptr<Renderer>& r, std::shared_ptr<Estimator>& e);
