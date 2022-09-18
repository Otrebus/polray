#pragma once

#include "Renderer.h"
#include "Randomizer.h"

class Primitive;
class ColorBuffer;
class Camera;

class LightTracer : public Renderer
{
public:
    LightTracer(std::shared_ptr<Scene> scene);
    ~LightTracer();

    void Render(Camera& cam, ColorBuffer& colBuf);
    
    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);
private:
    Randomizer m_random;
};
