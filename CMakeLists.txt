cmake_minimum_required(VERSION 3.2.2)
project(Ubik)

# print information
message (STATUS "CMake version is ${CMAKE_VERSION}")
message (STATUS "Project source dir = ${PROJECT_SOURCE_DIR}")
message (STATUS "Project build dir   = ${CMAKE_BINARY_DIR}")

# Release/Debug
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Release")
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set (DEBUGMODE ON)
endif ()

# 
list(APPEND CMAKE_MODULE_PATH
     "${PROJECT_SOURCE_DIR}/cmake")

# Set C++ 11
set (CMAKE_CXX_STANDARD 11)

# 
include (externalpackages)

#
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Define executable target
include_directories(
  src/
  src/camera
  src/driver
  src/integrator
  src/primitive
  src/renderer
  src/sampler
  src/scene
  src/utils
  thirdparty/ctpl
  thirdparty/cpp-optparse
  ${CMAKE_BINARY_DIR}
)

add_executable(
  ubik
  src/logging.cpp
  src/main.cpp
  src/memory.cpp
  src/options.cpp
  src/rng.cpp
  src/signal.cpp
  src/task.cpp
  src/vector.cpp
  src/camera/camera.cpp
  src/camera/perspectivecamera.cpp
  src/driver/displaydriver.cpp
  src/driver/exrdriver.cpp
  src/driver/outputdriver.cpp
  src/integrator/imagerender.cpp
  src/integrator/integrator.cpp
  src/integrator/pathtracer.cpp
  src/primitive/primitive.cpp
  src/renderer/bucket.cpp
  src/renderer/interactiverenderer.cpp
  src/renderer/renderer.cpp
  src/renderer/tiledrenderer.cpp
  src/sampler/fixedsampler.cpp
  src/sampler/sampler.cpp
  src/scene/scene.cpp
  src/utils/arrayiterator.cpp
  thirdparty/cpp-optparse/OptionParser.cpp
  thirdparty/nadeau/getMemorySize.cpp
)

target_link_libraries(
  ubik
  ${SDL2_LIBS}
  ${TIFF_LIBRARY}
  ${OPENEXR_LIBRARIES}
  ${EMBREE_LIBRARY}
)
