/*

***********************************************************
* © Milk VFX, inc. All rights reserved. *
***********************************************************

*/

#include <iostream>

#include <SDL.h>

#include "color.hpp"
#include "displaydriver.hpp"
#include "bucket.hpp"

DisplayDriver::DisplayDriver(size_t width, size_t height) : OutputDriver(width, height), renderer(NULL), texture(NULL), window(NULL), pixeldata(NULL)
{
	//First we need to start up SDL, and make sure it went ok
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
	}
	//Now create a window
	window = SDL_CreateWindow("Test", 100, 100, width, height, SDL_WINDOW_SHOWN);
	//Make sure creating our window went ok
	if (window == nullptr){
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
	}

	//Create a renderer that will draw to the window, -1 specifies that we want to load whichever
	//video driver supports the flags we're passing
	//Flags: SDL_RENDERER_ACCELERATED: We want to use hardware accelerated rendering
	//SDL_RENDERER_PRESENTVSYNC: We want the renderer's present function (update screen) to be
	//synchornized with the monitor's refresh rate
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr){
		SDL_DestroyWindow(window);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
	}

	//SDL 2.0 now uses textures to draw things but SDL_LoadBMP returns a surface
	//this lets us choose when to upload or remove textures from the GPU
	pixeldata = new char[width*height*4];
    clear();   

    //To use a hardware accelerated texture for rendering we can create one from
	//the surface we loaded
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR888, SDL_TEXTUREACCESS_STREAMING, width, height);
	//We no longer need the surface
	//SDL_FreeSurface(surface);
	if (texture == nullptr){
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		std::cout << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
	}

    // first draw to the screen
    update();
}

void DisplayDriver::clear()
{
	for (size_t y=0; y<height; ++y){
	   	for (size_t x=0; x<width; ++x){
			size_t offset = (y*width+x)*4;
            pixeldata[offset+0] = pixeldata[offset+1] = pixeldata[offset+2] = 0;
            pixeldata[offset+3] = (char)255;
		}
	}
}

void DisplayDriver::write(Bucket *bucket)
{
    for (size_t y=0; y<bucket->bucketheight; ++y){
	   	for (size_t x=0; x<bucket->bucketwidth; ++x){
			size_t image_offset  = ((y+bucket->ypos)*width+(x+bucket->xpos))*4;
            size_t bucket_offset = (y*bucket->bucketwidth+x);
            const Color &pixel(bucket->bucketdata[bucket_offset]);
            pixeldata[image_offset+0] = static_cast<char>(pixel.r * 255.0f);
            pixeldata[image_offset+1] = static_cast<char>(pixel.g * 255.0f);
            pixeldata[image_offset+2] = static_cast<char>(pixel.b * 255.0f);
            pixeldata[image_offset+3] = static_cast<char>(255);
		}
	}
}

void DisplayDriver::update()
{
    // SDL_RenderPresent(renderer);
    SDL_UpdateTexture(texture, NULL, pixeldata, width * 4);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void DisplayDriver::loop()
{
    //Our event structure
	SDL_Event e;
	//For tracking if we want to quit
	bool quit = false;
	while (!quit)
    {
		//Read any events that occured, for now we'll just quit if any event occurs
		while (SDL_PollEvent(&e)){
			//If user closes the window
			if (e.type == SDL_QUIT){
				quit = true;
			}
			//If user presses any key
			if (e.type == SDL_KEYDOWN){
				quit = true;
			}
			//If user clicks the mouse
			if (e.type == SDL_MOUSEBUTTONDOWN){
				quit = true;
			}
		}
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
	}
}

DisplayDriver::~DisplayDriver()
{
    delete [] pixeldata;

	//Clean up our objects and quit
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
