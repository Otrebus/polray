#pragma once

#include "Color.h"
#include "Estimator.h"

const int M = 21;

class MonEstimator;

class Bucket {
public:
    Bucket() : avg(Color::Black), nSamples(0) { }
    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    friend class MonEstimator;
private:
    Color avg;
    int nSamples;
};

class MonEstimator : public Estimator
{
public:
    MonEstimator();
    MonEstimator(int xres, int yres);
    void AddSample(int x, int y, const Color& c);
    Color GetEstimate(int x, int y) const;
    Bucket& GetBucket(int x, int y, int m) const;

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);
private:
    Bucket* buckets;
    int* nSamples;
};
