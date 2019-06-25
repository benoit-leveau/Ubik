/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef IMAGERENDER_HPP
#define IMAGERENDER_HPP

#include <random>
#include <string>
#include <cstddef>
#include "integrator.hpp"
#include "rng.hpp"

// forward declaration
class Options;
class Radiance;
class Scene;
class ImageRenderData;


class ImageRender : public Integrator
{
public:
    ImageRender(const Options &options, std::shared_ptr<Scene> scene);
    virtual ~ImageRender();

    virtual Radiance render(size_t x, size_t y, size_t sample);
private:
    ImageRenderData *data;
    bool add_noise;
};

#endif // !IMAGERENDER_HPP
