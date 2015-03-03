/*

***********************************************************
* © Milk VFX, inc. All rights reserved. *
***********************************************************

*/


#include "fixedsampler.hpp"
#include "options.hpp"
#include "integrator.hpp"
#include "color.hpp"


FixedSampler::FixedSampler(const Options &options) : 
    Sampler(options),
    num_samples(1<<options.max_samples)
{ 
}

FixedSampler::~FixedSampler()
{}

Color FixedSampler::render(Integrator *integrator, Scene *scene, size_t x, size_t y)
{
    Color color;
    for (size_t sample=0; sample<num_samples; ++sample)
    {
        color += integrator->render(scene, x, y, sample);
    }
    color /= float(num_samples);
    return color;
}
