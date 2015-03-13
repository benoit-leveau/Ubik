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

// forward declaration
class Options;
class Color;
class Integrator;
class Scene;


class Sampler
{
public:
    Sampler(const Options &options, std::shared_ptr<Integrator> integrator);
    virtual ~Sampler(){std::cout << "Deleting Sampler" << std::endl;}

    virtual Color render(size_t x, size_t y) = 0;

protected:
    std::shared_ptr<Integrator> integrator;
};

#endif // !SAMPLER_HPP
