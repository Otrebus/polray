#include <algorithm>
#include "MeanEstimator.h"
#include "Main.h"


MeanEstimator::MeanEstimator() {
}

MeanEstimator::MeanEstimator(int xres, int yres) {
    nSamples = new int[xres*yres];
    samples = new Color[xres*yres];
    width = xres;
    height = yres;
    std::fill(nSamples, nSamples + xres*yres, 0);
    std::fill(samples, samples + xres*yres, Color::Black);
}

void MeanEstimator::AddSample(int x, int y, const Color& c) {
    int& ns = nSamples[y*width+x];
    Color& k = samples[y*width+x];

    k += (c - k)/(++ns);
}

Color MeanEstimator::GetEstimate(int x, int y) const {
    return samples[y*width+x];
}

void MeanEstimator::Save(Bytestream& stream) const {
    stream << ID_MEANESTIMATOR << height << width;
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            stream << nSamples[y*width+x];
        }
    }
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            stream << samples[y*width+x];
        }
    }
}

void MeanEstimator::Load(Bytestream& stream) {
    stream >> height >> width;
    nSamples = new int[width*height];
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            stream >> nSamples[y*width+x];
        }
    }
    samples = new Color[width*height];
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            stream >> samples[y*width+x];
        }
    }
}