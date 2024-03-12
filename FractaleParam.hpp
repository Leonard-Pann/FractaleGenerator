#ifndef __FRACTAL_PARAM_HPP
#define __FRACTAL_PARAM_HPP

#include "Vector.hpp"

struct FractaleParam
{
    Vector2 seed;
    float xMin, xMax, yMin, yMax;
    Vector3 inColor, outColor;
    int maxIter;

    FractaleParam()
    {
        this->seed = Vector2(0.0, 0.0);
        this->xMin = 0.0;
        this->xMax = 0.0;
        this->yMin = 0.0;
        this->yMax = 0.0;
        this->inColor = Vector3(0.0, 0.0, 0.0);
        this->outColor = Vector3(0.0, 0.0, 0.0);
        this->maxIter = 0;
    }

    FractaleParam(Vector2 seed, float xMin, float xMax, float yMin, float yMax, Vector3 inColor, Vector3 outColor, int maxIter)
    {
        this->seed = seed;
        this->xMin = xMin;
        this->xMax = xMax;
        this->yMin = yMin;
        this->yMax = yMax;
        this->inColor = inColor;
        this->outColor = outColor;
        this->maxIter = maxIter;
    }
};

#endif