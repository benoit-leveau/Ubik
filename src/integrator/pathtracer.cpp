/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#include "pathtracer.hpp"
#include "color.hpp"
#include "scene.hpp"
#include "options.hpp"

#include <chrono>
#include <unistd.h>
#include <embree2/rtcore_ray.h>
#include <embree2/rtcore_geometry.h>
#include <math/vec3.h>
#include <math/vec3fa.h>
#include <math/affinespace.h>

using namespace embree;

__forceinline Vec3f  neg(const Vec3f& a ) { return -a; }
__forceinline Vec3fa neg(const Vec3fa& a) { return -a; }
__forceinline bool   eq (const Vec3fa& a, const Vec3fa& b) { return a == b; }
__forceinline bool   ne (const Vec3fa& a, const Vec3fa& b) { return a != b; }

    /*! OBJ Triangle */
    struct Triangle 
    {
    public:
      Triangle () {}
      
      Triangle (const Triangle& other) 
      : v0(other.v0), v1(other.v1), v2(other.v2), materialID(other.materialID) {}

      Triangle (unsigned v0, unsigned v1, unsigned v2, unsigned materialID) 
      : v0(v0), v1(v1), v2(v2), materialID(materialID) {}

    public:
      unsigned v0, v1, v2, materialID;
    };

  /* camera settings */
  struct Camera 
  {
    struct ISPCCamera
    {
    public:
      ISPCCamera (const AffineSpace3fa& xfm)
      : xfm(xfm) {}

    public:
      AffineSpace3fa xfm;
    };

  public:

    Camera () 
    : from(6.0f,1.0f,0.0f), to(0,0,0), up(0,1,0), fov(60) {}

    Camera (Vec3fa& from, Vec3fa& to, Vec3fa& up, float fov) 
      : from(from), to(to), up(up), fov(fov) {}

    AffineSpace3fa camera2world () { return AffineSpace3fa::lookat(from, to, up); }
    AffineSpace3fa world2camera () { return rcp(AffineSpace3fa::lookat(from, to, up)); }
    Vec3fa world2camera(const Vec3fa& p) { return xfmPoint(world2camera(),p); }
    Vec3fa camera2world(const Vec3fa& p) { return xfmPoint(camera2world(),p); }

    /*AffineSpace3fa pixel2world (size_t width, size_t height) 
    {
      const float fovScale = 1.0f/tanf(deg2rad(0.5f*fov));
      const AffineSpace3fa local2world = AffineSpace3fa::lookat(from, to, up);
      return AffineSpace3fa(local2world.l.vx,
                           -local2world.l.vy,
                           -0.5f*width*local2world.l.vx + 0.5f*height*local2world.l.vy + 0.5f*height*fovScale*local2world.l.vz,
                           local2world.p);
                           }*/

    ISPCCamera getISPCCamera (size_t width, size_t height, bool flip_y = false) 
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

  public:
    Vec3fa from;   //!< position of camera
    Vec3fa to;     //!< look at point
    Vec3fa up;     //!< up vector
    float fov;       //!< field of view
  };

  typedef Camera::ISPCCamera ISPCCamera;

namespace {
    
  /* adds a ground plane to the scene */
  unsigned int addGroundPlane (RTCScene scene_i)
  {
    /* create a triangulated plane with 2 triangles and 4 vertices */
    unsigned int mesh = rtcNewTriangleMesh (scene_i, RTC_GEOMETRY_STATIC, 2, 4);
    
    /* set vertices */
    Vec3fa* vertices = (Vec3fa*) rtcMapBuffer(scene_i,mesh,RTC_VERTEX_BUFFER); 
    vertices[0].x = -10; vertices[0].y = 0; vertices[0].z = -10; 
    vertices[1].x = -10; vertices[1].y = 0; vertices[1].z = +10; 
    vertices[2].x = +10; vertices[2].y = 0; vertices[2].z = -10; 
    vertices[3].x = +10; vertices[3].y = 0; vertices[3].z = +10;
    rtcUnmapBuffer(scene_i,mesh,RTC_VERTEX_BUFFER); 
    
    /* set triangles */
    Triangle* triangles = (Triangle*) rtcMapBuffer(scene_i,mesh,RTC_INDEX_BUFFER);
    triangles[0].v0 = 0; triangles[0].v1 = 2; triangles[0].v2 = 1;
    triangles[1].v0 = 1; triangles[1].v1 = 2; triangles[1].v2 = 3;
    rtcUnmapBuffer(scene_i,mesh,RTC_INDEX_BUFFER);
    
    return mesh;
  }

