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

PathTracer::PathTracer(const Options &options, std::shared_ptr<Scene> scene) : Integrator(options, scene), width64(width), height64(height)
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
    
/*
    // construct image plane vector with sub-pixel jitter
    const double x_disp = (double(x) + rng() * 2.0 / width64) - 1.0;
    const double y_disp = (double(y) + rng() * 2.0 / height64) - 1.0;
    
    // make image plane offset vector
    // by scaling the view definition by the coefficients
    const Camera &camera(scene->camera);
    const Vector offset((right_m * x_disp) + (up_m * y_disp * (height64 / width64)));

    // add image offset vector to view direction
    Vector3f sampleDirection = (viewDirection_m + (offset * ::tan(viewAngle_m * 0.5))).unitize();

    // trace a ray
    const Color radiance = get_radiance(viewPosition_m, sampleDirection, rng));
    return radiance;
*/
    return Color();
}

Color get_radiance()
{
    return Color();
}
