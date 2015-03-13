/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/

#include <iostream>

#include "integrator.hpp"
#include "options.hpp"


Integrator::Integrator(const Options &options, std::shared_ptr<Scene> scene) : width(options.width), height(options.height), scene(scene)
{}

Integrator::~Integrator()
{
    std::cout << "Deleting Integrator" << std::endl;
}
