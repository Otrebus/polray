#include "Rendering.h"
#include "MonEstimator.h"
#include <intrin.h>
#include <iostream>
#include <thread>


Rendering::Rendering(std::shared_ptr<Renderer> r, std::shared_ptr<Estimator> e) : 
    renderer(r), estimator(e), running(false), updated(true), 
    stopping(false), nSamples(0)
{
    int xres = r->GetScene()->GetCamera()->GetXRes();
    int yres = r->GetScene()->GetCamera()->GetYRes();
    image = new ColorBuffer(xres, yres);
    image->Clear(Color::Black);
    bufferMutex = CreateMutex(0, false, 0);
}

Rendering::Rendering(std::string fileName)
    : running(false), updated(true), stopping(false)
{
    Bytestream b;
    b.LoadFromFile(fileName);
    std::shared_ptr<Scene> scene(new Scene());
    scene->Load(b);
    unsigned char rendererType, estimatorType;
    b >> rendererType;
    renderer = std::shared_ptr<Renderer>(Renderer::Create(rendererType, scene));

    b >> estimatorType;
    estimator = std::shared_ptr<Estimator>(Estimator::Create(estimatorType));
    estimator->Load(b);

    image = new ColorBuffer(estimator->GetWidth(), estimator->GetHeight());
    image->Clear(Color::Black);
}
 
void Rendering::SaveRendering(std::string fileName)
{
    Bytestream b;
    renderer->GetScene()->Save(b);
    renderer->Save(b);
    estimator->Save(b);
    b.SaveToFile(fileName);
}

bool Rendering::WasBufferRedrawn() const
{
    return updated;
}

ColorBuffer Rendering::GetImage()
{
    WaitForSingleObject(bufferMutex, INFINITE);
    updated = false;
    ColorBuffer tmp = *image;
    ReleaseMutex(bufferMutex);
    return tmp;
}

void Rendering::Thread()
{
    while(true)
    {
        ColorBuffer temp(image->GetXRes(), image->GetYRes(), Color::Black);
        renderer->Render(*(renderer->GetScene()->GetCamera()), temp);

        if(stopping) // If we were asked to stop rendering, the latest frame 
            break;   // was not rendered entirely and should be discarded
        WaitForSingleObject(bufferMutex, INFINITE);
        for(int y = 0; y < image->GetYRes(); y++)
        {
            for(int x = 0; x < image->GetXRes(); x++)			
            {
                //if(y == YRES/2 && x == XRES/2)
                //    __debugbreak();
                estimator->AddSample(x, y, temp.GetPixel(x, y));
            }
        }
        for(int y = 0; y < image->GetYRes(); y++)
        {
            for(int x = 0; x < image->GetXRes(); x++)
            {
                //if(y == YRES/2 && x == XRES/2)
                //    __debugbreak();
                Color c = estimator->GetEstimate(x, y);
                
                double exposure = 0.75;
                c.r = 1 - exp(-exposure*c.r);
                c.g = 1 - exp(-exposure*c.g);
                c.b = 1 - exp(-exposure*c.b);

                image->SetPixel(x, y, c);
            }
        }
        updated = true;
        nSamples ++;
        ReleaseMutex(bufferMutex);
    }
}

void Rendering::Thread(void* r)
{
    ((Rendering*) r)->Thread();
}

void Rendering::Start()
{
    assert(!running);
    running = true;
    stopping = false;
    auto processorCount = std::thread::hardware_concurrency();
#ifdef _DEBUG
    processorCount = 1;
#endif

    for(unsigned int i = 0; i < processorCount; i++)
        _beginthread(Rendering::Thread, 0, (void*)this);
}

void Rendering::Stop()
{
    stopping = true;
    renderer->Stop();
}