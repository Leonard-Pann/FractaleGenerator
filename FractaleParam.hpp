#ifndef __FRACTAL_PARAM_HPP
#define __FRACTAL_PARAM_HPP

#include "Vector.hpp"

struct FractaleParam
{
    Vector2 seed;
    float xMin, xMax, yMin, yMax;
    Vector3 fadeColor;
    int maxIter;

    FractaleParam()
    {
        this->seed = Vector2(0.0, 0.0);
        this->xMin = 0.0;
        this->xMax = 0.0;
        this->yMin = 0.0;
        this->yMax = 0.0;
        this->fadeColor = Vector3(0.0, 0.0, 0.0);
        this->maxIter = 0;
    }

    FractaleParam(Vector2 seed, float xMin, float xMax, float yMin, float yMax, Vector3 fadeColor, int maxIter)
    {
        this->seed = seed;
        this->xMin = xMin;
        this->xMax = xMax;
        this->yMin = yMin;
        this->yMax = yMax;
        this->fadeColor = fadeColor;
        this->maxIter = maxIter;
    }
};

#endif