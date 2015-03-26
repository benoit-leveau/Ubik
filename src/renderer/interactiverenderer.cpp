/*

***********************************************************
* � Benoit Leveau - All rights reserved. *
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
#include "integrator.hpp"
#include "scene.hpp"
#include "color.hpp"
#include "options.hpp"
#include "arrayiterator.hpp"

#include <cmath>

std::atomic<int> global_scene_version = ATOMIC_VAR_INIT(0);
std::atomic<int> current_bucket = ATOMIC_VAR_INIT(0);
volatile bool threads_stop = false;
std::vector<Bucket *> bucket_list;

struct Pixel
{
    //std::atomic<int> sample;
    Color color;
    Pixel()
    {
        //sample = ATOMIC_VAR_INIT(0);
    }
};

struct Bucket
{
    Bucket(std::shared_ptr<Integrator> integrator, size_t pos_x, size_t pos_y, size_t bucket_width, size_t bucket_height) : integrator(integrator), pos_x(pos_x), pos_y(pos_y), bucket_width(bucket_width), bucket_height(bucket_height), copied(false), completed(false), scene_version(-1)
    {}

    virtual void render() = 0;
    virtual void write(DisplayDriver *driver) = 0;
    virtual void reset()
    {
        copied = false;
        completed = false;
    }

    std::shared_ptr<Integrator> integrator;
    size_t pos_x, pos_y;
    size_t bucket_width, bucket_height;
    bool copied;
    bool completed;
    int scene_version;
};

struct HighBucket : Bucket
{
    HighBucket(std::shared_ptr<Integrator> integrator, size_t pos_x, size_t pos_y, size_t bucket_width, size_t bucket_height) : 
        Bucket(integrator, pos_x, pos_y, bucket_width, bucket_height)//,sample(0)
    {
        bucketdata = new Pixel[bucket_width*bucket_height]();
        bucket_sample = 0; // ATOMIC_VAR_INIT(0);
        bucket_lock = ATOMIC_VAR_INIT(0);
    }

    virtual void render()
    {
        int expected = 0;
        while (std::atomic_compare_exchange_weak(&bucket_lock, &expected, 1))
            usleep(5);
        // int sample = bucket_sample.load();
        int my_scene_version = global_scene_version.load();
        for (size_t y=0; y<bucket_height; ++y)
        {
            if (my_scene_version != global_scene_version.load())
                return;
            for (size_t x=0; x<bucket_width; ++x)
            {
                if (threads_stop)
                    return;
                size_t offset = y*bucket_width+x;
                // if (sample == 0)
                //    bucketdata[offset] = Color();
                Pixel &pixel(bucketdata[offset]);
                // int sample = std::atomic_fetch_add(&pixel.sample, 1);
                pixel.color += integrator->render(x+pos_x, y+pos_y, bucket_sample);
            }
        }
        //if (my_scene_version != global_scene_version.load())
        //    return;
        // ++sample;
        ++bucket_sample;
        // std::atomic_fetch_add(&bucket_sample, 1);
        copied = false;
        completed = true;
        scene_version = my_scene_version;
        std::atomic_store(&bucket_lock, 0);
    }

    virtual void reset()
    {
        int expected = 0;
        while (std::atomic_compare_exchange_weak(&bucket_lock, &expected, 1))
            usleep(5);
        this->Bucket::reset();
        // sample = 0;
        for (size_t y=0; y<bucket_height; ++y)
        {
            for (size_t x=0; x<bucket_width; ++x)
            {
                Pixel &pixel(bucketdata[y*bucket_width+x]);
                // std::atomic_store(&pixel.sample, 0);
                pixel.color = Color();
            }
        }
        bucket_sample = 0;// std::atomic_store(&bucket_sample, 0);
        std::atomic_store(&bucket_lock, 0);
    }

    virtual void write(DisplayDriver *driver)
    {
        int expected = 0;
        while (std::atomic_compare_exchange_weak(&bucket_lock, &expected, 1))
            usleep(5);
        // int current_sample = bucket_sample.load();
        for (size_t y=0; y<bucket_height; ++y)
        {
            for (size_t x=0; x<bucket_width; ++x)
            {
                Pixel &pixel(bucketdata[y*bucket_width+x]);
                // int current_sample = pixel.sample.load();
                Color color = pixel.color;
                color /= float(bucket_sample);
                driver->write_pixel(x+pos_x, y+pos_y, color);
            }
        }
        std::atomic_store(&bucket_lock, 0);
    }

    Pixel *bucketdata;
    // std::atomic<int> bucket_sample;
    volatile int bucket_sample;
    std::atomic<int> bucket_lock;
    // std::atomic<int> sample;
    // size_t sample;
};

struct LowBucket : Bucket
{
    LowBucket(std::shared_ptr<Integrator> integrator, size_t pos_x, size_t pos_y, size_t bucket_width, size_t bucket_height) : 
        Bucket(integrator, pos_x, pos_y, bucket_width, bucket_height),
        color(0, 0, 0)
    {
        center_x = pos_x + bucket_width / 2;
        center_y = pos_y + bucket_height / 2;
    }

    virtual void render()
    {
        int my_scene_version = global_scene_version.load();
        color = integrator->render(center_x, center_y, 0);
        copied = false;
        completed = true;
        scene_version = my_scene_version;
    }

    virtual void write(DisplayDriver *driver)
    {
        for (size_t y=pos_y; y<pos_y+bucket_height; ++y)
            for (size_t x=pos_x; x<pos_x+bucket_width; ++x)
                driver->write_pixel(x, y, color);
    }

    size_t center_x, center_y;
    Color color;
};

Bucket *create_bucket(std::shared_ptr<Integrator> integrator, size_t pos_x, size_t pos_y, size_t bucket_width, size_t bucket_height, bool high)
{
    if (high)
        return new HighBucket(integrator, pos_x, pos_y, bucket_width, bucket_height);
    else
        return new LowBucket(integrator, pos_x, pos_y, bucket_width, bucket_height);
}

void InteractiveRenderer::create_buckets()
{
    size_t bucket_size = 32;
    bool highres = false;
    while(true)
    {
        int number_tiles_x = ceil(float(width) / float(bucket_size));
        int number_tiles_y = ceil(float(height) / float(bucket_size));
        if (highres)
            full_tiles = number_tiles_x * number_tiles_y;
        ArrayIterator it(number_tiles_x, number_tiles_y, ArrayIterationMode::SPIRAL);
        while (it.valid)
        {
            auto pair = it.next();
            size_t pos_x = pair.first * bucket_size;
            size_t pos_y = pair.second * bucket_size;
            bucket_list.push_back(create_bucket(integrator,
                                                pos_x,
                                                pos_y,
                                                std::min(bucket_size, width-pos_x),
                                                std::min(bucket_size, height-pos_y),
                                                highres));
        }
        if (highres)
            break;
        if (bucket_size > 8)
            bucket_size /= 2;
        else
        {
            progressive_tiles = bucket_list.size();
            highres = true;
            bucket_size = this->bucketsize;
        }
    }
}

void render_loop(size_t /*thread_index*/, InteractiveRenderer *renderer)
{
    size_t max_size = bucket_list.size();
    while (not threads_stop)
    {
        int bucket_index = std::atomic_fetch_add(&current_bucket, 1);
        if (bucket_index == -1) //  || bucket_index >= int(max_size))
        {
            usleep(5);
            continue;
        }
        
        if (bucket_index >= (renderer->progressive_tiles+renderer->full_tiles))
        {
            bucket_index = renderer->progressive_tiles + (bucket_index - renderer->progressive_tiles) % renderer->full_tiles;
            //int expected_sample = (bucket_index - renderer->progressive_tiles) / renderer->full_tiles;
            //HighBucket *hb = dynamic_cast<HighBucket *>(bucket_list[bucket_index]);
            //while (expected_sample != hb->bucket_sample.load())
            //    usleep(10);
        }

        Bucket *bucket = bucket_list[bucket_index];

        // retrieve the current version of the scene
        // int my_scene_version = scene_version.load();

        bucket->render();
        
        // 
        //if (my_scene_version != scene_version.load())
        //{
            // ditch the result, it was computed for an earlier version of the scene
            //continue;
        //}

        // store the result somewhere, along with number of samples, and scene iteration count
        // bucket->color = result;
    }
}

