/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/

#ifndef PERSPECTIVECAMERA_HPP
#define PERSPECTIVECAMERA_HPP

#include "camera.hpp"

// forward declaration
class Options;

class PerspectiveCamera : public Camera
{
public:
    PerspectiveCamera(const Options &options);
    PerspectiveCamera(embree::Vec3fa& from, embree::Vec3fa& to, embree::Vec3fa& up, float fov);
    
    ~PerspectiveCamera();

    embree::AffineSpace3fa camera2world();
    embree::AffineSpace3fa world2camera();
    embree::Vec3fa world2camera(const embree::Vec3fa& p);
    embree::Vec3fa camera2world(const embree::Vec3fa& p);
        
    void move (float dx, float dy, float dz);
    void rotate (float dtheta, float dphi);
    void rotateOrbit (float dtheta, float dphi);
    void dolly (float ds);

    ISPCCamera getISPCCamera (size_t width, size_t height, bool flip_y = false);
    //
    embree::Vec3fa from;   //!< position of camera
    embree::Vec3fa to;     //!< look at point
    embree::Vec3fa up;     //!< up vector
    float fov;       //!< field of view
    
private:

};

#endif // !PERSPECTIVECAMERA_HPP
