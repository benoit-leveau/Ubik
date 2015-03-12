/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef INTERACTIVERENDERER_H
#define INTERACTIVERENDERER_H

#include <vector>

#include "renderer.hpp"

// forwrard declaration
class OutputDriver;
class DisplayDriver;

class InteractiveRenderer : public Renderer
{
public:
    InteractiveRenderer(Scene *scene, const Options &options);
    virtual ~InteractiveRenderer();

    virtual void run(std::vector<OutputDriver *>output_list);
    DisplayDriver *display;
private:
};

#endif // !INTERACTIVERENDERER_H
