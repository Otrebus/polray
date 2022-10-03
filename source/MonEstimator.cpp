#include <algorithm>
#include "MonEstimator.h"
#include "Bytestream.h"
#include "Utils.h"

/**
 * Saves a bucket to a bytestream.
 * 
 * @param stream The bytestream to serialize to.
 */
void Bucket::Save(Bytestream& stream) const
{
    stream << avg << nSamples;
}

/**
 * Loads a bucket from a bytestream.
 * 
 * @param stream The stream to deserialize from.
 */
void Bucket::Load(Bytestream& stream) 
{
    stream >> avg >> nSamples;
}

/**
 * Constructor.
 */
MonEstimator::MonEstimator()
{
}

/**
 * Constructor.
 * 
 * @param x The horizontal size of the buffer.
 * @param y The vertical size of the buffer.
 */
MonEstimator::MonEstimator(int xres, int yres)
{
    buckets = new Bucket[xres*yres*M];
    width = xres;
    height = yres;
    nSamples = new int[xres*yres];
    std::fill(nSamples, nSamples + xres*yres, 0);
}

/**
 * Returns a bucket.
 * 
 * @param x The horizontal component of the bucket.
 * @param y The vertical component of the bucket.
 * @param m The number of the bucket.
 */
Bucket& MonEstimator::GetBucket(int x, int y, int m) const
{
    return buckets[y*(width*M) + x*M + m];
}

/**
 * Adds a sample to a pixel.
 * 
 * @param x The horizontal component of the pixel coordinate.
 * @param y The vertical component of the pixel coordinate.
 */
void MonEstimator::AddSample(int x, int y, const Color& c)
{
    int ns = nSamples[y*width+x];
    auto& bucket = GetBucket(x, y, ns%M);

    bucket.avg = bucket.avg + (c - bucket.avg)/(++bucket.nSamples);
    nSamples[y*width+x]++;
}

/**
 * Returns the current estimate of a pixel color.
 * 
 * @param x The horizontal component of the pixel coordinate.
 * @param y The vertical component of the pixel coordinate.
 */
Color MonEstimator::GetEstimate(int x, int y) const
{
    int ns = nSamples[y*width+x];
    if(ns < M)
    {
        // If we haven't filled all the buckets yet, just do an average of the samples that we have
        auto avg = Color::Black;
        for(int i = 0; i < ns; i++)
            avg += GetBucket(x, y, i).avg/ns;
        return avg;
    }
    else
    {
        // Order the buckets by the average estimator (as pBuckets)
        Bucket* pBuckets[M];
        for(int i = 0; i < M; i++)
            pBuckets[i] = &GetBucket(x, y, i);

        auto fn = [] (Bucket* a, Bucket* b) { return a->avg.GetMax() < b->avg.GetMax(); };
        std::sort(pBuckets, pBuckets+M, fn);

        // Calculate the Gini coefficent
        double nom = 0, denom = 0;
        for(int i = 0; i < M; i++)
            nom += 2*(i+1)*pBuckets[i]->avg.GetMax();  
        for(int i = 0; i < M; i++)
            denom += M*pBuckets[i]->avg.GetMax();

        auto G = nom/denom - double(M+1)/M;
        if(!nom)
            return Color::Black;
        if(std::abs(G) < eps)
            return pBuckets[0]->avg;
        if(G <= 0 || G > 1)
            return Color::Black;

        // Average the medians
        int c = int(G*(M/2));
        Color MoN = Color::Black;
        for(int i = c; i < M-c; i++)
            MoN += pBuckets[i]->avg/(M-2*c);
        return MoN;
    }
}

/**
 * Saves the estimator to a bytestream.
 * 
 * @param stream The bytestream to serialize to.
 */
void MonEstimator::Save(Bytestream& stream) const
{
    stream << ID_MONESTIMATOR << height << width;
    for(int y = 0; y < height; y++)
        for(int x = 0; x < width; x++)
            stream << nSamples[y*width+x];

    for(int y = 0; y < height; y++)
        for(int x = 0; x < width; x++)
            for(int m = 0; m < M; m++)
                GetBucket(x, y, m).Save(stream);
}

/**
 * Loads the estimator from a bytestream.
 * 
 * @param stream The bytestream to deserialize from.
 */
void MonEstimator::Load(Bytestream& stream)
{
    stream >> height >> width;
    nSamples = new int[width*height];
    for(int y = 0; y < height; y++)
        for(int x = 0; x < width; x++)
            stream >> nSamples[y*width+x];

    buckets = new Bucket[width*height*M];
    for(int y = 0; y < height; y++)
        for(int x = 0; x < width; x++)
            for(int m = 0; m < M; m++)
                GetBucket(x, y, m).Load(stream);
}