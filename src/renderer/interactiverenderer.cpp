/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#include <iostream>
#include <chrono>
#include <unistd.h>
#include <atomic>
#include <thread>

#include <SDL.h>

#include "ctpl.h"

#include "interactiverenderer.hpp"
#include "outputdriver.hpp"
#include "displaydriver.hpp"
#include "scene.hpp"
#include "sampler.hpp"
#include "color.hpp"
#include "options.hpp"

#include <cmath>

struct Bucket
{
    Bucket(size_t pos_x, size_t pos_y, size_t size_x, size_t size_y) : 
        pos_x(pos_x), pos_y(pos_y), size_x(size_x), size_y(size_y), sample(0), color(0,0,0), copied(false)
    {
        center_x = pos_x + size_x / 2;
        center_y = pos_y + size_y / 2;
    }

    size_t pos_x, pos_y;
    size_y size_x, size_y;
    size_t center_x, center_y;

    Color color;
    size_t scene_version;
    bool copied;
};

std::atomic<int> scene_version = ATOMIC_VAR_INIT(0);
std::atomic<int> current_bucket = ATOMIC_VAR_INIT(0);
volatile bool threads_stop = false;
std::vector<Bucket *> bucket_list;

void InteractiveRenderer::create_buckets()
{    
    size_t level = 0;
    size_t current = 0;
    size_t width_level = this->width;
    size_t height_level = this->height;
    size_t bucket_offset = 0;
    while(true)
    {
        current += pow(4,level);
        if (pixel_index < int(current))
        {
            break;
        }
        bucket_offset = current;
        ++level;
        width_level = width_level / 2;
        height_level = height_level / 2;
        for (size_t bucket_index_y=0; bucket_index_y<int(pow(2, level)); ++bucket_index_y)
        {
            for (size_t bucket_index_x=0; bucket_index_x<int(pow(2, level)); ++bucket_index_x)
            {
                posx = bucket_index_x * sizex;
                posy = bucket_index_y * sizey;
                center_x = pos_x + width_level / 2;
                center_y = pos_y + height_level / 2;
                bucket_list.push_back(new Bucket(posx, posy, width_level, height_level));
            }
        }
    }
}

void render_loop(size_t thread_index, InteractiveRenderer *renderer)
{
    while (not threads_stop)
    {
        int bucket_index = std::atomic_fetch_add(&current_bucket, 1);
        if (bucket_index == -1)
        {
            usleep(10);
            continue;
        }
        Bucket *bucket = bucket_list[bucket_index];

        // retrieve the current version of the scene
        int my_scene_version = scene_version.load();

        Color result = renderer->sampler->render(bucket->center_x, bucket->center_y);
        
        // 
        if (my_scene_version != scene_version.load())
        {
            // ditch the result, it was computed for an earlier version of the scene
            continue;
        }

        // store the result somewhere, along with number of samples, and scene iteration count
        bucket->color = result;
    }
}

InteractiveRenderer::InteractiveRenderer(std::shared_ptr<Scene> scene, const Options &options) :
    Renderer(scene, options)
{
    display = std::unique_ptr<DisplayDriver>(new DisplayDriver(options.width, options.height));
}

InteractiveRenderer::~InteractiveRenderer()
{
}

void InteractiveRenderer::run()
{
    create_buckets();

    std::vector<std::thread> threads;

    std::cout << "Starting interactive render with " << nbthreads << " threads" << std::endl;

    for(size_t thread_index=0; thread_index<nbthreads; ++thread_index)
    {
        threads.push_back(std::thread(render_loop, thread_index, this));
    }

    bool mouse_button_down = false;
    while(true)
    {
        bool skip_sleep = false;
        SDL_Event e;
        if (SDL_PollEvent(&e))
        {
            if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                mouse_button_down = true;
                skip_sleep = true;
            }
            else if (e.type == SDL_MOUSEBUTTONUP)
            {
                mouse_button_down = false;
                skip_sleep = true;
            }
            else if (e.type == SDL_MOUSEMOTION)
            {
                skip_sleep = true;
                if (mouse_button_down)
                {
                    std::atomic_store(&current_pixel, -1);
                    scene->x += e.motion.xrel;
                    scene->y += e.motion.yrel;
                    std::atomic_fetch_add(&scene_version, 1);
                    for(auto &bucket : bucket_list){
                        bucket->color = Color(0,0,0);
                        bucket->scene_version = scene_version;
                        bucket->sample = 0;
                        bucket->copied = false;
                    }
                    std::atomic_store(&current_pixel, 0);
                    //display->clear();
                    //display->update();
                    //continue;
                }
            }
            if (e.type == SDL_QUIT)
            {
                break;
            }
            if (e.type == SDL_KEYDOWN)
            {
                break;
            }
        }
        
        for(auto &bucket : bucket_list)
        {
            if (bucket->sample>0 && !bucket->copied)
            {
                for (size_t y=bucket->pos_y; y<bucket->pos_y+bucket->size_y; ++y)
                {
                    for (size_t x=bucket->pos_x; x<bucket->pos_x+bucket->size_x; ++x)
                    {
                        this->display->write_pixel(x, y, bucket->color);
                    }
                }
                bucket->copied = true;
            }
        }
        display->update();
        if (not skip_sleep)
            usleep(20);
    }

    // tell the threads to stop
    threads_stop = true;

    for(auto& thread : threads)
    {
        thread.join();
    }
}
