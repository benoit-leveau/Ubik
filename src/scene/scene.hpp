/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef SCENE_H
#define SCENE_H

#include <cstddef>
#include <vector>
#include <memory>
#include <embree2/rtcore.h>

// forward declaration
class Camera;
class Logger;
class Options;

class Scene
{
public:
    Scene(const Options &options, Logger &logger);
    ~Scene();

    void load();
    void update();
    
    //
    RTCDevice rtc_device;
    RTCScene rtc_scene, rtc_scene1;    
    size_t frame;
    std::shared_ptr<Camera> camera;
};

#endif // !SCENE_H
