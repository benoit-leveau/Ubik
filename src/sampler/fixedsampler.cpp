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
#include "OptionParser.h"

FixedSampler::FixedSampler(const Options &options, std::shared_ptr<Integrator> integrator, Logger &logger) : 
    Sampler(options, integrator, logger),
    num_samples(1<<options.samples)
{
    logger.log("FixedSampler: samples(" + std::to_string(num_samples) + ")", INFO);
}

FixedSampler::~FixedSampler()
{
}

Color FixedSampler::render(size_t x, size_t y)
{
    Radiance radiance;
    unsigned short Xi[3]={0,0,y*y*y+x*x};
    for (size_t sample=0; sample<num_samples; ++sample)
    {
        radiance += integrator->render(x, y, sample, Xi);
    }
    return Color(radiance / double(num_samples));
}

void FixedSampler::createOptions(optparse::OptionParser &parser)
{
    //optparse::OptionGroup group = optparse::OptionGroup(parser, "Fixed Sampler");
    parser.add_option("--samples").action("store").type("int").set_default(1).help("Sets the sampling rate.  default: %default");
    //parser.add_option_group(group);
}
