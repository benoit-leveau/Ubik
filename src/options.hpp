/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <cstddef>
#include <string>


class Options
{
public:
    Options(size_t width, size_t height, bool interactive, bool show_window, size_t bucketsize, std::string image_mode, bool pathtracer, bool fixedsampling, size_t min_samples, size_t max_samples, size_t nbthreads, std::string output_file); 
    
    size_t width;
    size_t height;
    bool interactive;
    bool show_window;
    size_t bucketsize;
    std::string image_mode;
    bool pathtracer;
    bool fixedsampling;
    size_t min_samples;
    size_t max_samples;
    size_t nbthreads;
    std::string output_file;
};

#endif // !OPTIONS_HPP
