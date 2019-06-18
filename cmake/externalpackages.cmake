###########################################################################
# Find libraries

###########################################################################
# Find SDL2
find_package(SDL2 REQUIRED)
include_directories(
  ${SDL2_INCLUDE_DIRS}
)

###########################################################################
# TIFF
if (NOT TIFF_LIBRARIES OR NOT TIFF_INCLUDE_DIR)
    find_package (TIFF 3.9 REQUIRED)
    include_directories (${TIFF_INCLUDE_DIR})
else ()
    message (STATUS "Custom TIFF_LIBRARIES ${TIFF_LIBRARIES}")
    message (STATUS "Custom TIFF_INCLUDE_DIR ${TIFF_INCLUDE_DIR}")
endif ()

###########################################################################
# OpenEXR setup

find_package (OpenEXR 2.0 REQUIRED)
include_directories ("${OPENEXR_INCLUDE_DIR}")
message (STATUS "OPENEXR_INCLUDE_DIR = ${OPENEXR_INCLUDE_DIR}")
message (STATUS "OPENEXR_LIBRARIES = ${OPENEXR_LIBRARIES}")

###########################################################################
# Embree
find_package (Embree 2.17 REQUIRED)

# add all the include dirs
include_directories(
  ${EMBREE_INCLUDE_DIR}
)
