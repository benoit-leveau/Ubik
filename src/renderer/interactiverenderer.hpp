/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef INTERACTIVERENDERER_H
#define INTERACTIVERENDERER_H

#include <vector>
#include <list>

#include "renderer.hpp"

// forwrard declaration
class OutputDriver;
class Bucket;
class Task;


class InteractiveRenderer : public Renderer
{
public:
    InteractiveRenderer(Scene *scene, const Options &options);
    virtual ~InteractiveRenderer();

    virtual void run(std::vector<OutputDriver *>output_list);
private:
    std::vector<Bucket *> bucket_list;
    std::vector<Bucket *> bucket_list_final;
    std::list<Task *>task_list;
};

#endif // !INTERACTIVERENDERER_H
