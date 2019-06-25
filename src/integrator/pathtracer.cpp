/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#include "pathtracer.hpp"
#include "color.hpp"
#include "scene.hpp"
#include "camera.hpp"
#include "options.hpp"

#include <chrono>
#include <unistd.h>
#include <embree2/rtcore_ray.h>
#include <embree2/rtcore_geometry.h>
#include <math/vec3.h>
#include <math/vec3fa.h>
#include <math/affinespace.h>

using namespace embree;

extern AffineSpace3fa instance_xfm[4];
extern LinearSpace3fa normal_xfm[4];
extern Vec3fa colors[4][4];

namespace embree {

__forceinline Vec3f  neg(const Vec3f& a ) { return -a; }
__forceinline Vec3fa neg(const Vec3fa& a) { return -a; }
__forceinline bool   eq (const Vec3fa& a, const Vec3fa& b) { return a == b; }
__forceinline bool   ne (const Vec3fa& a, const Vec3fa& b) { return a != b; }
}

PathTracer::PathTracer(const Options &options, std::shared_ptr<Scene> scene, Logger &logger) : Integrator(options, scene, logger), width64(width), height64(height)
{}

PathTracer::~PathTracer()
{}

Radiance PathTracer::render(size_t x, size_t y, size_t sample)
{
    ISPCCamera camera = scene->camera->getISPCCamera(this->width, this->height);
    //camera.from = Vec3fa(2.244145155f, 1.310973883f, 0.09447964281f);
    //camera.to   = Vec3fa(0.0f,0.0f,0.0f);
      
    RTCRay ray;
    //ray.org = Vec3fa cam_p(camera.xfm.p);
    ray.org[0] = camera.xfm.p[0];
    ray.org[1] = camera.xfm.p[1];
    ray.org[2] = camera.xfm.p[2];
    Vec3fa dir(normalize(x*camera.xfm.l.vx + y*camera.xfm.l.vy + camera.xfm.l.vz));
    ray.dir[0] = dir[0];
    ray.dir[1] = dir[1];
    ray.dir[2] = dir[2];
    ray.tnear = 0.0f;
    ray.tfar = inf;
    ray.geomID = RTC_INVALID_GEOMETRY_ID;
    ray.primID = RTC_INVALID_GEOMETRY_ID;
    ray.instID = -1;
    ray.mask = -1;
    ray.time = 0;

    // intersect ray with scene
    rtcIntersect(scene->rtc_scene, ray);

    /* shade pixels */
    Vec3fa color = Vec3fa(0.0f);
    if (ray.geomID == RTC_INVALID_GEOMETRY_ID) {
        return color;
    }

    /* calculate shading normal in world space */
    Vec3fa Ns = Vec3fa(ray.Ng[0], ray.Ng[1], ray.Ng[2]);
    if (ray.instID != RTC_INVALID_GEOMETRY_ID)
        Ns = xfmVector(normal_xfm[ray.instID],Ns);
    Ns = normalize(Ns);

    /* calculate diffuse color of geometries */
    Vec3fa diffuse = Vec3fa(1,1,1);
    if (ray.instID != RTC_INVALID_GEOMETRY_ID)
        diffuse = colors[ray.instID][ray.geomID];
    color = color + diffuse*0.5;

    /* initialize shadow ray */
    Vec3fa lightDir = normalize(Vec3fa(-1,-1,-1));
    RTCRay shadow;
    shadow.org[0] = ray.org[0] + ray.tfar*ray.dir[0];
    shadow.org[1] = ray.org[1] + ray.tfar*ray.dir[1];
    shadow.org[2] = ray.org[2] + ray.tfar*ray.dir[2];
    Vec3fa negLight = neg(lightDir);
    shadow.dir[0] = negLight.x;
    shadow.dir[1] = negLight.y;
    shadow.dir[2] = negLight.z;
    shadow.tnear = 0.001f;
    shadow.tfar = (float)(inf);
    shadow.geomID = 1;
    shadow.primID = 0;
    shadow.mask = -1;
    shadow.time = 0;

    /* trace shadow ray */
    rtcOccluded(scene->rtc_scene, shadow);

    /* add light contribution */
    if (shadow.geomID)
        color = color + diffuse*clamp(-dot(lightDir,Ns),0.0f,1.0f);

    return color;
}
