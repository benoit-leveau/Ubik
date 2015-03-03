/*

***********************************************************
* © Milk VFX, inc. All rights reserved. *
***********************************************************

*/

#include "interactiverenderer.hpp"
#include "options.hpp"


InteractiveRenderer::InteractiveRenderer(Scene *scene, const Options &options) :
    Renderer(scene, options)
{
}

InteractiveRenderer::~InteractiveRenderer()
{
}

void InteractiveRenderer::run(std::vector<OutputDriver *>/*output_list*/)
{
}

//    SDL_MOUSEMOTION    = 0x400, /**< Mouse moved */
//    SDL_MOUSEBUTTONDOWN,        /**< Mouse button pressed */
//    SDL_MOUSEBUTTONUP,          /**< Mouse button released */
//    SDL_MOUSEWHEEL,             /**< Mouse wheel motion */
