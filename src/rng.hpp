/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/

#ifndef RNG_H
#define RNG_H

#include <random>


class RNG
{
public:
    RNG();
    ~RNG();

    void seed(uint_fast32_t seed);

    double operator()();
private:
    std::mt19937 generator;
    std::uniform_real_distribution<> distribution;
};

#endif // !RNG_H
