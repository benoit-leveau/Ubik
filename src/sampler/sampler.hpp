/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef SAMPLER_HPP
#define SAMPLER_HPP

#include <cstddef>

// forward declaration
class Options;
class Color;
class Integrator;
class Scene;


class Sampler
{
public:
    Sampler(const Options &/*options*/){}
    virtual ~Sampler(){}

    virtual Color render(Integrator *integrator, Scene *scene, size_t x, size_t y) = 0;
};

#endif // !SAMPLER_HPP
