#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <cstdlib>
#include <vector>

static constexpr unsigned int rand_max = 4294967295;
static constexpr double one_o_rand_max = 1.0 / static_cast<double>(rand_max);
static constexpr double one_o_rand_max_p1 = 1.0 / (static_cast<double>(rand_max) + 1.0);

// return a random number between 0 and rand_max=4294967295, aka in [0, rand_max]
static inline uint32_t c_rand()
{
    // 4294967296 = 131072 * (32767 + 1)
    uint32_t r = ((uint32_t)rand() % 32768) << 17; // 131072 = 2^17
    // 131072 = 32768 * 4
    uint32_t r2 = (rand() % 32768) << 2; // 4 = 2^2
    uint32_t r3 = rand() % 32768; // no ajustement because (32767 + 1) % 4 == 0
    return r + r2 + (r3 % 4);
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
        return static_cast<float>(static_cast<double>(c_rand()) * one_o_rand_max);
    }

    static inline float randExclude()
    {
        return static_cast<float>(static_cast<double>(c_rand()) * one_o_rand_max_p1);
    }

    static inline int rand(int a, int b)
    {
        if (a == b)
            return a;
        return (c_rand() % abs(b - a + 1)) + a;
    }

    static inline float rand(float a, float b)
    {
        return (Random::rand() * abs(b - a)) + a;
    }

    static inline int randExclude(int a, int b)
    {
        if (abs(b - a) <= 1)
            return a;
        return (c_rand() % (abs(b - a))) + a;
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
