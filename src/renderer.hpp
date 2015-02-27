/*

***********************************************************
* © Milk VFX, inc. All rights reserved. *
***********************************************************

*/

#ifndef RENDERER_H
#define RENDERER_H

#include <cstddef>
#include <vector>

// forward declaration
class Scene;
class Options;
class OutputDriver;


class Renderer
{
public:
    Renderer(Scene *scene, const Options &options);
    virtual ~Renderer();

    virtual void run(std::vector<OutputDriver *>output_list) = 0;
    static Renderer *create_renderer(Scene *scene, const Options &options);

protected:
    Scene *scene;
    size_t width;
    size_t height;
};

#endif // !RENDERER_H
