/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/


#include "options.hpp"

Options::Options(size_t width, size_t height, bool interactive, bool show_window, size_t bucketsize, bool spiral, bool pathtracer, bool fixedsampling, size_t min_samples, size_t max_samples, size_t nbthreads, std::string output_file) : 
    width(width),
    height(height),
    interactive(interactive),
    show_window(show_window),
    bucketsize(bucketsize),
    spiral(spiral),
    pathtracer(pathtracer),
    fixedsampling(fixedsampling),
    min_samples(min_samples),
    max_samples(max_samples),
    nbthreads(nbthreads),
    output_file(output_file)
{}

