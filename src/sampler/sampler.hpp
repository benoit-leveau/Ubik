/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef SAMPLER_HPP
#define SAMPLER_HPP

#include <cstddef>
#include <iostream>
#include <memory>
#include "logging.hpp"

// forward declaration
class Options;
class Color;
class Integrator;
class Scene;


class Sampler
{
public:
    Sampler(const Options &options, std::shared_ptr<Integrator> integrator, Logger &logger);
    virtual ~Sampler(){}

    virtual Color render(size_t x, size_t y) = 0;
    static std::shared_ptr<Sampler> create_sampler(const Options &options, /*std::shared_ptr<Scene> scene, */std::shared_ptr<Integrator> integrator, Logger &logger);
protected:
    std::shared_ptr<Integrator> integrator;
};

#endif // !SAMPLER_HPP
