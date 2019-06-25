/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#include <thread>
#include <iostream>


#include "options.hpp"

Options::Options(size_t width, size_t height, std::string sampler, std::string integrator, std::string renderer, bool show_window, size_t bucketsize, std::string image_mode, size_t min_samples, size_t max_samples, size_t samples, double threshold, size_t nbthreads, std::string output_file) : 
    width(width),
    height(height),
    sampler(sampler),
    integrator(integrator),
    renderer(renderer),
    show_window(show_window),
    bucketsize(bucketsize),
    image_mode(image_mode),
    min_samples(min_samples),
    max_samples(std::max(min_samples, max_samples)),
    samples(samples),
    threshold(threshold),
    nbthreads(nbthreads>0 ? nbthreads : std::thread::hardware_concurrency()),
    output_file(output_file)
{
}
