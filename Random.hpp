#ifndef __RANDOM_HPP
#define __RANDOM_HPP

#include <iostream>

static int c_rand()
{
    return rand();
}

class Random
{
public:
    static inline void set_random_seed()
    {
        srand(time(0));
    }

    static inline float rand()
    {
        return ((float)c_rand()) / RAND_MAX;
    }

    static inline float randExclude()
    {
        return ((float)c_rand()) / (RAND_MAX + 1);
    }

    static inline int rand(int a, int b)
    {
        return (int)(randExclude() * (abs(b - a) + 1)) + a;
    }

    static inline  float rand(float a, float b)
    {
        return c_rand() * abs(b - a) + a;
    }

    static inline int randExclude(int a, int b)
    {
        return rand(a, b - 1);
    }

    static inline float randExclude(float a, float b)
    {
        return randExclude() * abs(b - a) + a;
    }
};

#endif

