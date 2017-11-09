/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/


#include "camera.hpp"
#include "perspectivecamera.hpp"

Camera::Camera()
{}

Camera::Camera(const Options &options)
{}

Camera::~Camera()
{}

std::unique_ptr<Camera> Camera::create_camera(const Options &options, Logger &logger)
{
    //if (options.perspective)
    return std::unique_ptr<Camera>(new PerspectiveCamera(options));
}
