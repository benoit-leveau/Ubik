/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/


#include "fixedsampler.hpp"
#include "options.hpp"
#include "integrator.hpp"
#include "radiance.hpp"
#include "color.hpp"


FixedSampler::FixedSampler(const Options &options, std::shared_ptr<Integrator> integrator) : 
    Sampler(options, integrator),
    num_samples(1<<options.max_samples)
{ 
}

FixedSampler::~FixedSampler()
{
}

Color FixedSampler::render(size_t x, size_t y)
{
    Radiance radiance;
    for (size_t sample=0; sample<num_samples; ++sample)
    {
        radiance += integrator->render(x, y, sample);
    }
    return Color(radiance / double(num_samples));
}
}
