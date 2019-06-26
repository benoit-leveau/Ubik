/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/


#include "adaptativesampler.hpp"
#include "options.hpp"
#include "integrator.hpp"
#include "color.hpp"
#include "radiance.hpp"
#include "OptionParser.h"


AdaptativeSampler::AdaptativeSampler(const Options &options, std::shared_ptr<Integrator> integrator, Logger &logger) : 
    Sampler(options, integrator, logger),
    min_samples(1<<options.min_samples),
    max_samples(1<<options.max_samples),
    threshold(options.threshold)
{ 
    logger.log("AdaptativeSampler: min_samples(" + std::to_string(min_samples) + ") max_samples(" + std::to_string(max_samples) + ") threshold(" + std::to_string(threshold) + ")", INFO);
}

AdaptativeSampler::~AdaptativeSampler()
{
}

Color AdaptativeSampler::render(size_t x, size_t y)
{
    size_t sample = 0;
    Radiance radiance;
    Radiance variation;
    unsigned short Xi[3]={0,0,y*y*y+x*x};
    for (sample=0; sample<max_samples; ++sample)
    {
        Radiance new_radiance = integrator->render(x, y, sample, Xi);
        if (sample >= min_samples)
        {
            Radiance variation = (radiance/float(sample)) - ((radiance+new_radiance)/float(sample+1));
            if ((variation.r > threshold) && (variation.g > threshold) && (variation.b > threshold)){
                radiance += new_radiance;
                break;
            }
        }
        radiance += new_radiance;
    }
    return Color(radiance / double(sample));
}

void AdaptativeSampler::createOptions(optparse::OptionParser &parser)
{
    // optparse::OptionGroup group = optparse::OptionGroup(parser, "Adaptative Sampler");
    parser.add_option("--min-samples").action("store").type("int").set_default(0).help("Sets the minimum sampling rate.  default: %default");
    parser.add_option("--max-samples").action("store").type("int").set_default(1).help("Sets the maximum sampling rate.  default: %default");
    parser.add_option("--threshold").action("store").type("double").set_default(0.01).help("Sets the adaptative sampling threshold.  default: %default");
  // parser.add_option_group(group);
}
