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

// integrate the renderer with SDL, no additional output driver
// make the tasks interruptible (especially the bucket ones)


std::atomic<int> current_pixel = ATOMIC_VAR_INIT(16);
volatile bool threads_stop = false;

void render_loop(size_t thread_index, InteractiveRenderer *renderer)
{
    while (not threads_stop)
    {
        int pixel_index = std::atomic_fetch_add(&current_pixel, 1);
        size_t level = 0;
        size_t current = 0;
        size_t width_level = renderer->width;
        size_t height_level = renderer->height;
        size_t bucket_index = 0;
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
        }
        bucket_index = pixel_index - bucket_offset;
        size_t bucket_index_y = bucket_index / int(pow(2, level));
        size_t bucket_index_x = bucket_index % int(pow(2, level));
        size_t pos_x = bucket_index_x * width_level;
        size_t pos_y = bucket_index_y * height_level;
        size_t center_x = pos_x + width_level / 2;
        size_t center_y = pos_y + height_level / 2;
        Color result = renderer->sampler->render(renderer->integrator, renderer->scene, center_x, center_y);
        for (size_t y=pos_y; y<pos_y+height_level; ++y)
        {
            for (size_t x=pos_x; x<pos_x+width_level; ++x)
            {
                renderer->display->write_pixel(x, y, result);
            }
        }
        // usleep(1000);
        // pixel_index = pixel_index % (renderer->width * renderer->height);
        // size_t x = pixel_index % renderer->width;
        // size_t y = pixel_index / renderer->width;
        // Color result = renderer->sampler->render(renderer->integrator, renderer->scene, x, y);
        // renderer->display->write_pixel(x, y, result);
    }
}

InteractiveRenderer::InteractiveRenderer(Scene *scene, const Options &options) :
    Renderer(scene, options)
{
}

InteractiveRenderer::~InteractiveRenderer()
{
}

void InteractiveRenderer::run(std::vector<OutputDriver *> output_list)
{
    this->display = static_cast<DisplayDriver *>(output_list[0]);

    std::vector<std::thread> threads;

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
                    scene->x -= e.motion.xrel;
                    scene->y -= e.motion.yrel;
                    // restart from 0
                    std::atomic_store(&current_pixel, 16);
                    display->clear();
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

//    SDL_MOUSEMOTION    = 0x400, /**< Mouse moved */
//    SDL_MOUSEBUTTONDOWN,        /**< Mouse button pressed */
//    SDL_MOUSEBUTTONUP,          /**< Mouse button released */
//    SDL_MOUSEWHEEL,             /**< Mouse wheel motion */

