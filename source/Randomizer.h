/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Randomizer.h
 * 
 * Declaration of the Randomizer class.
 */

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
