#include "Rendering.h"
#include <intrin.h>
#include <iostream>
#include <thread>


Rendering::Rendering(std::shared_ptr<Renderer> r) : 
    renderer(r), running(false), updated(true), 
    stopping(false), nSamples(0)
{
    int xres = r->GetScene()->GetCamera()->GetXRes();
    int yres = r->GetScene()->GetCamera()->GetYRes();
    image = new ColorBuffer(xres, yres);
    accumulation = new ColorBuffer(xres, yres);
    accumulation->Clear(0);
    image->Clear(0);
    bufferMutex = CreateMutex(0, false, 0);
}

Rendering::Rendering(std::string fileName)
    : running(false), updated(true), stopping(false)
{
    Bytestream b;
    b.LoadFromFile(fileName);
    std::shared_ptr<Scene> scene(new Scene());
    scene->Load(b);
    unsigned int rendererType;
    b >> rendererType >> nSamples;
    switch(rendererType)
    {
    case Renderer::typePathTracer:
        renderer = make_shared<PathTracer>(scene);
        break;
    case Renderer::typeLightTracer:
        renderer = make_shared<LightTracer>(scene);
        break;
    case Renderer::typeBDPT:
        renderer = make_shared<BDPT>(scene);
        break;
    default:
        __debugbreak();
    }
    accumulation = new ColorBuffer(b);
    image = new ColorBuffer(accumulation->GetXRes(), accumulation->GetYRes());
    image->Clear(0);
}

void Rendering::SaveRendering(std::string fileName)
{
    Bytestream b;
    renderer->GetScene()->Save(b);
    b << renderer->GetType() << nSamples;
    accumulation->Save(b);
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
        unsigned int nNewSamples = renderer->GetSPP();
        renderer->Render(*(renderer->GetScene()->GetCamera()), temp);

        if(stopping) // If we were asked to stop rendering, the latest frame 
            break;   // was not rendered entirely and should be discarded
        WaitForSingleObject(bufferMutex, INFINITE);
        for(int y = 0; y < image->GetYRes(); y++)
        {
            for(int x = 0; x < image->GetXRes(); x++)			
            {
                Color k = temp.GetPixel(x, y);
                Color c = accumulation->GetPixel(x, y);

                if(nSamples > 0)
                {   // Calculate the new average image based on the new samples
                    // and the number of old samples (running average)
                    double D = double(nSamples + nNewSamples);
                    double final_r = c.r + nNewSamples*(k.r - c.r)/D;
                    double final_g = c.g + nNewSamples*(k.g - c.g)/D;
                    double final_b = c.b + nNewSamples*(k.b - c.b)/D;
                    accumulation->SetPixel(x, y, final_r, final_g, final_b);
                }
                else
                    accumulation->SetPixel(x, y, k);
            }
        }
        for(int y = 0; y < image->GetYRes(); y++)
        {
            for(int x = 0; x < image->GetXRes(); x++)
            {
                Color c = accumulation->GetPixel(x, y);
                
                double exposure = 0.75;
                c.r = 1 - exp(-exposure*c.r);
                c.g = 1 - exp(-exposure*c.g);
                c.b = 1 - exp(-exposure*c.b);

                image->SetPixel(x, y, c);
            }
        }
        updated = true;
        nSamples += nNewSamples;
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
    auto processor_count = std::thread::hardware_concurrency();
#ifdef _DEBUG
    processor_count = 1;
#endif
    for(int i = 0; i < processor_count; i++)
        _beginthread(Rendering::Thread, 0, (void*)this);
}

void Rendering::Stop()
{
    stopping = true;
    renderer->Stop();
}