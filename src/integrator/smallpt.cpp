/*

***********************************************************
* � Benoit Leveau - All rights reserved. *
***********************************************************

*/

#include "smallpt.hpp"
#include "options.hpp"
#include "radiance.hpp"
#include "rng.hpp"
#include "scene.hpp"
#include "camera.hpp"
#include "perspectivecamera.hpp"

#include <math.h>   // smallpt, a Path Tracer by Kevin Beason, 2008
#include <stdlib.h> // Make : g++ -O3 -fopenmp smallpt.cpp -o smallpt
#include <stdio.h>  //        Remove "-fopenmp" for g++ version < 4.2

struct Vec {        // Usage: time ./smallpt 5000 && xv image.ppm
  double x, y, z;                  // position, also color (r,g,b)
  Vec(double x_=0, double y_=0, double z_=0){ x=x_; y=y_; z=z_; }
  Vec operator+(const Vec &b) const { return Vec(x+b.x,y+b.y,z+b.z); }
  Vec operator-(const Vec &b) const { return Vec(x-b.x,y-b.y,z-b.z); }
  Vec operator*(double b) const { return Vec(x*b,y*b,z*b); }
  Vec mult(const Vec &b) const { return Vec(x*b.x,y*b.y,z*b.z); }
  Vec& norm(){ return *this = *this * (1/sqrt(x*x+y*y+z*z)); }
  double dot(const Vec &b) const { return x*b.x+y*b.y+z*b.z; } // cross:
  Vec operator%(Vec&b){return Vec(y*b.z-z*b.y,z*b.x-x*b.z,x*b.y-y*b.x);}
};

struct Ray {
    Vec o, d; Ray(Vec o_, Vec d_) : o(o_), d(d_) {}
};

enum Refl_t {
    DIFF, SPEC, REFR
};  // material types, used in radiance()

struct Sphere {
  double rad;       // radius
  Vec p, e, c;      // position, emission, color
  Refl_t refl;      // reflection type (DIFFuse, SPECular, REFRactive)
  Sphere(double rad_, Vec p_, Vec e_, Vec c_, Refl_t refl_):
    rad(rad_), p(p_), e(e_), c(c_), refl(refl_) {}
  double intersect(const Ray &r) const { // returns distance, 0 if nohit
    Vec op = p-r.o; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
    double t, eps=1e-4, b=op.dot(r.d), det=b*b-op.dot(op)+rad*rad;
    if (det<0) return 0; else det=sqrt(det);
    return (t=b-det)>eps ? t : ((t=b+det)>eps ? t : 0);
  }
};

Sphere spheres[] = {//Scene: radius, position, emission, color, material
  Sphere(1e5, Vec( 1e5+1,40.8,81.6), Vec(),Vec(.75,.25,.25),DIFF),//Left
  Sphere(1e5, Vec(-1e5+99,40.8,81.6),Vec(),Vec(.25,.25,.75),DIFF),//Rght
  Sphere(1e5, Vec(50,40.8, 1e5),     Vec(),Vec(.75,.75,.75),DIFF),//Back
  Sphere(1e5, Vec(50,40.8,-1e5+170), Vec(),Vec(),           DIFF),//Frnt
  Sphere(1e5, Vec(50, 1e5, 81.6),    Vec(),Vec(.75,.75,.75),DIFF),//Botm
  Sphere(1e5, Vec(50,-1e5+81.6,81.6),Vec(),Vec(.75,.75,.75),DIFF),//Top
  Sphere(16.5,Vec(27,16.5,47),       Vec(),Vec(1,1,1)*.999, SPEC),//Mirr
  Sphere(16.5,Vec(73,16.5,78),       Vec(),Vec(1,1,1)*.999, REFR),//Glas
  Sphere(600, Vec(50,681.6-.27,81.6),Vec(12,12,12),  Vec(), DIFF) //Lite
};

inline bool intersect(const Ray &r, double &t, int &id){
  double n=sizeof(spheres)/sizeof(Sphere), d, inf=t=1e20;
  for(int i=int(n);i--;) if((d=spheres[i].intersect(r))&&d<t){t=d;id=i;}
  return t<inf;
}

