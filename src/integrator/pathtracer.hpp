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
#include <memory>
#include "integrator.hpp"

// forward declaration
class Options;
class Color;
class Scene;


class PathTracer : public Integrator
{
public:
    PathTracer(const Options &options, std::shared_ptr<Scene> scene);
    virtual ~PathTracer();

    virtual Color render(size_t x, size_t y, size_t sample);
private:
};

#endif // !PATHTRACER_HPP
