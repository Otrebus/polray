#pragma once

#include "color.h"
#include "Estimator.h"

class MeanEstimator : public Estimator
{
public:
    MeanEstimator();
    MeanEstimator(int xres, int yres);
    void AddSample(int x, int y, const Color& c);
    Color GetEstimate(int x, int y) const;

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);
private:
    int* nSamples;
    Color* samples;
};