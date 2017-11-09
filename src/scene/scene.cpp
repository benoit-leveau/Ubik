/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/


#include "options.hpp"
#include "perspectivecamera.hpp"
#include "logging.hpp"
#include "scene.hpp"
#include <embree2/rtcore_ray.h>
#include <embree2/rtcore_geometry.h>
#include <math/vec3.h>
#include <math/vec3fa.h>
#include <math/affinespace.h>

using namespace embree;

/* error reporting function */
void error_handler(void* userPtr, const RTCError code, const char* str = nullptr)
{
  if (code == RTC_NO_ERROR)
    return;

  printf("Embree: ");
  switch (code) {
  case RTC_UNKNOWN_ERROR    : printf("RTC_UNKNOWN_ERROR"); break;
  case RTC_INVALID_ARGUMENT : printf("RTC_INVALID_ARGUMENT"); break;
  case RTC_INVALID_OPERATION: printf("RTC_INVALID_OPERATION"); break;
  case RTC_OUT_OF_MEMORY    : printf("RTC_OUT_OF_MEMORY"); break;
  case RTC_UNSUPPORTED_CPU  : printf("RTC_UNSUPPORTED_CPU"); break;
  case RTC_CANCELLED        : printf("RTC_CANCELLED"); break;
  default                   : printf("invalid error code"); break;
  }
  if (str) {
    printf(" (");
    while (*str) putchar(*str++);
    printf(")\n");
  }
  exit(1);
}

unsigned int g_instance0 = -1;
unsigned int g_instance1 = -1;
unsigned int g_instance2 = -1;
unsigned int g_instance3 = -1;
AffineSpace3fa instance_xfm[4];

const int numPhi = 5;
const int numTheta = 2*numPhi;

struct Vertex   { float x,y,z,a;  }; // FIXME: rename to Vertex4f
struct Triangle { int v0, v1, v2; };

unsigned int createTriangulatedSphere (RTCScene scene, const Vec3fa& p, float r)
{
  /* create triangle mesh */
  unsigned int mesh = rtcNewTriangleMesh (scene, RTC_GEOMETRY_STATIC, 2*numTheta*(numPhi-1), numTheta*(numPhi+1));

  /* map triangle and vertex buffers */
  Vertex* vertices = (Vertex*) rtcMapBuffer(scene,mesh,RTC_VERTEX_BUFFER);
  Triangle* triangles = (Triangle*) rtcMapBuffer(scene,mesh,RTC_INDEX_BUFFER);

  /* create sphere */
  int tri = 0;
  const float rcpNumTheta = rcp((float)numTheta);
  const float rcpNumPhi   = rcp((float)numPhi);
  for (int phi=0; phi<=numPhi; phi++)
  {
    for (int theta=0; theta<numTheta; theta++)
    {
      const float phif   = phi*float(pi)*rcpNumPhi;
      const float thetaf = theta*2.0f*float(pi)*rcpNumTheta;

      Vertex& v = vertices[phi*numTheta+theta];
      v.x = p.x + r*sin(phif)*sin(thetaf);
      v.y = p.y + r*cos(phif);
      v.z = p.z + r*sin(phif)*cos(thetaf);
    }
    if (phi == 0) continue;

    for (int theta=1; theta<=numTheta; theta++)
    {
      int p00 = (phi-1)*numTheta+theta-1;
      int p01 = (phi-1)*numTheta+theta%numTheta;
      int p10 = phi*numTheta+theta-1;
      int p11 = phi*numTheta+theta%numTheta;

      if (phi > 1) {
        triangles[tri].v0 = p10;
        triangles[tri].v1 = p00;
        triangles[tri].v2 = p01;
        tri++;
      }

      if (phi < numPhi) {
        triangles[tri].v0 = p11;
        triangles[tri].v1 = p10;
        triangles[tri].v2 = p01;
        tri++;
      }
    }
  }
  rtcUnmapBuffer(scene,mesh,RTC_VERTEX_BUFFER);
  rtcUnmapBuffer(scene,mesh,RTC_INDEX_BUFFER);
  return mesh;
}

