/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/

#include "arrayiterator.hpp"

namespace
{
    class SpiralIteratorData
    {
    public:
        SpiralIteratorData(size_t size_x, size_t size_y);
        std::pair<size_t, size_t> next();
    private:
        size_t size_x, size_y;
        size_t current_length, length;
        int current_x, current_y;
        int dx, dy;
        size_t iterations;
    };

    SpiralIteratorData::SpiralIteratorData(size_t size_x, size_t size_y) : 
        size_x(size_x),
        size_y(size_y),
        current_length(0),
        length(1),
        current_x((size_x-1)/2),
        current_y((size_y-1)/2),
        dx(1), dy(0),
        iterations(0)
    {}

    std::pair<size_t, size_t> SpiralIteratorData::next()
    {
        size_t x, y;
        bool found = false;
        while (!found)
        {
            if ((current_x>=0) && (current_x<int(size_x)) && (current_y>=0) && (current_y<int(size_y)))
            {
                x = current_x;
                y = current_y;
                found = true;
            }
            if (current_length == length)
            {
                // we're at a corner
                ++iterations;
                current_length = 0;
                // switch direction
                std::swap(dx, dy);
                dx = -dx;
            }
            if (iterations == 2)
            {
                ++length;
                iterations = 0;
            }
            ++current_length;
            current_x += dx;
            current_y += dy;
        }
        return std::make_pair(x, y);
    }
}

ArrayIterator::ArrayIterator(size_t size_x, size_t size_y, ArrayIterationMode mode) :
    valid(true),
    size_x(size_x),
    size_y(size_y),
    mode(mode),
    current(0),
    data(nullptr)
{
    if (size_x * size_y == 0)
        valid = false;
    if (mode == ArrayIterationMode::SPIRAL)
        data = new SpiralIteratorData(size_x, size_y);
}

ArrayIterator::~ArrayIterator()
{
    if (data)
    {
        if (mode == ArrayIterationMode::SPIRAL)
            delete static_cast<SpiralIteratorData *>(data);
    }
}

std::pair<size_t, size_t> ArrayIterator::next()
{
    if (!valid)
        return std::make_pair(0, 0);
    std::pair<size_t, size_t> result;
    switch (mode)
    {
    case ArrayIterationMode::TOPLEFT:
        {
            result = std::make_pair(current % size_x, current / size_x);
            break;
        }
    case ArrayIterationMode::TOPRIGHT:
        {
            result = std::make_pair(size_x - (current % size_x) - 1, current / size_x);
            break;
        }
    case ArrayIterationMode::BOTTOMLEFT:
        {
            result = std::make_pair(current % size_x, size_y - (current / size_x) - 1);
            break;
        }
    case ArrayIterationMode::BOTTOMRIGHT:
        {
            result = std::make_pair(size_x - (current % size_x) - 1, size_y - (current / size_x) - 1);
            break;
        }
    case ArrayIterationMode::SPIRAL:
        {
            result = static_cast<SpiralIteratorData *>(data)->next();
            break;
        }
    }
    ++current;
    if (current >= size_x * size_y)
        valid = false;
    return result;
}

size_t ArrayIterator::next_1d()
{
    auto pair = next();
    return (pair.second * size_x + pair.first);
}
