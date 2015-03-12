/*

***********************************************************
* � Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef IMAGERENDER_HPP
#define IMAGERENDER_HPP

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

#endif // !IMAGERENDER_HPP
