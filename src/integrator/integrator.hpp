/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef INTEGRATOR_HPP
#define INTEGRATOR_HPP

#include <cstddef>
#include <memory>

// forward declaration
class Options;
class Radiance;
class Scene;

class Integrator
{
public:
    Integrator(const Options &options, std::shared_ptr<Scene> scene);
    virtual ~Integrator();

    virtual Radiance render(size_t x, size_t y, size_t sample) = 0;

protected:
    size_t width;
    size_t height;
    std::shared_ptr<Scene> scene;
};

#endif // !INTEGRATOR_HPP
