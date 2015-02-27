/*

***********************************************************
* © Milk VFX, inc. All rights reserved. *
***********************************************************

*/


#include "renderer.hpp"
#include "interactiverenderer.hpp"
#include "tiledrenderer.hpp"
#include "options.hpp"


Renderer::Renderer(Scene *scene, const Options &options) : 
    scene(scene),
    width(options.width),
    height(options.height)
{}

Renderer::~Renderer()
{}

Renderer *Renderer::create_renderer(Scene *scene, const Options &options)
{
    if (options.interactive)
        return new InteractiveRenderer(scene, options);
    else
        return new TiledRenderer(scene, options);
}
