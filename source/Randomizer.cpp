#include "Randomizer.h"

/**
 * Constructor.
 */
Randomizer::Randomizer()
{
    std::random_device rd;
}

/**
 * Constructor.
 */
Randomizer::~Randomizer()
{
}

/**
 * Constructor. Seeds the random number generator with a given seed.
 * 
 * @param seed The integer to seed the generator with.
 */
Randomizer::Randomizer(unsigned int seed)
{
    generator.seed(seed);
}

/**
 * Returns an integer between a and b, inclusive.
 * 
 * @param a The lower limit of the range to pick from.
 * @param b The upper limit of the range to pick from.
 */
int Randomizer::GetInt(int a, int b)
{
    std::uniform_int_distribution<int> ngen(a, b);
    return ngen(generator);
}

/**
 * Returns a real number between a and b.
 * 
 * @param a The lower limit of the range to pick from.
 * @param b The upper limit of the range to pick from.
 */
double Randomizer::GetDouble(double a, double b)
{
    std::uniform_real_distribution<double> ngen(a, b);
    return ngen(generator);
}

/**
 * Seeds the randomizer with a given seed.
 * 
 * @param aSeed The number to seed with.
 */
void Randomizer::Seed(unsigned int aSeed)
{
    generator.seed(aSeed);
}

/**
 * Seeds the randomizer.
 */
void Randomizer::Seed()
{
    generator.seed(std::random_device()());
}

std::default_random_engine thread_local Randomizer::generator = std::default_random_engine(std::random_device()());
