/*

***********************************************************
* © Milk VFX, inc. All rights reserved. *
***********************************************************

*/


#include "task.hpp"


Task::Task(Bucket *bucket, Scene *scene) : 
    bucket(bucket), 
    scene(scene),
    completed(false),
    time(0.0),
    threadid(-1)
{}
