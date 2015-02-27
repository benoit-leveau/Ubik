/*

***********************************************************
* © Milk VFX, inc. All rights reserved. *
***********************************************************

*/


#include "options.hpp"

Options::Options(size_t width, size_t height, bool interactive, bool show_window, size_t bucketsize, std::string output_file) : 
    width(width),
    height(height),
    interactive(interactive),
    show_window(show_window),
    bucketsize(bucketsize),
    output_file(output_file)
{}

