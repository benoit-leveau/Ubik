/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#include "pathtracer.hpp"
#include "color.hpp"
#include "scene.hpp"
#include "rng.hpp"
#include "options.hpp"

#include <chrono>
#include <unistd.h>

PathTracer::PathTracer(const Options &options) : Integrator(options)
{
}

PathTracer::~PathTracer()
{
}

Color PathTracer::render(Scene *scene, size_t x, size_t y, size_t sample)
{
    // initialize the RNG in a deterministic way for each sample
    RNG rng;
    rng.seed(y*y*y + x*x + scene->frame + sample);

    // trace a ray
    // ...
}
