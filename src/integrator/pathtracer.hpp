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
class PathTracerData;


class PathTracer : public Integrator
{
public:
    PathTracer(const Options &options);
    virtual ~PathTracer();

    virtual Color render(Scene *scene, size_t x, size_t y, size_t sample);
private:
    PathTracerData *data;
};

#endif // !PATHTRACER_HPP
