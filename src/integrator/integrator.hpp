/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef INTEGRATOR_HPP
#define INTEGRATOR_HPP

#include <cstddef>

// forward declaration
class Options;
class Color;
class Scene;

class Integrator
{
public:
    Integrator(const Options &options);
    virtual ~Integrator();

    virtual Color render(Scene*scene, size_t x, size_t y, size_t sample) = 0;

protected:
    size_t width;
    size_t height;
};

#endif // !INTEGRATOR_HPP
