/*

***********************************************************
* © Milk VFX, inc. All rights reserved. *
***********************************************************

*/

#include "pathtracer.hpp"
#include "color.hpp"
#include "scene.hpp"
#include "rng.hpp"
#include "options.hpp"

#include "tiffio.h"

#include <chrono>
#include <unistd.h>

struct PathTracerData
{
    PathTracerData(const std::string &filename);
    ~PathTracerData();
    TIFF* tif;
    uint32 imageWidth, imageLength;
	uint32 tileWidth, tileLength;
    tdata_t *buf;
    uint32 *raster;
    size_t nb_tiles;
    bool tiled;
};


PathTracerData::PathTracerData(const std::string &filename) : buf(NULL), raster(NULL), tiled(false)
{
    uint32 x, y;
    tif = TIFFOpen(filename.c_str(), "r");
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imageWidth);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageLength);
    if (tiled)
    {
        TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tileWidth);
        TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileLength);
        nb_tiles = (imageWidth/tileWidth) * (imageLength/tileLength);
        buf = new tdata_t[nb_tiles];
        size_t index_tile;
        for (y = 0, index_tile=0; y < imageLength; y += tileLength)
        {
            for (x = 0; x < imageWidth; x += tileWidth)
            {
                buf[index_tile] = _TIFFmalloc(TIFFTileSize(tif));
                TIFFReadTile(tif, buf[index_tile], x, y, 0, 0);
                index_tile += 1;
            }
        }
    }
    else
    {
        raster = new uint32[imageWidth*imageLength];
        TIFFReadRGBAImage(tif, imageWidth, imageLength, raster, 0);
    }
}

PathTracerData::~PathTracerData()
{
    if (tiled)
    {
        for (size_t index_tile=0; index_tile<nb_tiles; ++index_tile)
            _TIFFfree(buf[index_tile]);
        delete [] buf;
    }
    else
    {
        delete [] raster;
    }
    TIFFClose(tif);
}

PathTracer::PathTracer(const Options &options) : Integrator(options)
{
    data = new PathTracerData("/milk/users/benoit/personal/ubik/image.tx");
}

PathTracer::~PathTracer()
{
    delete data;
}

Color PathTracer::render(Scene *scene, size_t x, size_t y, size_t sample)
{
    // initialize the RNG in a deterministic way for each sample
    RNG rng;
    rng.seed(y*y*y + x*x + scene->frame + sample);

    // trace a ray
    // ...

    // convert (x, y) into image coordinates
    size_t img_x = (x * data->imageWidth) / this->width;
    size_t img_y = ((this->height-y-1) * data->imageLength) / this->height;

    if (data->tiled)
    {
        size_t tile_x = img_x / data->tileWidth;
        size_t tile_y = img_y / data->tileLength;

        size_t pos_tile_x = img_x % data->tileWidth;
        size_t pos_tile_y = img_y % data->tileLength;
        
        uint32 abgr = static_cast<uint32 *>(data->buf[tile_y*(data->imageWidth/data->tileWidth)+tile_x])[pos_tile_y*data->tileWidth+pos_tile_x];
    
        // std::cout << "Rendering (" << x  << "," << y << ") - image (" << img_x << "," << img_y << ") - tile (" << tile_x << "," << tile_y << ") pixel(" << pos_tile_x << "," << pos_tile_y << ")" << std::endl;

        float r = TIFFGetR(abgr) / 255.0;
        float g = TIFFGetG(abgr) / 255.0;
        float b = TIFFGetB(abgr) / 255.0;
        return Color(r, g, b);
    }
    else
    {
        uint32 abgr = data->raster[img_y*data->imageWidth+img_x];
        float r = TIFFGetR(abgr) / 255.0;
        float g = TIFFGetG(abgr) / 255.0;
        float b = TIFFGetB(abgr) / 255.0;
        r += rng()*0.3;
        g += rng()*0.3;
        b += rng()*0.3;
        return Color(r, g, b);
    }
}
