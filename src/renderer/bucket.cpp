/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#include <iostream>
#include <vector>
#include <list>
#include <time.h>
#include <unistd.h>

#include "color.hpp"
#include "scene.hpp"
#include "bucket.hpp"
#include "sampler.hpp"


Bucket::Bucket(std::shared_ptr<Sampler> sampler, size_t xpos, size_t ypos, size_t index_x, size_t index_y, size_t bucketwidth, size_t bucketheight) : 
    sampler(sampler),
    xpos(xpos), ypos(ypos),
    index_x(index_x), index_y(index_y),
    bucketwidth(bucketwidth),
    bucketheight(bucketheight),
    completed(false)
{
    bucketdata = new Color[bucketwidth*bucketheight]();
}
 
Bucket::~Bucket()
{
    delete [] bucketdata;
}

void Bucket::render()
{
    for (size_t y=ypos; y<ypos+bucketheight; ++y)
    {
        for (size_t x=xpos; x<xpos+bucketwidth; ++x)
        {
            size_t offset = (y-ypos)*bucketwidth+(x-xpos);
            bucketdata[offset] = sampler->render(x, y);
        }
    }
    // barrier?
    completed = true;
}

