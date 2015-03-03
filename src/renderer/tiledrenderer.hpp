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

class TiledRenderer : public Renderer
{
public:
    TiledRenderer(Scene *scene, const Options &options);
    virtual ~TiledRenderer();

    virtual void run(std::vector<OutputDriver *>output_list);    
private:
    size_t bucketsize;
    std::vector<Bucket *> bucket_list;
    std::list<Task *>task_list;
};

#endif // !TILEDRENDERER_H