/* creates a ground plane */
unsigned int createGroundPlane (RTCScene scene)
{
  /* create a triangulated plane with 2 triangles and 4 vertices */
  unsigned int mesh = rtcNewTriangleMesh (scene, RTC_GEOMETRY_STATIC, 2, 4);

  /* set vertices */
  Vertex* vertices = (Vertex*) rtcMapBuffer(scene,mesh,RTC_VERTEX_BUFFER);
  vertices[0].x = -10; vertices[0].y = -2; vertices[0].z = -10;
  vertices[1].x = -10; vertices[1].y = -2; vertices[1].z = +10;
  vertices[2].x = +10; vertices[2].y = -2; vertices[2].z = -10;
  vertices[3].x = +10; vertices[3].y = -2; vertices[3].z = +10;
  rtcUnmapBuffer(scene,mesh,RTC_VERTEX_BUFFER);

  /* set triangles */
  Triangle* triangles = (Triangle*) rtcMapBuffer(scene,mesh,RTC_INDEX_BUFFER);
  triangles[0].v0 = 0; triangles[0].v1 = 2; triangles[0].v2 = 1;
  triangles[1].v0 = 1; triangles[1].v1 = 2; triangles[1].v2 = 3;
  rtcUnmapBuffer(scene,mesh,RTC_INDEX_BUFFER);

  return mesh;
}


Scene::Scene(const Options &options, Logger &logger) :
    frame(0),
    camera(nullptr)
{
    camera = Camera::create_camera(options, logger);
    rtc_device = rtcNewDevice(NULL);
    error_handler(nullptr,rtcDeviceGetError(rtc_device));
    rtcDeviceSetErrorFunction2(rtc_device, error_handler, nullptr);
    rtc_scene = rtcDeviceNewScene(rtc_device, RTC_SCENE_DYNAMIC, RTC_INTERSECT1 /*| RTC_INTERPOLATE*/);
}

void Scene::load()
{
    rtc_scene1 = rtcDeviceNewScene(rtc_device, RTC_SCENE_STATIC,RTC_INTERSECT1 /*| RTC_INTERPOLATE*/);
    createTriangulatedSphere(rtc_scene1,Vec3fa( 0, 0,+1),0.5f);
    createTriangulatedSphere(rtc_scene1,Vec3fa(+1, 0, 0),0.5f);
    createTriangulatedSphere(rtc_scene1,Vec3fa( 0, 0,-1),0.5f);
    createTriangulatedSphere(rtc_scene1,Vec3fa(-1, 0, 0),0.5f);
    rtcCommit(rtc_scene1);
    
    g_instance0 = rtcNewInstance2(rtc_scene,rtc_scene1,1);
    g_instance1 = rtcNewInstance2(rtc_scene,rtc_scene1,1);
    g_instance2 = rtcNewInstance2(rtc_scene,rtc_scene1,1);
    g_instance3 = rtcNewInstance2(rtc_scene,rtc_scene1,1);
    createGroundPlane(rtc_scene);
    update();
    rtcCommit(rtc_scene);
}

void Scene::update()
{
    float t0 = 0.7f * frame;
    float t1 = 1.5f * frame;
    LinearSpace3fa xfm;
    xfm.vx = Vec3fa(cos(t1),0,sin(t1));
    xfm.vy = Vec3fa(0,1,0);
    xfm.vz = Vec3fa(-sin(t1),0,cos(t1));
    for (int i=0; i<4; i++) {
        float t = t0+i*2.0f*float(pi)/4.0f;
        instance_xfm[i] = AffineSpace3fa(xfm,2.2f*Vec3fa(+cos(t),0.0f,+sin(t)));
    }

    /* set instance transformations */
    rtcSetTransform2(rtc_scene,g_instance0,RTC_MATRIX_COLUMN_MAJOR_ALIGNED16,(float*)&instance_xfm[0],0);
    rtcSetTransform2(rtc_scene,g_instance1,RTC_MATRIX_COLUMN_MAJOR_ALIGNED16,(float*)&instance_xfm[1],0);
    rtcSetTransform2(rtc_scene,g_instance2,RTC_MATRIX_COLUMN_MAJOR_ALIGNED16,(float*)&instance_xfm[2],0);
    rtcSetTransform2(rtc_scene,g_instance3,RTC_MATRIX_COLUMN_MAJOR_ALIGNED16,(float*)&instance_xfm[3],0);

    /* update scene */
    rtcUpdate(rtc_scene,g_instance0);
    rtcUpdate(rtc_scene,g_instance1);
    rtcUpdate(rtc_scene,g_instance2);
    rtcUpdate(rtc_scene,g_instance3);
    rtcCommit(rtc_scene);    
}

Scene::~Scene()
{
}
