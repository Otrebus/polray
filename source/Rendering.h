#ifndef RENDERING_H
#define RENDERING_H

#include "Bytestream.h"
#include "ColorBuffer.h"
#include "Scene.h"
#include <process.h>
#include "Estimator.h"


class Rendering
{
public:
    Rendering(std::shared_ptr<Renderer> renderer);
    Rendering(std::string fileName);

    void Start();
    void Stop();

    void SaveRendering(std::string fileName);

    bool WasBufferRedrawn() const;
    ColorBuffer GetImage();
//private:
    void Thread();
    static void __cdecl Thread(void*);

    std::shared_ptr<Renderer> renderer;

    Estimator* estimator;
    ColorBuffer* image;

    unsigned int nSamples;

    HANDLE bufferMutex;

    bool updated;
    bool stopping;
    bool running;
};

#endif