/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#include <iostream>
#include <chrono>
#include <unistd.h>

#include "ctpl.h"

#include "interactiverenderer.hpp"
#include "outputdriver.hpp"
#include "task.hpp"
#include "bucket.hpp"
#include "options.hpp"

#include <SDL.h>

// integrate the renderer with SDL, no additional output driver
// make the tasks interruptible (especially the bucket ones)


namespace
{
    void render_task(size_t threadid, Task *task, Renderer *renderer, bool render_center)
    {
        // start chrono
        auto t_start = std::chrono::high_resolution_clock::now();
        
        // do the real work
        if (render_center)
        {
            // usleep(1);
            task->bucket->render_center(renderer->sampler, renderer->integrator, renderer->scene);
        }
        else
        {
            task->bucket->render(renderer->sampler, renderer->integrator, renderer->scene);
            // usleep(10);
        }
        
        // end chrono
        auto t_end = std::chrono::high_resolution_clock::now();
        
        // set bucket to rendered
        task->time = std::chrono::duration<double, std::milli>(t_end-t_start).count();
        task->threadid = threadid;
        task->completed = true;
    }
}


InteractiveRenderer::InteractiveRenderer(Scene *scene, const Options &options) :
    Renderer(scene, options)
{
    size_t level = 1;
    size_t number_buckets = 1;
    size_t bucket_width = options.width;
    size_t bucket_height = options.height;
    while (std::max(bucket_width, bucket_height)>options.bucketsize)
    {
        if (level > 0)
        {
            for (size_t bucket_index_y = 0; bucket_index_y<number_buckets; ++bucket_index_y)
            {
                for (size_t bucket_index_x = 0; bucket_index_x<number_buckets; ++bucket_index_x)
                {
                    size_t x = bucket_index_x*bucket_width;
                    size_t y = bucket_index_y*bucket_height;
                    Bucket *bucket = new Bucket(x, y,
                                                level,
                                                bucket_index_y*number_buckets+bucket_index_x, 
                                                std::min(bucket_width, options.width-x), 
                                                std::min(bucket_height, options.height-y));
                    bucket_list.push_back(bucket);
                    // std::cout << "creating bucket (" << x << "," << y << ") => " << (x+std::min(bucket_width, options.width-x)-1) << "," << (y+std::min(bucket_height, options.height-y)-1) << ")" << std::endl;
                    if (std::max(bucket_width/2, bucket_height/2)<=options.bucketsize)
                    {
                        bucket_list_final.push_back(bucket);
                    }
                }
            }
        }
        ++level;
        number_buckets *= 2;
        bucket_width /= 2;
        bucket_height /= 2;
    }
}

InteractiveRenderer::~InteractiveRenderer()
{
    for(auto &task : task_list)
        delete task;
    for(auto &bucket : bucket_list)
        delete bucket;
    for(auto &bucket : bucket_list_final)
        delete bucket;
}

void InteractiveRenderer::run(std::vector<OutputDriver *> output_list)
{
    // start chrono
    auto t_start = std::chrono::high_resolution_clock::now();

    // wait for all tasks to be processed
    bool quit = false;
    
    // create the thread pool
    ctpl::thread_pool pool(nbthreads);

    while (!quit)
    {
        // add the tasks to the pool    
        for(auto &bucket : bucket_list){
            Task *task = new Task(bucket);
            task_list.push_back(task);   
            pool.push(render_task, task, this, true);
        }
        for(auto &bucket : bucket_list_final){
            Task *task = new Task(bucket);
            task_list.push_back(task);   
            pool.push(render_task, task, this, false);
        }

        std::list<Task *> tasks_pending(task_list);
        while(tasks_pending.size()>0)
        {
            SDL_Event e;
            if (SDL_PollEvent(&e))
            {
                if (e.type == SDL_MOUSEBUTTONDOWN)
                {
                    for(auto &output : output_list)
                        output->clear();
                    pool.clear_queue();
                    break;
                }
                if (e.type == SDL_QUIT){
                    quit = true;
                    break;
                }
                //If user presses any key
                if (e.type == SDL_KEYDOWN){
                    quit = true;
                    break;
                }
            }
            
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
            {
                for(auto &output : output_list)
                    output->draw();
                usleep(50);
            }
        }
    }

    // end chrono
    auto t_end = std::chrono::high_resolution_clock::now();

    // 
    double time = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    std::cout << "Rendered image in " << time << "ms" << std::endl;
}

//    SDL_MOUSEMOTION    = 0x400, /**< Mouse moved */
//    SDL_MOUSEBUTTONDOWN,        /**< Mouse button pressed */
//    SDL_MOUSEBUTTONUP,          /**< Mouse button released */
//    SDL_MOUSEWHEEL,             /**< Mouse wheel motion */
