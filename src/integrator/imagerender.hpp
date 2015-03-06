/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef PATHTRACER_HPP
#define PATHTRACER_HPP

#include <random>
#include <string>
#include <cstddef>
#include "integrator.hpp"

// forward declaration
class Options;
class Color;
class Scene;
class ImageRenderData;


class ImageRender : public Integrator
{
public:
    ImageRender(const Options &options);
    virtual ~ImageRender();

    virtual Color render(Scene *scene, size_t x, size_t y, size_t sample);
private:
    ImageRenderData *data;
    bool add_noise;
};

#endif // !PATHTRACER_HPP
