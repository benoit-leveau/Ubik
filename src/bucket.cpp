/*

***********************************************************
* © Milk VFX, inc. All rights reserved. *
***********************************************************

*/

#include <iostream>
#include <vector>
#include <list>
#include <time.h>
#include <random>
#include <unistd.h>

#include <OpenEXR/ImfRgba.h>

#include "scene.hpp"
#include "bucket.hpp"


Bucket::Bucket(size_t xpos, size_t ypos, size_t index_x, size_t index_y, size_t bucketsize) : 
    xpos(xpos), ypos(ypos),
    index_x(index_x), index_y(index_y),
    bucketsize(bucketsize),
    completed(false)
{
    bucketdata = new Imf::Rgba[bucketsize*bucketsize];
    for (size_t i=0; i<bucketsize*bucketsize; ++i)
        bucketdata->r = bucketdata->g = bucketdata->b = 0.0;
}
 
Bucket::~Bucket()
{
    delete [] bucketdata;
}

void Bucket::render(Scene *scene)
{
    std::uniform_real_distribution<float> color_distribution(0.0f, 1.0f);
    std::mt19937 color_rng;
    color_rng.seed(index_y*index_y+index_x+scene->frame);
    Imf::Rgba random_color(color_distribution(color_rng),
                           color_distribution(color_rng),
                           color_distribution(color_rng));
    for (size_t y=ypos; y<ypos+bucketsize; ++y)
    {
        for (size_t x=xpos; x<xpos+bucketsize; ++x)
        {
            rng.seed(y*y*y+x*x+scene->frame);
            size_t offset = (y-ypos)*bucketsize+(x-xpos);
            bucketdata[offset] = random_color;
        }
    }
    usleep(color_distribution(color_rng)*2*255);
    completed = true;
}
