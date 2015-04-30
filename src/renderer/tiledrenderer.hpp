/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef TILEDRENDERER_H
#define TILEDRENDERER_H

#include <cstddef>
#include <vector>
#include <list>

#include "renderer.hpp"

// forward declaration
class Bucket;
class Task;
class OutputDriver;
class DisplayDriver;
enum class ArrayIterationMode;
class Logger;


class TiledRenderer : public Renderer
{
public:
    TiledRenderer(std::shared_ptr<Scene> scene, const Options &options, Logger &logger);
    virtual ~TiledRenderer();

    virtual void run();
private:
    size_t bucketsize;
    std::vector<Bucket *> bucket_list;
    std::list<Task *>task_list;
    std::vector<OutputDriver *>output_list;
    DisplayDriver *display;
    ArrayIterationMode image_mode;
};

#endif // !TILEDRENDERER_H
