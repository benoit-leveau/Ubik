/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/


#include "perspectivecamera.hpp"
#include <embree2/rtcore_ray.h>
#include <embree2/rtcore_geometry.h>
#include <math/vec3.h>
#include <math/vec3fa.h>
#include <math/affinespace.h>

using namespace embree;

PerspectiveCamera::PerspectiveCamera(const Options &options) :
    Camera(options), from(0.0001f,0.0001f,-3.0f), to(0,0,0), up(0,1,0), fov(90)
{}

PerspectiveCamera::~PerspectiveCamera()
{}

AffineSpace3fa PerspectiveCamera::camera2world ()
{
    return AffineSpace3fa::lookat(from, to, up);
}

AffineSpace3fa PerspectiveCamera::world2camera ()
{
    return rcp(AffineSpace3fa::lookat(from, to, up));
}

Vec3fa PerspectiveCamera::world2camera(const Vec3fa& p)
{
    return xfmPoint(world2camera(),p);
}

Vec3fa PerspectiveCamera::camera2world(const Vec3fa& p)
{
    return xfmPoint(camera2world(),p);
}

void PerspectiveCamera::move (float dx, float dy, float dz)
{
    AffineSpace3fa xfm = camera2world();
    Vec3fa ds = xfmVector(xfm,Vec3fa(dx,dy,dz));
    from += ds;
    to   += ds;
}

void PerspectiveCamera::rotate (float dtheta, float dphi)
{
    const Vec3fa up1 = normalize(up);
    Vec3fa view1 = normalize(to-from);
    view1 = xfmVector(AffineSpace3fa::rotate(up1, dtheta), view1);
    const float phi = acosf(dot(view1, up1));
    const float dphi2 = phi - clamp(phi-dphi, 0.001f*float(pi), 0.999f*float(pi));
    view1 = xfmVector(AffineSpace3fa::rotate(cross(view1, up1), dphi2), view1);
    to = from + length(to-from) * view1;
}

void PerspectiveCamera::rotateOrbit (float dtheta, float dphi)
{
    const Vec3fa up1 = normalize(up);
    Vec3fa view1 = normalize(to-from);
    view1 = xfmVector(AffineSpace3fa::rotate(up1, dtheta), view1);
    const float phi = acosf(dot(view1, up1));
    const float dphi2 = phi - clamp(phi-dphi, 0.001f*float(pi), 0.999f*float(pi));
    view1 = xfmVector(AffineSpace3fa::rotate(cross(view1, up1), dphi2), view1);
    from = to - length(to-from) * view1;
}

void PerspectiveCamera::dolly (float ds)
{
    float dollySpeed = 0.01f;
    float k = powf((1.0f-dollySpeed), ds);
    from += length(to-from) * (1-k) * normalize(to-from);
}

ISPCCamera PerspectiveCamera::getISPCCamera(size_t width, size_t height, bool flip_y/*=false*/) 
{
    const float fovScale = 1.0f/tanf(deg2rad(0.5f*fov));
    const AffineSpace3fa local2world = AffineSpace3fa::lookat(from, to, up);
    Vec3fa vx = local2world.l.vx;
    Vec3fa vy = -local2world.l.vy;
    Vec3fa vz = -0.5f*width*local2world.l.vx + 0.5f*height*local2world.l.vy + 0.5f*height*fovScale*local2world.l.vz;
    Vec3fa p =  local2world.p;
    if (flip_y) {
        vz = vz+float(height)*vy;
        vy = -vy;
    }
    return ISPCCamera(AffineSpace3fa(vx,vy,vz,p));
}
