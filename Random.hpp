#ifndef __RANDOM_HPP
#define __RANDOM_HPP

#include <cmath>
#include <iostream>
#include <math.h>

class Random
{
public:
    static void SetRandomSeed()
    {
        srand(time(0));
    }

    static float Rand()
    {
        return ((float)rand()) / RAND_MAX;
    }

    static float RandExclude()
    {
        return ((float)rand()) / (RAND_MAX + 1);
    }

    static int Rand(int a, int b)
    {
        return (int)(RandExclude() * (abs(b - a) + 1)) + a;
    }

    static float Rand(float a, float b)
    {
        return Rand() * abs(b - a) + a;
    }

    static int RandExclude(int a, int b)
    {
        return Rand(a, b - 1);
    }

    static float RandExclude(float a, float b)
    {
        return RandExclude() * abs(b - a) + a;
    }
};

#endif

