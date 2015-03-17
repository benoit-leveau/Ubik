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

PathTracer::PathTracer(const Options &options, std::shared_ptr<Scene> scene) : Integrator(options, scene)
{
}

PathTracer::~PathTracer()
{
}

Color PathTracer::render(size_t x, size_t y, size_t sample)
{
    // initialize the RNG in a deterministic way for each sample
    RNG rng;
    rng.seed(y*y*y + x*x + scene->frame + sample);

    // trace a ray
    // ...
    return Color();
}
