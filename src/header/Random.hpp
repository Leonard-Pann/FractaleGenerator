#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <cstdlib>
#include <vector>

static inline int c_rand()
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
        return (float)((double)c_rand() / ((double)RAND_MAX + 1.0));
    }

    static inline int rand(int a, int b)
    {
        if (a == b)
            return a;
        return (c_rand() % abs(b - a)) + a;
    }

    static inline float rand(float a, float b)
    {
        return (Random::rand() * abs(b - a)) + a;
    }

    static inline int randExclude(int a, int b)
    {
        if (a == b || abs(b - a) == 1)
            return a;
        return (c_rand() % (abs(b - a) - 1)) + a;
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
