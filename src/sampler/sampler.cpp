/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/


#include "sampler.hpp"

Sampler::Sampler(const Options &/*options*/, std::shared_ptr<Integrator> integrator) :
    integrator(integrator)
{}
