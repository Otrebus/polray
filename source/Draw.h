/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Draw.h
 * 
 * Declarations.
 */

#pragma once

#define NOMINMAX
#include <memory>
class Renderer;
class Estimator;
class Gfx;
class ColorBuffer;

void MakeScene(std::shared_ptr<Renderer>& r, std::shared_ptr<Estimator>& e);
