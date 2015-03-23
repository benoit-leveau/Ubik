/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/

#ifndef ARRAYITERATOR_HPP
#define ARRAYITERATOR_HPP

#include <cstddef>
#include <utility>

enum class ArrayIterationMode{TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT, SPIRAL};

class ArrayIterator
{
public:
    ArrayIterator(size_t size_x, size_t size_y, ArrayIterationMode mode);
    ~ArrayIterator();

    std::pair<size_t, size_t> next();
    size_t next_1d();

    bool valid;

private:
    size_t size_x, size_y;
    ArrayIterationMode mode;
    size_t current;
    void *data; // used only for SPIRAL mode
};

#endif // !ARRAYITERATOR_HPP
