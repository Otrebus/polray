#include "Random.h"

Random::Random()
{
    std::random_device rd;
    generator.seed(rd());
}

Random::~Random()
{
}

Random::Random(unsigned int seed)
{
    generator.seed(seed);
}

int Random::GetInt(int a, int b)
{
    std::uniform_int_distribution<int> ngen(a, b);
    return ngen(generator);
}

double Random::GetDouble(double a, double b)
{
    std::uniform_real_distribution<double> ngen(a, b);
    return ngen(generator);
}

void Random::Seed(unsigned int seed)
{
    generator.seed(seed);
}

std::default_random_engine thread_local Random::generator = std::default_random_engine();
