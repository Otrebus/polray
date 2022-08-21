#include <algorithm>
#include "MonEstimator.h"
#include "Main.h"

void Bucket::Save(Bytestream& stream) const {
    stream << avg << nSamples;
}
void Bucket::Load(Bytestream& stream) {
    stream >> avg >> nSamples;
}

MonEstimator::MonEstimator() {
}

MonEstimator::MonEstimator(int xres, int yres) {
    buckets = new Bucket[xres*yres*M];
    width = xres;
    height = yres;
    nSamples = new int[xres*yres];
    std::fill(nSamples, nSamples + xres*yres, 0);
}

Bucket& MonEstimator::GetBucket(int x, int y, int m) const {
    return buckets[y*(width*M) + x*M + m];
}

void MonEstimator::AddSample(int x, int y, const Color& c) {
    int ns = nSamples[y*width+x];
    auto& bucket = GetBucket(x, y, ns%M);

    bucket.avg = bucket.avg + (c - bucket.avg)/(++bucket.nSamples);
    nSamples[y*width+x]++;
}

Color MonEstimator::GetEstimate(int x, int y) const {
    int ns = nSamples[y*width+x];
    if(ns < M) {
        // If we haven't filled all the buckets yet, just do an average of the samples that we have
        auto avg = Color::Black;
        for(int i = 0; i < ns; i++)
            avg += GetBucket(x, y, i).avg/ns;
        return avg;
    } else {
        // Order the buckets by the average estimator (as pBuckets)
        Bucket* pBuckets[M];
        for(int i = 0; i < M; i++)
            pBuckets[i] = &GetBucket(x, y, i);
        std::sort(pBuckets, pBuckets+M, [] (Bucket* a, Bucket* b) { return a->avg.GetMax() < b->avg.GetMax(); });

        // Calculate the Gini coefficent
        double nom = 0, denom = 0;
        for(int i = 0; i < M; i++)
            nom += 2*(i+1)*pBuckets[i]->avg.GetMax();  
        for(int i = 0; i < M; i++)
            denom += M*pBuckets[i]->avg.GetMax();

        auto G = nom/denom - double(M+1)/M;
        if(!nom)
            return Color::Black;
        if(std::abs(G) < 1e-9)
            return pBuckets[0]->avg;
        if(!(G > 0 && G <= 1)) {
            logger.Box(std::to_string(G));
            return Color::Black;
        }

        // Average the medians
        int c = (G*(M/2));
        Color MoN = Color::Black;
        for(int i = c; i < M-c; i++)
            MoN += pBuckets[i]->avg/(M-2*c);
        return MoN;
    }
}

void MonEstimator::Save(Bytestream& stream) const {
    stream << ID_MONESTIMATOR << height << width;
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < height; x++) {
            stream << nSamples[y*width+x];
        }
    }
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < height; x++) {
            for(int m = 0; m < M; m++) {
                GetBucket(x, y, m).Save(stream);
            }
        }
    }
}

void MonEstimator::Load(Bytestream& stream) {
    stream >> height >> width;
    nSamples = new int[width*height];
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < height; x++) {
            stream >> nSamples[y*width+x];
        }
    }
    buckets = new Bucket[width*height*M];
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < height; x++) {
            for(int m = 0; m < M; m++) {
                GetBucket(x, y, m).Load(stream);
            }
        }
    }
}