  /* adds a cube to the scene */
  unsigned int addCube (RTCScene scene_i, const Vec3fa& pos)
  {
    /* create a triangulated cube with 12 triangles and 8 vertices */
    unsigned int mesh = rtcNewTriangleMesh (scene_i, RTC_GEOMETRY_STATIC, 12, 8);
    
    /* set vertices */
    Vec3fa* vertices = (Vec3fa*) rtcMapBuffer(scene_i,mesh,RTC_VERTEX_BUFFER); 
    vertices[0].x = pos.x + -1; vertices[0].y = pos.y + -1; vertices[0].z = pos.z + -1; 
    vertices[1].x = pos.x + -1; vertices[1].y = pos.y + -1; vertices[1].z = pos.z + +1; 
    vertices[2].x = pos.x + -1; vertices[2].y = pos.y + +1; vertices[2].z = pos.z + -1; 
    vertices[3].x = pos.x + -1; vertices[3].y = pos.y + +1; vertices[3].z = pos.z + +1; 
    vertices[4].x = pos.x + +1; vertices[4].y = pos.y + -1; vertices[4].z = pos.z + -1; 
    vertices[5].x = pos.x + +1; vertices[5].y = pos.y + -1; vertices[5].z = pos.z + +1; 
    vertices[6].x = pos.x + +1; vertices[6].y = pos.y + +1; vertices[6].z = pos.z + -1; 
    vertices[7].x = pos.x + +1; vertices[7].y = pos.y + +1; vertices[7].z = pos.z + +1; 
    rtcUnmapBuffer(scene_i,mesh,RTC_VERTEX_BUFFER); 
    
    /* set triangles */
    int tri = 0;
    Triangle* triangles = (Triangle*) rtcMapBuffer(scene_i,mesh,RTC_INDEX_BUFFER);
    
    // left side
    triangles[tri].v0 = 0; triangles[tri].v1 = 2; triangles[tri].v2 = 1; tri++;
    triangles[tri].v0 = 1; triangles[tri].v1 = 2; triangles[tri].v2 = 3; tri++;
    
    // right side
    triangles[tri].v0 = 4; triangles[tri].v1 = 5; triangles[tri].v2 = 6; tri++;
    triangles[tri].v0 = 5; triangles[tri].v1 = 7; triangles[tri].v2 = 6; tri++;
    
    // bottom side
    triangles[tri].v0 = 0; triangles[tri].v1 = 1; triangles[tri].v2 = 4; tri++;
    triangles[tri].v0 = 1; triangles[tri].v1 = 5; triangles[tri].v2 = 4; tri++;
    
    // top side
    triangles[tri].v0 = 2; triangles[tri].v1 = 6; triangles[tri].v2 = 3; tri++;
    triangles[tri].v0 = 3; triangles[tri].v1 = 6; triangles[tri].v2 = 7; tri++;
    
    // front side
    triangles[tri].v0 = 0; triangles[tri].v1 = 4; triangles[tri].v2 = 2; tri++;
    triangles[tri].v0 = 2; triangles[tri].v1 = 4; triangles[tri].v2 = 6; tri++;
    
    // back side
    triangles[tri].v0 = 1; triangles[tri].v1 = 3; triangles[tri].v2 = 5; tri++;
    triangles[tri].v0 = 3; triangles[tri].v1 = 7; triangles[tri].v2 = 5; tri++;
    
    rtcUnmapBuffer(scene_i,mesh,RTC_INDEX_BUFFER);

    std::cout << mesh << std::endl;
    return mesh;
  }    
}

