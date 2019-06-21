/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/

#ifndef COLOR_HPP
#define COLOR_HPP

#include <OpenEXR/half.h>
#include <tiffio.h> // for uint32
#include <math/vec3.h>
#include <math/vec3fa.h>

struct Color
{
    half	r;
    half	g;
    half	b;
    
    // Color(uint32 r, uint32 g, uint32 b) : r(r/255.0), g(g/255.0), b(b/255.0){}
    Color(half r=0.0, half g=0.0, half b=0.0): r (r), g (g), b (b) {}
    Color(const embree::Vec3fa &other) : r(other.x), g(other.y), b(other.z){}

    Color & operator = (const Color &other)
    {
    	r = other.r;
    	g = other.g;
    	b = other.b;

    	return *this;
    }

    Color & operator += (const Color &other)
    {
    	r += other.r;
    	g += other.g;
    	b += other.b;

    	return *this;
    }

    Color & operator /= (float scalar)
    {
    	r /= scalar;
    	g /= scalar;
    	b /= scalar;

    	return *this;
    }

    Color & operator *= (float scalar)
    {
    	r *= scalar;
    	g *= scalar;
    	b *= scalar;

    	return *this;
    }
};



#endif // !COLOR_HPP
