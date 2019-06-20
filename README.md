# Ubik
Path-tracer using embree / SDL / OpenEXR

# Examples

These examples are using a dummy integrator that opens a TIFF file from disk and "render" it.
Each time a pixel of the image is sampled, a random noise is added to the RGB value. Over time, the noise should be averaged out.

## Interactive Rendering
![video](doc/capture_interactive.gif "Interactive Rendering")

## Tiled rendering
![video](doc/capture_tiled_rendering.gif "Tiled Rendering")

# External Dependencies:
* OpenEXR (2.0+): openexr.com
* SDL (2.0+): https://www.libsdl.org
* TIFF (3.9+): https://www.adobe.io/open/standards/TIFF.html
* Embree (2.17+): https://www.embree.org

# Other Dependencies:
* getMemorySize.cpp by David Robert Nadeau (http://NadeauSoftware.com/) is provided under the Creative Commons Attribution 3.0 Unported License (http://creativecommons.org/licenses/by/3.0/deed.en_US)
* OptionbParser by Johannes Wei�l (http://github.com/weisslj/cpp-optparse.git), added as a git submodule
* ctpl by Vitaliy Vitsentiy (https://github.com/vit-vit/CTPL), added as a git submodule

# References
* Test image "Kodak Image 21" from http://r0k.us/graphics/kodak/kodim21.html