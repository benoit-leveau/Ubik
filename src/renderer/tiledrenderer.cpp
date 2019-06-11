/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#include <iostream>
#include <iomanip>
#include <sstream>

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
#include "logging.hpp"
#include "memory.hpp"

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


TiledRenderer::TiledRenderer(std::shared_ptr<Scene> scene, const Options &options, Logger &logger) : 
    Renderer(scene, options, logger),
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
        display = new DisplayDriver(options.width, options.height, logger);
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
        logger.log("No output defined, and show_window option not set", WARNING);
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
    while (it.valid)
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
    std::string message = "Starting bucket render with " + std::to_string(nbthreads) + " threads";
    logger.log(message, INFO);

    std::list<Task *> tasks_pending(task_list);

    // start chrono
    ProgressLog progress("Rendering", logger);
    auto t_start = std::chrono::high_resolution_clock::now();
    
    // create the thread pool
    ctpl::thread_pool pool(nbthreads);

    // add the tasks to the pool    
    for(auto &task : task_list){
        pool.push(render_task, task);
    }

    size_t number_tasks = task_list.size();
    float displayed_progress = 0.0f;

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
                
                // Log bucket time
                std::ostringstream stream;
                stream << "Bucket (" << task->bucket->index_x << "," << task->bucket->index_y << ") finished (thread " << task->threadid << "): " << std::fixed << std::setprecision(2) << std::setfill('0') << task->time << "ms";
                logger.log(stream.str(), DEBUG);
                
                // write Bucket
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
            progress.update(100.0*(number_tasks-tasks_pending.size())/float(number_tasks));
            for(auto &output : output_list)
                output->update();
        }
        else
            usleep(200);
    }

    // end chrono
    progress.done();

    size_t memory = getPeakRSS();
    std::string message3 = "Memory Usage Peak: " + get_memory_display(memory);
    logger.log(message3, INFO);

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
