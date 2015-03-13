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

Renderer::Renderer(std::shared_ptr<Scene> scene, const Options &options) : 
    scene(scene),
    sampler(nullptr),
    integrator(nullptr),
    width(options.width),
    height(options.height),
    nbthreads(options.nbthreads)
{
    if (options.pathtracer)
        integrator = std::shared_ptr<Integrator>(new ImageRender(options, scene)); // PathTracer, options
    if (options.fixedsampling)
        sampler = std::shared_ptr<Sampler>(new FixedSampler(options, integrator));
}

Renderer::~Renderer()
{
    std::cout << "Deleting Renderer" << std::endl;
}

std::unique_ptr<Renderer> Renderer::create_renderer(std::shared_ptr<Scene> scene, const Options &options)
{
    if (options.interactive)
        return std::unique_ptr<Renderer>(new InteractiveRenderer(scene, options));
    else
        return std::unique_ptr<Renderer>(new TiledRenderer(scene, options));
}
