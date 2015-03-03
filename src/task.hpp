/*

***********************************************************
* © Benoit Leveau - All rights reserved. *
***********************************************************

*/

#ifndef TASK_H
#define TASK_H

#include <cstddef>
#include <random>

// forward declaration
class Bucket;
class Scene;
class RNG;

class Task
{
public:
    Task(Bucket *bucket);

    Bucket *bucket;

    bool completed;
    double time;
    size_t threadid;
};

#endif // !TASK_H
