/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/

#include <cstddef>
#include <utility>
#include <iostream>

#include "arrayiterator.hpp"

int main(int argc, char **argv)
{
    size_t size_x = atoi(argv[1]);
    size_t size_y = atoi(argv[2]);
    std::string mode = argv[3];
    ArrayIterationMode iter_mode;
    if (mode == "topleft")
        iter_mode = ArrayIterationMode::TOPLEFT;
    else if (mode == "topright")
        iter_mode = ArrayIterationMode::TOPRIGHT;
    else if (mode == "bottomleft")
        iter_mode = ArrayIterationMode::BOTTOMLEFT;
    else if (mode == "bottomright")
        iter_mode = ArrayIterationMode::BOTTOMRIGHT;
    else if (mode == "spiral")
        iter_mode = ArrayIterationMode::SPIRAL;
    ArrayIterator it(size_x, size_y, iter_mode);
    size_t *array = new size_t[size_x*size_y];
    size_t current = 0;
    while (it.valid)
    {
        auto pair = it.next();
        size_t x = pair.first;
        size_t y = pair.second;
        array[y*size_x+x] = ++current;
    }
    for (size_t y=0; y<size_y; ++y)
    {
        for (size_t x=0; x<size_x; ++x)
        {
            std::cout << array[y*size_x+x] << "\t";
        }
        std::cout << std::endl;
    }
    
    return 0;
}
