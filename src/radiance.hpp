/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/

#ifndef RADIANCE_HPP
#define RADIANCE_HPP

#include <math/vec3.h>
#include <math/vec3fa.h>

struct Radiance
{
    double r;
    double g;
    double b;
    
    Radiance(double r=0.0, double g=0.0, double b=0.0): r (r), g (g), b (b) {}
    Radiance(const embree::Vec3fa &other) : r(other.x), g(other.y), b(other.z){}

    Radiance & operator = (const Radiance &other)
    {
    	r = other.r;
    	g = other.g;
    	b = other.b;

    	return *this;
    }

    Radiance & operator -= (const Radiance &other)
    {
    	r -= other.r;
    	g -= other.g;
    	b -= other.b;

    	return *this;
    }

    Radiance operator - (const Radiance &other)
    {
        return Radiance(r-other.r, g-other.g, b-other.b);
    }

    Radiance & operator += (const Radiance &other)
    {
    	r += other.r;
    	g += other.g;
    	b += other.b;

    	return *this;
    }

    Radiance operator + (const Radiance &other)
    {
        return Radiance(r+other.r, g+other.g, b+other.b);
    }

    Radiance & operator /= (float scalar)
    {
    	r /= scalar;
    	g /= scalar;
    	b /= scalar;

    	return *this;
    }

    Radiance operator / (float scalar)
    {
        return Radiance(r/scalar, g/scalar, b/scalar);
    }

    Radiance & operator *= (float scalar)
    {
    	r *= scalar;
    	g *= scalar;
    	b *= scalar;

    	return *this;
    }

    Radiance operator * (float scalar)
    {
        return Radiance(r*scalar, g*scalar, b*scalar);
    }
};



#endif // !RADIANCE_HPP
