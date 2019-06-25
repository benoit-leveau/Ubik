/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef SMALLPT_HPP
#define SMALLPT_HPP

#include <random>
#include <string>
#include <cstddef>
#include <memory>
#include "logging.hpp"
#include "integrator.hpp"

// forward declaration
class Options;
class Radiance;
class Scene;
class Vector;
class RNG;

class SmallPt : public Integrator
{
public:
    SmallPt(const Options &options, std::shared_ptr<Scene> scene, Logger &logger);
    virtual ~SmallPt();

    virtual Radiance render(size_t x, size_t y, size_t sample);
private:

    double width64;
    double height64;
    size_t samples;
};

#endif // !SMALLPT_HPP
