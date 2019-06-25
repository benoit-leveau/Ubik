/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef ADAPTATIVESAMPLER_HPP
#define ADAPTATIVESAMPLER_HPP

#include <cstddef>
#include <memory>
#include "sampler.hpp"
#include "OptionParser.h"

// forward declaration
class Options;
class Color;
class Integrator;
class Scene;
//namespace optparse{
//class OptionParser;
//}

class AdaptativeSampler : public Sampler
{
public:
    AdaptativeSampler(const Options &options, std::shared_ptr<Integrator> integrator, Logger &logger);
    ~AdaptativeSampler();

    virtual Color render(size_t x, size_t y);

    static void createOptions(optparse::OptionParser &parser);
private:
    size_t min_samples;
    size_t max_samples;
    double threshold;
};

#endif // !ADAPTATIVESAMPLER_HPP
