/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/


#include "renderer.hpp"
#include "interactiverenderer.hpp"
#include "tiledrenderer.hpp"
#include "options.hpp"
#include "fixedsampler.hpp"
#include "pathtracer.hpp"
#include "imagerender.hpp"

#include <iostream>
#include <memory>

Renderer::Renderer(std::shared_ptr<Scene> scene, const Options &options, Logger &logger) : 
    scene(scene),
    sampler(nullptr),
    integrator(nullptr),
    width(options.width),
    height(options.height),
    nbthreads(options.nbthreads),
    logger(logger)
{
    if (options.pathtracer)
        integrator = std::shared_ptr<Integrator>(new PathTracer(options, scene));
    else
        integrator = std::shared_ptr<Integrator>(new ImageRender(options, scene));
    if (!options.interactive && options.fixedsampling)
        sampler = std::shared_ptr<Sampler>(new FixedSampler(options, integrator));
}

Renderer::~Renderer()
{
}

std::unique_ptr<Renderer> Renderer::create_renderer(std::shared_ptr<Scene> scene, const Options &options, Logger &logger)
{
    if (options.interactive)
        return std::unique_ptr<Renderer>(new InteractiveRenderer(scene, options, logger));
    else
        return std::unique_ptr<Renderer>(new TiledRenderer(scene, options, logger));
}