Vec radiance(const Ray &r, int depth, RNG &rng){
  double t;                               // distance to intersection
  int id=0;                               // id of intersected object
  if (!intersect(r, t, id)) return Vec(); // if miss, return black
  const Sphere &obj = spheres[id];        // the hit object
  Vec x=r.o+r.d*t, n=(x-obj.p).norm(), nl=n.dot(r.d)<0?n:n*-1, f=obj.c;
  double p = f.x>f.y && f.x>f.z ? f.x : f.y>f.z ? f.y : f.z; // max refl
if (++depth>5) if (rng()<p) f=f*(1/p); else return obj.e; //R.R.
  if (obj.refl == DIFF){                  // Ideal DIFFUSE reflection
    double r1=2*M_PI*rng(), r2=rng(), r2s=sqrt(r2);
    Vec w=nl, u=((fabs(w.x)>.1?Vec(0,1):Vec(1))%w).norm(), v=w%u;
    Vec d = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2)).norm();
    return obj.e + f.mult(radiance(Ray(x,d),depth,rng));
  } else if (obj.refl == SPEC)            // Ideal SPECULAR reflection
    return obj.e + f.mult(radiance(Ray(x,r.d-n*2*n.dot(r.d)),depth,rng));
  Ray reflRay(x, r.d-n*2*n.dot(r.d));     // Ideal dielectric REFRACTION
  bool into = n.dot(nl)>0;                // Ray from outside going in?
  double nc=1, nt=1.5, nnt=into?nc/nt:nt/nc, ddn=r.d.dot(nl), cos2t;
  if ((cos2t=1-nnt*nnt*(1-ddn*ddn))<0)    // Total internal reflection
    return obj.e + f.mult(radiance(reflRay,depth,rng));
  Vec tdir = (r.d*nnt - n*((into?1:-1)*(ddn*nnt+sqrt(cos2t)))).norm();
  double a=nt-nc, b=nt+nc, R0=a*a/(b*b), c = 1-(into?-ddn:tdir.dot(n));
  double Re=R0+(1-R0)*c*c*c*c*c,Tr=1-Re,P=.25+.5*Re,RP=Re/P,TP=Tr/(1-P);
  return obj.e + f.mult(depth>2 ? (rng()<P ?   // Russian roulette
    radiance(reflRay,depth,rng)*RP:radiance(Ray(x,tdir),depth,rng)*TP) :
    radiance(reflRay,depth,rng)*Re+radiance(Ray(x,tdir),depth,rng)*Tr);
}

SmallPt::SmallPt(const Options &options, std::shared_ptr<Scene> scene, Logger &logger) : Integrator(options, scene, logger), width64(width), height64(height), samples(1<<options.max_samples)
{}

SmallPt::~SmallPt()
{}

Radiance SmallPt::render(size_t x, size_t y, size_t sample)
{
    // TODO: add RNG to the render function (initialized by the sampler)
    thread_local RNG rng;
    rng.seed(y*y*y + x*x + scene->frame + sample);

    size_t samps = 1;
    
    // Ray cam(Vec(50,52,295.6), Vec(0,-0.042612,-1).norm()); // cam pos, dir
    // get camera from scene
    PerspectiveCamera *pcam = dynamic_cast<PerspectiveCamera *>(scene->camera.get());
    auto dir = normalize(pcam->to - pcam->from);
    Ray cam(Vec(pcam->from.x,pcam->from.y,pcam->from.z),
            Vec(dir.x, dir.y, dir.z));
    
    Vec cx=Vec(width64*.5135/height64);
    Vec cy=(cx%cam.d).norm()*.5135;
    
    //for (int sy=0; sy<2; sy++)     // 2x2 subpixel rows
    //  for (int sx=0; sx<2; sx++, r=Vec()){// 2x2 subpixel cols
    //for (int s=0; s<samps; s++){
    double r1=2*rng(), dx=r1<1 ? sqrt(r1)-1: 1-sqrt(2-r1);
    double r2=2*rng(), dy=r2<1 ? sqrt(r2)-1: 1-sqrt(2-r2);
    Vec d = cx*( ( (1.0 + dx)/2 + x)/width64 - .5) +
            cy*( ( (1.0 + dy)/2 + (height64-y-1))/height64 - .5) + cam.d;
    Vec r = radiance(Ray(cam.o+d*140,d.norm()),0,rng);
    return Radiance(r.x, r.y, r.z);

    /*
    Vec r;
    Vec c;
    for (int sy=0; sy<2; sy++)     // 2x2 subpixel rows
        for (int sx=0; sx<2; sx++, r=Vec()){// 2x2 subpixel cols
            for (int s=0; s<samps; s++){
                double r1=2*rng(), dx=r1<1 ? sqrt(r1)-1: 1-sqrt(2-r1);
                double r2=2*rng(), dy=r2<1 ? sqrt(r2)-1: 1-sqrt(2-r2);
                Vec d = cx*( ( (sx+.5 + dx)/2 + x)/width64 - .5) +
                        cy*( ( (sy+.5 + dy)/2 + (height64-y-1))/height64 - .5) + cam.d;
                r = r + radiance(Ray(cam.o+d*140,d.norm()),0,rng)*(1./samps);
            }
            // Camera rays are pushed ^^^^^ forward to start in interior
            c = c + Vec(/*clamp*-/(r.x), /*clamp*-/(r.y), /*clamp*-/(r.z))*.25;
        }
    
    return Radiance(c.x, c.y, c.z);
    */
}
