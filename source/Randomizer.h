#pragma once

#include <random>

class Randomizer
{
public:
    Randomizer();
    Randomizer(unsigned int seed);

    void Seed(unsigned int seed);
    void Seed();

    double GetDouble(double a, double b);
    int GetInt(int a, int b);

    ~Randomizer();

    static thread_local std::default_random_engine generator;
    unsigned int seed;
};
