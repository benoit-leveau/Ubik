/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/


#include "renderer.hpp"
#include "interactiverenderer.hpp"
#include "tiledrenderer.hpp"
#include "options.hpp"

#include <iostream>
#include <memory>

Renderer::Renderer(std::shared_ptr<Scene> scene, std::shared_ptr<Integrator> integrator, std::shared_ptr<Sampler> sampler, const Options &options, Logger &logger) : 
    scene(scene),
    sampler(sampler),
    integrator(integrator),
    width(options.width),
    height(options.height),
    nbthreads(options.nbthreads),
    logger(logger)
{
}

Renderer::~Renderer()
{
}

std::unique_ptr<Renderer> Renderer::create_renderer(std::shared_ptr<Scene> scene, std::shared_ptr<Integrator> integrator, std::shared_ptr<Sampler> sampler, const Options &options, Logger &logger)
{
    if (options.renderer == "tiled")
        return std::unique_ptr<Renderer>(new TiledRenderer(scene, integrator, sampler, options, logger));
    else // if (options.renderer == "interactive")
        return std::unique_ptr<Renderer>(new InteractiveRenderer(scene, integrator, sampler, options, logger));
}
