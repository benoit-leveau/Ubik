#!/bin/bash

DIST=`uname`
if [ "$DIST" != "Darwin" ];then
export PATH=/milk/code/dev/bin/centos6/gcc484-nosuffix/:$PATH
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


if [ "$DIST" == "Darwin" ]; then
SDL_HEADER_DIR="-I/Library/Frameworks/SDL2.framework/Headers"
GCC48_LIB_DIR=
TIFF_HEADER_DIR="-I/Users/benoit/Documents/Prog/libs/include/tiff"
OPENEXR_LIB_DIR="-L/Users/benoit/Documents/Prog/libs/lib/ -lHalf -lIlmImf"
OPENEXR_HEADER_DIR="-I/Users/benoit/Documents/Prog/libs/include/OpenEXR -I/Users/benoit/Documents/Prog/libs/include/"
SDL_LIB_DIR="-framework SDL2"
TIFF_LINK="-ltiff"
else
SDL_HEADER_DIR="-I/milk/code/central/include/SDL/2.0/SDL2/"
OPENEXR_HEADER_DIR="-I/milk/code/central/include/OpenEXR/2.0.1/"
GCC48_LIB_DIR="-L/milk/code/central/lib/centos6/gcc-4.8.4/"
OPENEXR_LIB_DIR="-L/milk/code/central/lib/centos6/gcc-4.4.7/"
SDL_LIB_DIR="-L/milk/code/central/lib/centos6/gcc-4.4.7/ -lSDL2"
TIFF_LINK="-ltiff"
fi

OPTIONS="$OPTIONS -Wall -Wextra -pedantic -O3 -std=c++11"
INCLUDE_DIR="-I./ -Idriver -Irenderer -Iintegrator -Isampler -Iutils -I../thirdparty/ctpl -I../thirdparty/optparse ${SDL_HEADER_DIR} ${OPENEXR_HEADER_DIR} ${TIFF_HEADER_DIR}"
LIB_DIR="${GCC48_LIB_DIR} ${OPENEXR_LIB_DIR} ${SDL_LIB_DIR} ${TIFF_LINK}"

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

CMD="g++ $OBJECTS -o ../bin/pt $LIB_DIR"
echo $CMD
$CMD
