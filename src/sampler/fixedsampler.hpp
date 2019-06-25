/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef FIXEDSAMPLER_HPP
#define FIXEDSAMPLER_HPP

#include <cstddef>
#include <memory>
#include "sampler.hpp"
#include "OptionParser.h"

// forward declaration
class Options;
class Color;
class Integrator;
class Scene;


class FixedSampler : public Sampler
{
public:
FixedSampler(const Options &options, std::shared_ptr<Integrator> integrator, Logger &logger);
    ~FixedSampler();

    virtual Color render(size_t x, size_t y);
    
    static void createOptions(optparse::OptionParser &parser);
private:
    size_t num_samples;
};

#endif // !FIXEDSAMPLER_HPP
