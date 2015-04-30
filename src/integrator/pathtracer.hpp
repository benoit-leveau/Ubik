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
class Vector;
class RNG;

class PathTracer : public Integrator
{
public:
    PathTracer(const Options &options, std::shared_ptr<Scene> scene);
    virtual ~PathTracer();

    virtual Color render(size_t x, size_t y, size_t sample);
private:
    Color get_radiance(Vector view_position, Vector sample_direction, const RNG &rng);
    
    double width64;
    double height64;
};

#endif // !PATHTRACER_HPP
