/*

***********************************************************
* © Milk VFX, inc. All rights reserved. *
***********************************************************

*/


#include "task.hpp"


Task::Task(Bucket *bucket) : 
    bucket(bucket), 
    completed(false),
    time(0.0),
    threadid(-1)
{}
