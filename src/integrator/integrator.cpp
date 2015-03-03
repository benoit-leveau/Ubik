/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/


#include "integrator.hpp"
#include "options.hpp"


Integrator::Integrator(const Options &options) : width(options.width), height(options.height)
{}

Integrator::~Integrator()
{}
