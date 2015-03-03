/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef DISPLAYDRIVER_H
#define DISPLAYDRIVER_H

#include <cstddef>
#include "outputdriver.hpp"

// forward declaration
class SDL_Renderer;
class SDL_Texture;
class SDL_Window;


class DisplayDriver : public OutputDriver
{
public:
    DisplayDriver(size_t width, size_t height);
    ~DisplayDriver();

    void write(Bucket *bucket);
    void update();
    void loop();
private:
    void clear();

    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Window *window;
    char *pixeldata;
};

#endif // !DISPLAYDRIVER_H
