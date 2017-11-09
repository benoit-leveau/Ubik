/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <embree2/rtcore_ray.h>
#include <embree2/rtcore_geometry.h>
#include <math/vec3.h>
#include <math/vec3fa.h>
#include <math/affinespace.h>

// forward declaration
class Options;
class Logger;

struct ISPCCamera
{
public:
    ISPCCamera (const embree::AffineSpace3fa& xfm)
        : xfm(xfm) {}
public:
    embree::AffineSpace3fa xfm;
};

class Camera
{
public:
    Camera();
    Camera(const Options &options);
    virtual ~Camera() = 0;

    static std::unique_ptr<Camera> create_camera(const Options &options, Logger &logger);
    
    virtual embree::AffineSpace3fa camera2world() = 0;
    virtual embree::AffineSpace3fa world2camera() = 0;
    virtual embree::Vec3fa world2camera(const embree::Vec3fa& p) = 0;
    virtual embree::Vec3fa camera2world(const embree::Vec3fa& p) = 0;
        
    virtual void move (float dx, float dy, float dz) = 0;
    virtual void rotate (float dtheta, float dphi) = 0;
    virtual void rotateOrbit (float dtheta, float dphi) = 0;
    virtual void dolly (float ds) = 0;

    virtual ISPCCamera getISPCCamera (size_t width, size_t height, bool flip_y = false) = 0;
    
private:
};


#endif // !CAMERA_HPP
