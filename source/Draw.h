#pragma once

#define NOMINMAX
#include <memory>
class Renderer;
class Estimator;
class Gfx;
class ColorBuffer;

void MakeScene(std::shared_ptr<Renderer>& r, std::shared_ptr<Estimator>& e);
