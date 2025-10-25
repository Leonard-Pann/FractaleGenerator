#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <stdlib.h>
#include <vector>

static int c_rand()
{
    return rand();
}

class Random
{
public:

    static inline void setSeed(uint32_t seed)
    {
        srand(seed);
    }

    static inline void setRandomSeed()
    {
        srand(time(0));
    }

    static inline float rand()
    {
        return (float)((double)c_rand() / (double)RAND_MAX);
    }

    static inline float randExclude()
    {
        int rand = c_rand();
        if (rand == RAND_MAX)
            return 1.0f - FLT_MIN;
        return (float)((double)(rand - 1) / (double)RAND_MAX);
    }

    static inline int rand(int a, int b)
    {
        return (int)(Random::randExclude() * (abs(b - a) + 1)) + a;
    }

    static inline  float rand(float a, float b)
    {
        return (Random::rand() * abs(b - a)) + a;
    }

    static inline int randExclude(int a, int b)
    {
        return Random::rand(a, b - 1);
    }

    static inline float randExclude(float a, float b)
    {
        return Random::randExclude() * abs(b - a) + a;
    }

    template<class T>
    static void shuffle(std::vector<T>& array)
    {
        int start = (int)array.size() - 1;
        int randIndex;
        for (int i = start; i >= 0; i--)
        {
            randIndex = Random::rand(0, i);
            T tmp = array[i];
            array[i] = array[randIndex];
            array[randIndex] = tmp;
        }
    }
};

#endif

