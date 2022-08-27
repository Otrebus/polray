#pragma once

#define NOMINMAX
#include <random>

class Random
{
public:
    Random();
    Random(unsigned int seed);

    void Seed(unsigned int seed);
    void Seed();
    double GetDouble(double a, double b);
    int GetInt(int a, int b);
    ~Random();

    static thread_local std::default_random_engine generator;
    unsigned int seed;
};
