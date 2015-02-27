/*

***********************************************************
* © Milk VFX, inc. All rights reserved. *
***********************************************************

*/

#ifndef OUTPUTDRIVER_H
#define OUTPUTDRIVER_H

#include <cstddef>

// forward declaration
class Bucket;

class OutputDriver
{
public:
    OutputDriver(size_t width, size_t height);
    virtual ~OutputDriver() = default;    

    virtual void write(Bucket *bucket) = 0;
    virtual void update(){}
    virtual void loop(){}
    virtual void close(){}
protected:
    size_t width;
    size_t height;
};

#endif // !OUTPUTDRIVER_H
