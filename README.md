# Ubik
Path-tracer using embree / SDL / OpenEXR

# Examples

These examples are using a dummy integrator that opens a TIFF file from disk and "render" it.
Each time a pixel of the image is sampled, a random noise is added to the RGB value. Over time, the noise should be averaged out.

## Interactive Rendering
![video](doc/capture_interactive.gif "Interactive Rendering")

## Tiled rendering
![video](doc/capture_tiled_rendering.gif "Tiled Rendering")

# References
* OpenEXR: openexr.com
* SDL: https://www.libsdl.org
* TIFF: https://www.adobe.io/open/standards/TIFF.html
* Embree: https://www.embree.org
* Test image "Kodak Image 21" from http://r0k.us/graphics/kodak/kodim21.html
