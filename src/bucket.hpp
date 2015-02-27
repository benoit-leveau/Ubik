/*

***********************************************************
* © Milk VFX, inc. All rights reserved. *
***********************************************************

*/

#ifndef BUCKET_H
#define BUCKET_H

#include <random>

#include <OpenEXR/ImfRgba.h>

// forward declaration
class Scene;


class Bucket
{
public:
    Bucket(size_t xpos, size_t ypos, size_t index_x, size_t index_y, size_t bucketsize);
    ~Bucket();
    
    void render(Scene *scene);

    size_t xpos, ypos;
    size_t index_x, index_y;
    size_t bucketsize;
    bool completed;
    Imf::Rgba *bucketdata;
private:
    std::mt19937_64 rng;
};

#endif // !BUCKET_H
