/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef RENDERER_H
#define RENDERER_H

#include <cstddef>
#include <vector>
#include <memory>

// forward declaration
class Sampler;
class Integrator;
class Scene;
class Options;
class OutputDriver;


class Renderer
{
public:
    Renderer(std::shared_ptr<Scene> scene, const Options &options);
    virtual ~Renderer();

    virtual void run() = 0;
    static std::unique_ptr<Renderer> create_renderer(std::shared_ptr<Scene> scene, const Options &options);

// protected:
    std::shared_ptr<Scene> scene;
    std::shared_ptr<Sampler> sampler;
    std::shared_ptr<Integrator> integrator;
    size_t width;
    size_t height;
    size_t nbthreads;
};

#endif // !RENDERER_H
