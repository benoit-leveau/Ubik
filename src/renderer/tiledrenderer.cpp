/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#include <iostream>
#include <chrono>
#include <unistd.h>

#include "ctpl.h"

#include "tiledrenderer.hpp"
#include "outputdriver.hpp"
#include "displaydriver.hpp"
#include "exrdriver.hpp"
#include "task.hpp"
#include "bucket.hpp"
#include "options.hpp"


void render_task(size_t threadid, Task *task)
{
    // start chrono
    auto t_start = std::chrono::high_resolution_clock::now();
    
    // do the real work
    task->bucket->render();

    // end chrono
    auto t_end = std::chrono::high_resolution_clock::now();

    // set bucket to rendered
    task->time = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    task->threadid = threadid;
    task->completed = true;
}


TiledRenderer::TiledRenderer(std::shared_ptr<Scene> scene, const Options &options) : 
    Renderer(scene, options),
    bucketsize(options.bucketsize),
    display(nullptr)
{
    if (options.show_window)
    {
        display = new DisplayDriver(options.width, options.height);
        output_list.push_back(display);
    }

    // create EXR driver
    EXRDriver *exr_output = new EXRDriver(options.width, options.height, options.bucketsize, options.output_file);
    output_list.push_back(exr_output);

    // create list of buckets
    for (size_t y=0, bucket_y=0; y<height; y+=bucketsize, ++bucket_y){
        for (size_t x=0, bucket_x=0; x<width; x+=bucketsize, ++bucket_x){
            Bucket *bucket = new Bucket(sampler,
                                        x, y,
                                        bucket_x, bucket_y, 
                                        std::min(bucketsize, width-x), 
                                        std::min(bucketsize, height-y));
            bucket_list.push_back(bucket);
        }
    }

    if (options.image_mode == std::string("spiral"))
    {
        std::vector<Bucket *> bucket_list_spiral;
        int number_tiles_x = ceil(width / float(bucketsize));
        int number_tiles_y = ceil(height / float(bucketsize));
        int dx = 0;
        int dy = -1;
        int x = (width/bucketsize-1)/2;
        int y = (height/bucketsize-1)/2;
        size_t length = 1;
        size_t current_length = 0;
        size_t iterations = 0;
        size_t index = 0;
        while(index<bucket_list.size())
        {
            if ((x>=0) and (x<number_tiles_x) and (y>=0) and (y<number_tiles_y))
            {
                bucket_list_spiral.push_back(bucket_list[y*number_tiles_x+x]);
                ++index;
            }
            if (current_length == length)
            {
                // we're at a corner
                iterations += 1;
                current_length = 0;
                // switch direction
                int tmp = dx;
                dx = -dy;
                dy = tmp;
            }
            if (iterations == 2)
            {
                length += 1;
                iterations = 0;
            }
            current_length += 1;
            x += dx;
            y += dy;
        }
        std::swap(bucket_list, bucket_list_spiral);
    }

    // create list of tasks
    for(auto &bucket : bucket_list){
        Task *task = new Task(bucket);
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

void TiledRenderer::run()
{
    std::list<Task *> tasks_pending(task_list);

    // start chrono
    auto t_start = std::chrono::high_resolution_clock::now();
    
    // create the thread pool
    ctpl::thread_pool pool(nbthreads);

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
                // std::cout << "Bucket (" << task->bucket->index_x << "," << task->bucket->index_y << ") was rendered in " << task->time << " ms by thread #" << task->threadid << std::endl;
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

    // close any output
    for(auto &output : output_list)
        output->close();

    if (display)
    {
        // when there is a display driver, wait for user interaction to quit
        display->loop();
    }    

    for(auto &output : output_list)
        delete output;
}
