/*

***********************************************************
* © Milk VFX, inc. All rights reserved. *
***********************************************************

*/

#ifndef TASK_H
#define TASK_H

#include <cstddef>

// forward declaration
class Bucket;
class Scene;


class Task
{
public:
    Bucket *bucket;
    Scene *scene;
    bool completed;
    double time;
    size_t threadid;
    Task(Bucket *bucket, Scene *scene);
};

#endif // !TASK_H