InteractiveRenderer::InteractiveRenderer(std::shared_ptr<Scene> scene, const Options &options) :
    Renderer(scene, options),
    bucketsize(options.bucketsize),
    progressive_tiles(0),
    full_tiles(0)
{
    display = std::unique_ptr<DisplayDriver>(new DisplayDriver(options.width, options.height));
}

InteractiveRenderer::~InteractiveRenderer()
{
}

void InteractiveRenderer::run()
{
    int scene_version = 0;

    create_buckets();

    std::vector<std::thread> threads;

    std::cout << "Starting interactive render with " << nbthreads << " threads" << std::endl;

    for(size_t thread_index=0; thread_index<nbthreads; ++thread_index)
    {
        threads.push_back(std::thread(render_loop, thread_index, this));
    }

    bool mouse_button_down = false;
    bool quit = false;
    while(!quit)
    {
        auto t_start = std::chrono::high_resolution_clock::now();
        bool skip_sleep = false;
        SDL_Event e;
        bool reset_scene = false;
        while (SDL_PollEvent(&e))
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
                    reset_scene = true;
                    std::atomic_store(&current_bucket, -1);
                    scene->x += e.motion.xrel;
                    scene->y += e.motion.yrel;
                    std::atomic_fetch_add(&global_scene_version, 1);
                    scene_version = global_scene_version.load();
                    for(auto &bucket : bucket_list){
                        bucket->reset();
                    }
                    std::atomic_store(&current_bucket, 0);
                }
            }
            if (e.type == SDL_QUIT)
            {
                quit = true;
                break;
            }
            //if (e.type == SDL_KEYDOWN)
            //{
            //    break;
            //}
        }
        if (quit)
            break;
        bool updated = false;
        //if (reset_scene)
        //    display->clear();
        for(auto &bucket : bucket_list)
        {
            if (bucket->completed && !bucket->copied && scene_version==bucket->scene_version)
            {
                bucket->write(this->display.get());
                bucket->copied = true;
                updated = true;                
            }
            auto t_end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double, std::milli>(t_end-t_start).count();
            if (updated && time > 20)
                break;
        }
        if (updated)
            display->update();
        else
            usleep(20);
    }

    // tell the threads to stop
    threads_stop = true;

    for(auto& thread : threads)
    {
        thread.join();
    }
}
