/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef BUCKET_H
#define BUCKET_H

#include <cstddef>
#include <random>

// forward declaration
class Sampler;
class Integrator;
class Scene;
class Color;


class Bucket
{
public:
    Bucket(size_t xpos, size_t ypos, size_t index_x, size_t index_y, size_t bucketwidth, size_t bucketheight);
    ~Bucket();
    
    void render(Sampler *sampler, Integrator *integrator, Scene *scene);
    void render_center(Sampler *sampler, Integrator *integrator, Scene *scene);

    size_t xpos, ypos;
    size_t index_x, index_y;
    size_t bucketwidth, bucketheight;
    bool completed;
    Color *bucketdata;
};

#endif // !BUCKET_H
