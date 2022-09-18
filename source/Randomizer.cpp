#include "Randomizer.h"

Randomizer::Randomizer()
{
    std::random_device rd;
    //auto x = rd();
    //seed = x;
    //generator.seed(x);
}

Randomizer::~Randomizer()
{
}

Randomizer::Randomizer(unsigned int seed)
{
    generator.seed(seed);
}

int Randomizer::GetInt(int a, int b)
{
    std::uniform_int_distribution<int> ngen(a, b);
    return ngen(generator);
}

double Randomizer::GetDouble(double a, double b)
{
    std::uniform_real_distribution<double> ngen(a, b);
    return ngen(generator);
}

void Randomizer::Seed(unsigned int aSeed)
{
    generator.seed(aSeed);
}

void Randomizer::Seed()
{
    generator.seed(std::random_device()());
}

std::default_random_engine thread_local Randomizer::generator = std::default_random_engine(std::random_device()());
