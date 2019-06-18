#------------------------------------------------------------------------------
# Usage: find_package(SDL2 [REQUIRED])
#
# Sets variables:
#     SDL2_INCLUDE_DIRS
#     SDL2_LIBS
#     SDL2_DLLS
#------------------------------------------------------------------------------

include(FindPackageHandleStandardArgs)

if(WIN32)
    # Search for SDL2 Debug CMake build in extern/SDL2-2.0.5-dev/build
    find_path(SDL2_ROOT "include/SDL.h" PATHS "${CMAKE_CURRENT_LIST_DIR}/../extern/SDL2-2.0.5-dev" NO_DEFAULT_PATH)
    if(SDL2_ROOT)
        if (EXISTS "${SDL2_ROOT}/build/Debug/SDL2.lib")
            set(SDL2_INCLUDE_DIRS "${SDL2_ROOT}/include")
            set(SDL2_LIBS "${SDL2_ROOT}/build/Debug/SDL2.lib")
            set(SDL2_DLLS "${SDL2_ROOT}/build/Debug/SDL2.dll")
        endif()
    endif()
    if(NOT SDL2_FOUND)
        # Search for SDL2 in extern/SDL2-2.0.5
        find_path(SDL2_ROOT "include/SDL.h" PATHS "${CMAKE_CURRENT_LIST_DIR}/../extern/SDL2-2.0.5" NO_DEFAULT_PATH)
        if(SDL2_ROOT)
            set(SDL2_INCLUDE_DIRS "${SDL2_ROOT}/include")
            if("${CMAKE_GENERATOR}" MATCHES "Win64")
                set(SDL2_LIBS "${SDL2_ROOT}/lib/x64/SDL2.lib")
                set(SDL2_DLLS "${SDL2_ROOT}/lib/x64/SDL2.dll")
            else()
                set(SDL2_LIBS "${SDL2_ROOT}/lib/x86/SDL2.lib")
                set(SDL2_DLLS "${SDL2_ROOT}/lib/x86/SDL2.dll")
            endif()
        endif()
    endif()

    mark_as_advanced(SDL2_ROOT)
    find_package_handle_standard_args(SDL2 DEFAULT_MSG SDL2_INCLUDE_DIRS SDL2_LIBS SDL2_DLLS)
else()
    # On MacOS, should be installed via Macports
    # On Ubuntu, install with: apt-get install libsdl2-dev
    find_path(SDL2_INCLUDE_DIRS SDL.h PATH_SUFFIXES SDL2)
    find_library(_SDL2_LIB SDL2)
    set(SDL2_LIBS ${SDL2})
    mark_as_advanced(SDL2_INCLUDE_DIRS _SDL2_LIB)
    find_package_handle_standard_args(SDL2 DEFAULT_MSG SDL2_INCLUDE_DIRS SDL2_LIBS)
endif()
