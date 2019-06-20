if (APPLE)
    set (EMBREE_LIB_NAME libembree.dylib)
elseif (UNIX)
    set (EMBREE_LIB_NAME libembree.so)
elseif (WIN32)
    set (EMBREE_LIB_NAME embree.lib)
endif()

find_library(EMBREE_LIBRARY
        "${EMBREE_LIB_NAME}"
    HINTS
        "${EMBREE_LOCATION}/lib64"
        "${EMBREE_LOCATION}/lib"
        "$ENV{EMBREE_LOCATION}/lib64"
        "$ENV{EMBREE_LOCATION}/lib"
    DOC
        "Embree library path"
)

find_path(EMBREE_INCLUDE_DIR
    embree2/rtcore.h
HINTS
    "${EMBREE_LOCATION}/include"
    "$ENV{EMBREE_LOCATION}/include"
DOC
    "Embree headers path"
)

if (EMBREE_INCLUDE_DIR AND EXISTS "${EMBREE_INCLUDE_DIR}/embree2/rtcore_version.h" )
    file(STRINGS "${EMBREE_INCLUDE_DIR}/embree2/rtcore_version.h" TMP REGEX "^#define RTCORE_VERSION_MAJOR.*$")
    string(REGEX MATCHALL "[0-9]+" MAJOR ${TMP})
    file(STRINGS "${EMBREE_INCLUDE_DIR}/embree2/rtcore_version.h" TMP REGEX "^#define RTCORE_VERSION_MINOR.*$")
    string(REGEX MATCHALL "[0-9]+" MINOR ${TMP})
    file(STRINGS "${EMBREE_INCLUDE_DIR}/embree2/rtcore_version.h" TMP REGEX "^#define RTCORE_VERSION_PATCH.*$")
    string(REGEX MATCHALL "[0-9]+" PATCH ${TMP})

    set (EMBREE_VERSION ${MAJOR}.${MINOR}.${PATCH})
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Embree
    REQUIRED_VARS
        EMBREE_INCLUDE_DIR
        EMBREE_LIBRARY
    VERSION_VAR
        EMBREE_VERSION
)

set(CMAKE_EXE_LINKER_FLAGS "-lpthread")