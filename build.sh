#!/bin/bash

DIST=`uname`
if [ "$DIST" != "Darwin" ];then
export PATH=/milk/code/dev/tools/centos7/gcc-4.9.2/bin/:$PATH
source /milk/code/config/bash/expand-variables.sh
fi

cd `dirname $0`/src
rm -rf ../build
mkdir -p ../build
mkdir -p ../bin

if [ "$1" == "debug" ]; then
OPTIONS="-g -DNDEBUG"
else
OPTIONS="-O3"
fi

EMBREE_VERSION=2.17

if [ "$DIST" == "Darwin" ]; then
OPTIONS="$OPTIONS -DOSX=1"
SDL_HEADER_DIR="-I/Library/Frameworks/SDL2.framework/Headers"
GCC_LIB_DIR=
TIFF_HEADER_DIR="-I/Users/benoit/Documents/Prog/libs/include/tiff"
OPENEXR_LIB_DIR="-L/Users/benoit/Documents/Prog/libs/lib/"
OPENEXR_LINK="-lHalf -lIlmImf"
OPENEXR_HEADER_DIR="-I/Users/benoit/Documents/Prog/libs/include/OpenEXR -I/Users/benoit/Documents/Prog/libs/include/"
SDL_LIB_DIR=""
SDL_LINK="-framework SDL2"
TIFF_LINK="-ltiff"
else
OPTIONS="$OPTIONS -DLINUX=1"
SDL_HEADER_DIR="-I/milk/users/benoit/work/thirdparty/sdl2-2.0.9-gcc492-c7/include/SDL2/"
OPENEXR_HEADER_DIR="-I/milk/code/central/include/centos7/gcc-4.9.2/"
EMBREE_HEADER_DIR="-I../thirdparty/embree-${EMBREE_VERSION}.0.x86_64.linux/include/"
TIFF_HEADER_DIR="-I/milk/code/central/include/tiff/4.0.4/"
GCC_LIB_DIR="-L/milk/code/central/lib/centos7/gcc-4.9.2/"
OPENEXR_LIB_DIR="-L/milk/code/central/lib/centos7/gcc-4.9.2/"
OPENEXR_LINK="-lHalf -lIlmImf"
SDL_LIB_DIR="-L/milk/users/benoit/work/thirdparty/sdl2-2.0.9-gcc492-c7/lib/"
SDL_LINK="-lSDL2"
TIFF_LINK="-ltiff"
EMBREE_LIB_DIR="-L/milk/code/central/lib/centos7/gcc-4.9.2/"
EMBREE_LINK="-Wl,-rpath-link,/milk/code/central/lib/centos7/gcc-4.9.2/ -lembree"
fi

OPTIONS="$OPTIONS -O3 -std=c++11" # -Wall -Wextra -pedantic
INCLUDE_DIR="-I./ -Idriver -Irenderer -Iintegrator -Isampler -Iutils -Iscene -Icamera -I../thirdparty/ctpl -I../thirdparty/optparse ${SDL_HEADER_DIR} ${OPENEXR_HEADER_DIR} ${TIFF_HEADER_DIR} ${EMBREE_HEADER_DIR}"
LIB_DIR="${GCC_LIB_DIR} ${OPENEXR_LIB_DIR} ${OPENEXR_LINK} ${SDL_LIB_DIR} ${SDL_LINK} ${TIFF_LINK} ${EMBREE_LIB_DIR} ${EMBREE_LINK}"

OBJECTS=""
for FILE in `find . -iname "*.cpp" | sed 's/\.\///g'`; do
OUTPUT_FILE=`echo $FILE | cut -d "/" -f 2 | cut -d "." -f 1`
CMD="g++ $FILE -c -o ../build/$OUTPUT_FILE.o $OPTIONS $INCLUDE_DIR"
echo $CMD
$CMD
if [ "$?" != 0 ]; then
echo "Aborting!"
exit 1
fi
OBJECTS="$OBJECTS ../build/$OUTPUT_FILE.o"
done

CMD="g++ ../thirdparty/optparse/OptionParser.cpp -c -o ../build/OptionParser.o $OPTIONS $INCLUDE_DIR"
echo $CMD
$CMD
OBJECTS="$OBJECTS ../build/OptionParser.o"

CMD="g++ ../thirdparty/nadeau/getMemorySize.cpp -c -o ../build/getMemorySize.o $OPTIONS $INCLUDE_DIR"
echo $CMD
$CMD
OBJECTS="$OBJECTS ../build/getMemorySize.o"

CMD="g++ $OBJECTS -o ../bin/pt $LIB_DIR"
echo $CMD
$CMD
