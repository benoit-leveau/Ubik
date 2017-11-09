#!/bin/bash

export LD_LIBRARY_PATH=/milk/code/dev/tools/centos6/gcc-4.8.4/lib64/:/milk/code/central/lib/centos6/gcc-4.8.4:$LD_LIBRARY_PATH
source /milk/code/config/bash/expand-variables.sh 
source /milk/users/benoit/pwork/ubik/thirdparty/embree-2.11.0.x86_64.linux/embree-vars.sh

`dirname $0`/pt "$@"

