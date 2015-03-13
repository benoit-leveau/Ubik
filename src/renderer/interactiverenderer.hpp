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
    InteractiveRenderer(std::shared_ptr<Scene> scene, const Options &options);
    virtual ~InteractiveRenderer();

    virtual void run();
    std::unique_ptr<DisplayDriver> display;
private:
};

#endif // !INTERACTIVERENDERER_H