PathTracer::PathTracer(const Options &options, std::shared_ptr<Scene> scene) : Integrator(options, scene), width64(width), height64(height)
{
    rtc_device = rtcNewDevice(NULL);
    rtc_scene = rtcDeviceNewScene(rtc_device, RTC_SCENE_STATIC,RTC_INTERSECT1 | RTC_INTERPOLATE);
    //addGroundPlane(rtc_scene);
    addCube(rtc_scene, Vec3fa(0.0f,0.0f,0.0f));
    addCube(rtc_scene, Vec3fa(2.0f,0.0f,0.0f));
    //addCube(rtc_scene, Vec3fa(2.0f,0.0f,0.0f));
    //addCube(rtc_scene, Vec3fa(3.0f,0.0f,0.0f));
    //addCube(rtc_scene, Vec3fa(1.0f,1.0f,0.0f));
    //addCube(rtc_scene, Vec3fa(2.0f,1.0f,0.0f));
    //addCube(rtc_scene, Vec3fa(3.0f,1.0f,0.0f));
    rtcCommit(rtc_scene);
}

PathTracer::~PathTracer()
{
    rtcDeleteDevice(rtc_device);
}

Color PathTracer::render(size_t x, size_t y, size_t sample)
{
    Camera camera_3d;
    //camera_3d.from[0] += scene->x;
    //camera_3d.from[1] += scene->y;
    ISPCCamera camera = camera_3d.getISPCCamera(this->width, this->height);
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
    rtcIntersect(rtc_scene,ray);

    // shading
    Vec3fa color = Vec3fa(0.0f);
    if (ray.geomID != RTC_INVALID_GEOMETRY_ID)
    {
        if (ray.geomID == 0)
            return Color(1.0, 0.0, 0.0);
        else if (ray.geomID == 1)
            return Color(1.0, 0.0, 0.0);
        else if (ray.geomID == 2)
            return Color(0.0, 1.0, 0.0);
        else if (ray.geomID == 3)
            return Color(1.0, 1.0, 0.0);
        else if (ray.geomID == 4)
            return Color(0.0, 0.0, 1.0);
        else if (ray.geomID == 5)
            return Color(1.0, 0.0, 1.0);
        else if (ray.geomID == 6)
            return Color(0.0, 1.0, 1.0);
        
        /* interpolate diffuse color */
        Vec3fa diffuse = Vec3fa(1.0f, 1.0f, 1.0f);
        if (ray.geomID > 0)
        {
            unsigned int geomID = ray.geomID;
            {
                rtcInterpolate(rtc_scene, geomID, ray.primID, ray.u, ray.v, RTC_USER_VERTEX_BUFFER0, &diffuse.x, nullptr, nullptr, 3);
            }
            diffuse = 0.5f*diffuse;
        }

        color = color + diffuse*0.5f;
        /*
        /-* calculate smooth shading normal *-/
        Vec3fa Ng = normalize(Vec3fa(ray.Ng[0], ray.Ng[1], ray.Ng[2]));
        Vec3fa lightDir = normalize(Vec3fa(-1,-1,-1));
        
        /-* initialize shadow ray *-/
        RTCRay shadow;
        //shadow.org = ray.org + ray.tfar*ray.dir;
        //shadow.dir = neg(lightDir);
        shadow.tnear = 0.001f;
        shadow.tfar = inf;
        shadow.geomID = 1;
        shadow.primID = 0;
        shadow.mask = -1;
        shadow.time = 0;

        /-* trace shadow ray *-/
        rtcOccluded(rtc_scene, shadow);

        /-* add light contribution *-/
        if (shadow.geomID) {
            Vec3fa r; // = normalize(reflect(ray.dir,Ng));
            float s = pow(clamp(dot(r,lightDir),0.0f,1.0f),10.0f);
            float d = clamp(-dot(lightDir,Ng),0.0f,1.0f);
            color = color + diffuse*d + 0.5f*Vec3fa(s);
        }
        */
    }

    return Color(color.x, color.y, color.z);
}

Color get_radiance()
{
    return Color();
}
