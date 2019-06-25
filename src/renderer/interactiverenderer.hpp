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
class Logger;
class Integrator;
class Sampler;


class InteractiveRenderer : public Renderer
{
public:
    InteractiveRenderer(std::shared_ptr<Scene> scene, std::shared_ptr<Integrator> integrator, std::shared_ptr<Sampler> sampler, const Options &options, Logger &logger);
    virtual ~InteractiveRenderer();

    virtual void run();
    std::unique_ptr<DisplayDriver> display;
    size_t progressive_tiles;
    size_t full_tiles;
private:
    size_t bucketsize;
    void create_buckets();
};

#endif // !INTERACTIVERENDERER_H
