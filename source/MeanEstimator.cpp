#include <algorithm>
#include "MeanEstimator.h"
#include "Bytestream.h"
#include "Main.h"


MeanEstimator::MeanEstimator()
{
}

/**
 * Constructs a mean estimator.
 * 
 * @param xres The horizontal resolution of the estimator.
 * @param yres The vertical resolution of the estimator
 */
MeanEstimator::MeanEstimator(int xres, int yres)
{
    nSamples = new int[xres*yres];
    samples = new Color[xres*yres];
    width = xres;
    height = yres;
    std::fill(nSamples, nSamples + xres*yres, 0);
    std::fill(samples, samples + xres*yres, Color::Black);
}

/**
 * Adds a sample to the estimator.
 * 
 * @param x The x-coordinate of the sample.
 * @param y The y-coordinate of the sample
 * @param c The sample to add.
 */
void MeanEstimator::AddSample(int x, int y, const Color& c)
{
    int& ns = nSamples[y*width+x];
    Color& k = samples[y*width+x];

    k += (c - k)/(++ns);
}

/**
 * Returns the current estimate at a pixel.
 * 
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 */
Color MeanEstimator::GetEstimate(int x, int y) const
{
    return samples[y*width+x];
}

/**
 * Saves the estimator to a bytestream.
 * 
 * @param stream The bytestream to serialize to.
 */
void MeanEstimator::Save(Bytestream& stream) const
{
    stream << ID_MEANESTIMATOR << height << width;
    for(int y = 0; y < height; y++)
        for(int x = 0; x < width; x++)
            stream << nSamples[y*width+x];

    for(int y = 0; y < height; y++)
        for(int x = 0; x < width; x++)
            stream << samples[y*width+x];
}

/**
 * Loads the estimator from a bytestream.
 * 
 * @param stream The bytestream to deserialize from.
 */
void MeanEstimator::Load(Bytestream& stream)
{
    stream >> height >> width;
    nSamples = new int[width*height];
    for(int y = 0; y < height; y++)
        for(int x = 0; x < width; x++)
            stream >> nSamples[y*width+x];

    samples = new Color[width*height];
    for(int y = 0; y < height; y++)
        for(int x = 0; x < width; x++)
            stream >> samples[y*width+x];
}