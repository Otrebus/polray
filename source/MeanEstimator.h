/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file MeanEstimator.h
 * 
 * Declaration of the MeanEstimator class.
 */

#pragma once

#include "Color.h"
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
