/*

***********************************************************
* © Milk VFX, inc. All rights reserved. *
***********************************************************

*/

#ifndef INTERACTIVERENDERER_H
#define INTERACTIVERENDERER_H

#include <vector>

#include "renderer.hpp"

// forwrard declaration
class Scene;
class Options;
class OutputDriver;


class InteractiveRenderer : public Renderer
{
public:
    InteractiveRenderer(Scene *scene, const Options &options);
    virtual ~InteractiveRenderer();

    virtual void run(std::vector<OutputDriver *>output_list);
private:
};

#endif // !INTERACTIVERENDERER_H
