/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#include "imagerender.hpp"
#include "color.hpp"
#include "scene.hpp"
#include "rng.hpp"
#include "options.hpp"

#include "tiffio.h"

#include <chrono>
#include <unistd.h>

struct ImageRenderData
{
    ImageRenderData(const std::string &filename);
    ~ImageRenderData();
    TIFF* tif;
    uint32 imageWidth, imageLength;
	uint32 tileWidth, tileLength;
    tdata_t *buf;
    uint32 *raster;
    size_t nb_tiles;
    bool tiled;
};


ImageRenderData::ImageRenderData(const std::string &filename) : buf(nullptr), raster(nullptr), tiled(false)
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

ImageRenderData::~ImageRenderData()
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

ImageRender::ImageRender(const Options &options, std::shared_ptr<Scene> scene) : 
    Integrator(options, scene),
    add_noise(true) // !options.interactive)
{
#ifdef LINUX
    data = new ImageRenderData("/milk/users/benoit/pwork/ubik/image.tx");
#elif OSX
    data = new ImageRenderData("/Users/benoit/Documents/Prog/ubik/bin/image.tiff");
#endif
}

ImageRender::~ImageRender()
{
    delete data;
}

Color ImageRender::render(size_t x, size_t y, size_t sample)
{
    // initialize the RNG in a deterministic way for each sample
    RNG rng;
    rng.seed(y*y*y + x*x + scene->frame + sample);

    // trace a ray
    // ...

    // convert (x, y) into image coordinates
    int img_x = 0; //x - scene->x;
    int img_y = 0; //y - scene->y;
    if ((img_x < 0) || (img_y < 0) || (img_x >= int(this->width)) || (img_y >= int(this->height)))
    {
        return Color(0, 0, 0);
    }
    img_x = (img_x * data->imageWidth) / this->width;
    img_y = ((this->height-img_y-1) * data->imageLength) / this->height;

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
        float noise = 0.0f;
        for (size_t index=0; index<50; ++index)
        {
            noise = std::max(0.0, std::min(1.0, noise+rng()-0.5));
        }
        if (add_noise)
        {
            r = std::max(0.0, std::min(1.0, r + (noise-0.5)*0.3));
            g = r + (g-r) * std::min(1.0, (sample/10.0));
            b = r + (b-r) * std::min(1.0, (sample/10.0));
        }
        return Color(r, g, b);
    }
}
