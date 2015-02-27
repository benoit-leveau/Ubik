/*

***********************************************************
* © Milk VFX, inc. All rights reserved. *
***********************************************************

*/

#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <string>


class Options
{
public:
    Options(size_t width, size_t height, bool interactive, bool show_window, size_t bucketsize, std::string output_file); 
    
    size_t width;
    size_t height;
    bool interactive;
    bool show_window;
    size_t bucketsize;
    std::string output_file;
};




#endif // !OPTIONS_HPP
