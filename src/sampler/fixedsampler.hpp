/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef FIXEDSAMPLER_HPP
#define FIXEDSAMPLER_HPP

#include <cstddef>
#include "sampler.hpp"

// forward declaration
class Options;
class Color;
class Integrator;
class Scene;


class FixedSampler : public Sampler
{
public:
    FixedSampler(const Options &options);
    ~FixedSampler();

    virtual Color render(Integrator *integrator, Scene *scene, size_t x, size_t y);
private:
    size_t num_samples;
};

#endif // !FIXEDSAMPLER_HPP
