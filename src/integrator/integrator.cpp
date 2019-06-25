/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/

#include <iostream>

#include "pathtracer.hpp"
#include "smallpt.hpp"
#include "imagerender.hpp"
#include "integrator.hpp"
#include "options.hpp"


Integrator::Integrator(const Options &options, std::shared_ptr<Scene> scene, Logger &logger) : width(options.width), height(options.height), scene(scene)
{}

Integrator::~Integrator()
{
}

std::shared_ptr<Integrator> Integrator::create_integrator(const Options &options, std::shared_ptr<Scene> scene, Logger &logger)
{
    if (options.integrator == "pathtracer")
        return std::shared_ptr<Integrator>(new PathTracer(options, scene, logger));
    else if (options.integrator == "smallpt")
        return std::shared_ptr<Integrator>(new SmallPt(options, scene, logger));
    else // if (options.integrator == "image")
        return std::shared_ptr<Integrator>(new ImageRender(options, scene, logger));
}
