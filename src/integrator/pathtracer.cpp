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

PathTracer::PathTracer(const Options &options, std::shared_ptr<Scene> scene) : Integrator(options, scene), width64(width), height64(height)
{}

PathTracer::~PathTracer()
{}

Color PathTracer::render(size_t x, size_t y, size_t sample)
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
    ray.mask = -1;
    ray.time = 0;

    // intersection
    rtcIntersect(scene->rtc_scene,ray);

    // shading
    Vec3fa color = Vec3fa(0.0f);
    if (ray.geomID != RTC_INVALID_GEOMETRY_ID)
    {
        Vec3fa diffuse = Vec3fa(1.0f, 1.0f, 1.0f);
        /*
          if (ray.geomID == 0)
            diffuse = Vec3fa(1.0, 0.0, 0.0);
        else if (ray.geomID == 1)
            diffuse = Vec3fa(1.0, 0.0, 0.0);
        else if (ray.geomID == 2)
            diffuse = Vec3fa(0.0, 1.0, 0.0);
        else if (ray.geomID == 3)
            diffuse = Vec3fa(1.0, 1.0, 0.0);
        else if (ray.geomID == 4)
            diffuse = Vec3fa(0.0, 0.0, 1.0);
        else if (ray.geomID == 5)
            diffuse = Vec3fa(1.0, 0.0, 1.0);
        else if (ray.geomID == 6)
            diffuse = Vec3fa(0.0, 1.0, 1.0);
        */

        /* interpolate diffuse color */
        /*
        if (ray.geomID > 0)
        {
            unsigned int geomID = ray.geomID;
            {
                rtcInterpolate(scene->rtc_scene, geomID, ray.primID, ray.u, ray.v, RTC_USER_VERTEX_BUFFER0, &diffuse.x, &diffuse.y, &diffuse.z, 3);
            }
            diffuse = 0.5f*diffuse;
            }
        */

        color = color + diffuse*0.5f;
        
        /* calculate smooth shading normal */
        Vec3fa Ng = normalize(Vec3fa(ray.Ng[0], ray.Ng[1], ray.Ng[2]));
        Vec3fa lightDir = normalize(Vec3fa(0-(ray.org[0] + ray.tfar*ray.dir[0]),
                                           10-(ray.org[1] + ray.tfar*ray.dir[1]),
                                           0-(ray.org[2] + ray.tfar*ray.dir[2])));
        
        /* initialize shadow ray */
        RTCRay shadow;
        shadow.org[0] = ray.org[0] + ray.tfar*ray.dir[0];
        shadow.org[1] = ray.org[1] + ray.tfar*ray.dir[1];
        shadow.org[2] = ray.org[2] + ray.tfar*ray.dir[2];
        shadow.dir[0] = lightDir[0];
        shadow.dir[1] = lightDir[1];
        shadow.dir[2] = lightDir[2];
        shadow.tnear = 0.001f;
        shadow.tfar = inf;
        shadow.geomID = RTC_INVALID_GEOMETRY_ID;
        shadow.primID = RTC_INVALID_GEOMETRY_ID;
        shadow.mask = -1;
        shadow.time = 0;

        /* trace shadow ray */
        rtcOccluded(scene->rtc_scene, shadow);

        /* add light contribution */
        if (shadow.geomID) {
            color.x = 0;
            color.y = 0;
            color.z = 0;
            //Vec3fa r; // = normalize(reflect(ray.dir,Ng));
            //float s = pow(clamp(dot(r,lightDir),0.0f,1.0f),10.0f);
            //float d = clamp(-dot(lightDir,Ng),0.0f,1.0f);
            //color = color + diffuse*d + 0.5f*Vec3fa(s);
        }
        
    }

    return Color(color.x, color.y, color.z);
}

Color get_radiance()
{
    return Color();
}
