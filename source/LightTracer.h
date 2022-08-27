#pragma once

#define NOMINMAX
#include "Camera.h"
#include "ColorBuffer.h"
#include "Primitive.h"
#include "Renderer.h"
#include "Random.h"

class LightTracer : public Renderer
{
public:
    LightTracer(std::shared_ptr<Scene> scene);
    ~LightTracer();

    void Render(Camera& cam, ColorBuffer& colBuf);
    void RenderPart(Camera& cam, ColorBuffer& colBuf) const;
    
    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);
private:
    mutable Random m_random;
};
