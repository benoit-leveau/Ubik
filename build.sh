#!/bin/bash

export PATH=/milk/code/dev/bin/centos6/gcc484-nosuffix/:$PATH
source /milk/code/config/bash/expand-variables.sh

cd `dirname $0`/src
rm -rf ../build
mkdir -p ../build
mkdir -p ../bin

if [ "$1" == "debug" ]; then
OPTIONS="-g -DNDEBUG"
else
OPTIONS="-O3"
fi

OPTIONS="$OPTIONS -Wall -Wextra -pedantic -O3 -std=c++11"
INCLUDE_DIR="-I./ -Idriver -Irenderer -Iintegrator -Isampler -I../thirdparty/ctpl -I../thirdparty/optparse -I/milk/code/central/include/SDL/2.0/SDL2/ -I/milk/code/central/include/OpenEXR/2.0.1/"
LIB_DIR="-L/milk/code/central/lib/centos6/gcc-4.8.4/ -L/milk/code/central/lib/centos6/gcc-4.4.7/"
LINK_LIB="-lSDL2 -lIlmImf -ltiff"

OBJECTS=""
for FILE in `find ./ -iname "*.cpp" | sed 's/\.\///g'`; do
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

CMD="g++ $OBJECTS -o ../bin/pt $LIB_DIR $LINK_LIB"
echo $CMD
$CMD
