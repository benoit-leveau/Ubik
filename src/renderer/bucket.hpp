/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef BUCKET_H
#define BUCKET_H

#include <cstddef>
#include <random>
#include <memory>

// forward declaration
class Sampler;
class Color;


class Bucket
{
public:
    Bucket(std::shared_ptr<Sampler> sampler, size_t xpos, size_t ypos, size_t index_x, size_t index_y, size_t bucketwidth, size_t bucketheight);
    ~Bucket();
    
    void render();

    std::shared_ptr<Sampler> sampler;
    size_t xpos, ypos;
    size_t index_x, index_y;
    size_t bucketwidth, bucketheight;
    bool completed;
    Color *bucketdata;
};

#endif // !BUCKET_H
