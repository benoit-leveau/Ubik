#!/bin/bash

export LD_LIBRARY_PATH=/milk/code/dev/tools/centos6/gcc-4.8.4/lib64/:/milk/code/central/lib/centos6/gcc-4.8.4:$LD_LIBRARY_PATH
export EMBREE_VER=2.17
source /milk/code/config/bash/expand-variables.sh 
source /milk/users/benoit/pwork/ubik/thirdparty/embree-${EMBREE_VER}.0.x86_64.linux/embree-vars.sh

`dirname $0`/pt "$@"
