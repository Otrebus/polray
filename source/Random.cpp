#include "Random.h"
#include "Logger.h"

Random::Random()
{
    std::random_device rd;
    auto x = rd();
    seed = x;
    generator.seed(x);
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

void Random::Seed()
{
    generator.seed(std::random_device()());
}

std::default_random_engine thread_local Random::generator = std::default_random_engine(std::random_device()());
