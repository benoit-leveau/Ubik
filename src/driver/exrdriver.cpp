/*

***********************************************************
* © Benoit Leveau - All rights reserved. 
***********************************************************

*/

#include <random>

#include <OpenEXR/half.h>
#include <OpenEXR/ImfRgba.h>
#include <OpenEXR/ImfArray.h>

#include "scene.hpp"
#include "color.hpp"
#include "bucket.hpp"
#include "outputdriver.hpp"
#include "exrdriver.hpp"


EXRDriver::EXRDriver(size_t width, size_t height, size_t bucketsize, std::string filename) : OutputDriver(width, height), bucketsize(bucketsize)
{
    size_t tileWidth = bucketsize;
    size_t tileHeight = bucketsize;
    out = new Imf_2_0::TiledRgbaOutputFile(filename.c_str(),
                                           width, height,         // image size
                                           tileWidth, tileHeight, // tile size
                                           Imf::ONE_LEVEL,        // level mode
                                           Imf::ROUND_DOWN,       // rounding mode
                                           Imf::WRITE_RGBA);      // channels in file
}

void EXRDriver::close()
{
    if (out != NULL)
    {
        delete this->out;
        out = NULL;
    }
}

EXRDriver::~EXRDriver()
{
    this->close();
}

void EXRDriver::write(Bucket *bucket)
{
    Imf::Array2D<Imf::Rgba> pixels(bucket->bucketheight, bucket->bucketwidth);
    size_t tileY = bucket->index_y;
    size_t tileX = bucket->index_x;
    Imath::Box2i range = out->dataWindowForTile(tileX, tileY);
    for (size_t y=0; y<bucket->bucketheight; ++y){
	   	for (size_t x=0; x<bucket->bucketwidth; ++x){
            size_t bucket_offset = y*bucket->bucketwidth + x;
            const Color &color(bucket->bucketdata[bucket_offset]);
            pixels[y][x] = Imf::Rgba(color.r, color.g, color.b);
        }
    }
    out->setFrameBuffer(&pixels[-range.min.y][-range.min.x],
                       1,                    // xStride
                       bucket->bucketwidth);  // yStride
    out->writeTile(tileX, tileY);
}
