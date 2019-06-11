#!/bin/bash

export EMBREE_VER=2.17
export COMPILER=gcc-4.9.2

export LD_LIBRARY_PATH=/milk/code/dev/tools/centos7/gcc-4.9.2/lib64/:/milk/users/benoit/work/thirdparty/sdl2-2.0.9-gcc492-c7/lib/:$LD_LIBRARY_PATH
source /milk/code/config/bash/expand-variables.sh 
source /milk/users/benoit/pwork/ubik/thirdparty/embree-${EMBREE_VER}.0.x86_64.linux/embree-vars.sh

`dirname $0`/pt "$@"
