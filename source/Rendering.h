/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Rendering.h
 * 
 * Declaration of the Rendering class.
 */

#pragma once

#define NOMINMAX
#include <memory>
#include <string>
#include <Windows.h>

class ColorBuffer;
class Estimator;
class Renderer;

class Rendering
{
public:
    Rendering(std::shared_ptr<Renderer> renderer, std::shared_ptr<Estimator> estimator);
    Rendering(std::string fileName);

    void Start();
    void Stop();

    void SaveRendering(std::string fileName);

    bool WasBufferRedrawn() const;
    ColorBuffer GetImage();
//private:
    void Thread();

    std::shared_ptr<Renderer> renderer;
    std::shared_ptr<Estimator> estimator;
    ColorBuffer* image;

    unsigned int nSamples;

    HANDLE bufferMutex;

    bool updated;
    bool stopping;
    bool running;
};
