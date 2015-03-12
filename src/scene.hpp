/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef SCENE_H
#define SCENE_H

#include <cstddef>


class Scene
{
public:
    Scene() : frame(0), x(0), y(0){}
    size_t frame;
    int x;
    int y;
};

#endif // !SCENE_H
