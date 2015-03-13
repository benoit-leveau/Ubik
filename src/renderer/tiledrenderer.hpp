/*

***********************************************************
* � Benoit Leveau - All rights reserved. *
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

class TiledRenderer : public Renderer
{
public:
    TiledRenderer(std::shared_ptr<Scene> scene, const Options &options);
    virtual ~TiledRenderer();

    virtual void run();
private:
    size_t bucketsize;
    std::vector<Bucket *> bucket_list;
    std::list<Task *>task_list;
    std::vector<OutputDriver *>output_list;
    DisplayDriver *display;
};

#endif // !TILEDRENDERER_H
