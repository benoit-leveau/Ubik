/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/


#include "rng.hpp"

RNG::RNG() : distribution(0.0, 1.0)
{}

RNG::~RNG()
{}

void RNG::seed(uint_fast32_t seed)
{
    generator.seed(seed);
}

double RNG::operator()()
{
    return distribution(generator);
}
