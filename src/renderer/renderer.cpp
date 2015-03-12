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


Renderer::Renderer(Scene *scene, const Options &options) : 
    scene(scene),
    sampler(NULL),
    integrator(NULL),
    width(options.width),
    height(options.height),
    nbthreads(options.nbthreads)
{
    if (options.fixedsampling)
        sampler = new FixedSampler(options);
    if (options.pathtracer)
        integrator = new ImageRender(options); // PathTracer(options);
}

Renderer::~Renderer()
{
    if (sampler)
        delete sampler;
    if (integrator)
        delete integrator;
}

Renderer *Renderer::create_renderer(Scene *scene, const Options &options)
{
    if (options.interactive)
        return new InteractiveRenderer(scene, options);
    else
        return new TiledRenderer(scene, options);
}
