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
#include "arrayiterator.hpp"


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
    display(nullptr),
    image_mode(ArrayIterationMode::BOTTOMLEFT)
{
    if (options.image_mode == "topleft")
        image_mode = ArrayIterationMode::TOPLEFT;
    else if (options.image_mode == "topright")
        image_mode = ArrayIterationMode::TOPRIGHT;
    else if (options.image_mode == "bottomleft")
        image_mode = ArrayIterationMode::BOTTOMLEFT;
    else if (options.image_mode == "bottomright")
        image_mode = ArrayIterationMode::BOTTOMRIGHT;
    else if (options.image_mode == "spiral")
        image_mode = ArrayIterationMode::SPIRAL;

    if (options.show_window)
    {
        display = new DisplayDriver(options.width, options.height);
        output_list.push_back(display);
    }

    if (options.output_file.size() > 0)
    {
        // create EXR driver
        EXRDriver *exr_output = new EXRDriver(options.width, options.height, options.bucketsize, options.output_file);
        output_list.push_back(exr_output);
    }
    else if (!options.show_window)
    {
        std::cout << "Warning! No output defined, and show_window option not set." << std::endl;
    }

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

    // create list of tasks
    int number_tiles_x = ceil(float(width) / float(bucketsize));
    int number_tiles_y = ceil(float(height) / float(bucketsize));
    ArrayIterator it(number_tiles_x, number_tiles_y, image_mode);
    while(it.valid)
    {
        Task *task = new Task(bucket_list[it.next_1d()]);
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
    
    std::cout << "Starting bucket render with " << nbthreads << " threads" << std::endl;

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
