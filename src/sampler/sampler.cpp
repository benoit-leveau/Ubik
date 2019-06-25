/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/


#include "sampler.hpp"
#include "options.hpp"
#include "fixedsampler.hpp"
#include "adaptativesampler.hpp"

Sampler::Sampler(const Options &/*options*/, std::shared_ptr<Integrator> integrator, Logger &logger) :
    integrator(integrator)
{}

std::shared_ptr<Sampler> Sampler::create_sampler(const Options &options, /*std::shared_ptr<Scene> scene, */std::shared_ptr<Integrator> integrator, Logger &logger)
{
    if (options.sampler == "fixed")
        return std::shared_ptr<Sampler>(new FixedSampler(options, integrator, logger));
    else // if (options.sampler == "adaptative")
        return std::shared_ptr<Sampler>(new AdaptativeSampler(options, integrator, logger));
}
