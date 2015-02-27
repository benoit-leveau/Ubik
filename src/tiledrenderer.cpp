/*

***********************************************************
* © Milk VFX, inc. All rights reserved. *
***********************************************************

*/

#include <chrono>
#include <unistd.h>

#include "ctpl.h"

#include "tiledrenderer.hpp"
#include "outputdriver.hpp"
#include "task.hpp"
#include "bucket.hpp"
#include "options.hpp"


void render_task(size_t threadid, Task *task)
{
    // start chrono
    auto t_start = std::chrono::high_resolution_clock::now();
    
    // do the real work
    task->bucket->render(task->scene);

    // end chrono
    auto t_end = std::chrono::high_resolution_clock::now();

    // set bucket to rendered
    task->time = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    task->threadid = threadid;
    task->completed = true;
}

TiledRenderer::TiledRenderer(Scene *scene, const Options &options) : 
    Renderer(scene, options),
    bucketsize(options.bucketsize)    
{
    // create list of buckets
    for (size_t y=0, bucket_y=0; y<height; y+=bucketsize, ++bucket_y){
        for (size_t x=0, bucket_x=0; x<width; x+=bucketsize, ++bucket_x){
            Bucket *bucket = new Bucket(x, y, bucket_x, bucket_y, bucketsize);
            bucket_list.push_back(bucket);
        }
    }

    // create list of tasks
    for(auto &bucket : bucket_list){
        Task *task = new Task(bucket, scene);
        task_list.push_back(task);   
    }
}

TiledRenderer::~TiledRenderer()
{
    for(auto &task : task_list)
        delete task;
    for(auto &bucket : bucket_list)
        delete bucket;
}

void TiledRenderer::run(std::vector<OutputDriver *>output_list)
{
    std::list<Task *> tasks_pending(task_list);

    // start chrono
    auto t_start = std::chrono::high_resolution_clock::now();
    
    // create the thread pool
    ctpl::thread_pool pool(8);

    // add the tasks to the pool    
    for(auto &task : task_list){
        pool.push(render_task, task);
    }

    // wait for all tasks to be processed
    while(tasks_pending.size()>0)
    {
        bool found = false;
        auto it = tasks_pending.begin();
        while(it != tasks_pending.end())
        {   
            if ((*it)->completed)
            {             
                Task *task(*it);
                std::cout << "Bucket (" << task->bucket->index_x << "," << task->bucket->index_y << ") was rendered in " << task->time << " ms by thread #" << task->threadid << std::endl;
                for(auto &output : output_list)
                    output->write(task->bucket);
                it = tasks_pending.erase(it);
                found = true;
            }
            else
                ++it;
        }
        if (found)
        {
            for(auto &output : output_list)
                output->update();
        }
        else
            usleep(200);
    }

    // end chrono
    auto t_end = std::chrono::high_resolution_clock::now();

    // 
    double time = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    std::cout << "Rendered image in " << time << "ms" << std::endl;
}
