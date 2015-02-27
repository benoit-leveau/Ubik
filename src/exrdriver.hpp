/*

***********************************************************
* © Milk VFX, inc. All rights reserved. *
***********************************************************

*/

#ifndef EXRDRIVER_H
#define EXRDRIVER_H

#include <OpenEXR/ImfTiledRgbaFile.h>

#include "outputdriver.hpp"


class EXRDriver : public OutputDriver
{
public:
    EXRDriver(size_t width, size_t height, size_t bucketsize, std::string filename);
    ~EXRDriver();

    void write(Bucket *bucket);
    void close();
private:
    Imf::TiledRgbaOutputFile *out;
    size_t bucketsize;
};

#endif // !EXRDRIVER_H
