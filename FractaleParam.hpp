#ifndef __FRACTAL_PARAM_HPP
#define __FRACTAL_PARAM_HPP

#include<vector>
#include "Vector.hpp"

struct FractaleParam
{
    Vector2 origin; // the c constant in the formula Zn+1 = Zn² + c
    float xMin, xMax, yMin, yMax;
    Vector3 inColor;
    std::vector<Vector3> colorPalette;
    int maxIter;

    FractaleParam()
    {
        this->origin = Vector2(0.0, 0.0);
        this->xMin = 0.0;
        this->xMax = 0.0;
        this->yMin = 0.0;
        this->yMax = 0.0;
        this->inColor = Vector3(0.0, 0.0, 0.0);
        this->colorPalette = std::vector<Vector3>();
        this->maxIter = 0;
    }

    FractaleParam(Vector2 origin, float xMin, float xMax, float yMin, float yMax, Vector3 inColor, std::vector<Vector3> colorPalette, int maxIter)
    {
        this->origin = origin;
        this->xMin = xMin;
        this->xMax = xMax;
        this->yMin = yMin;
        this->yMax = yMax;
        this->inColor = inColor;
        this->colorPalette = colorPalette;
        this->maxIter = maxIter;
    }
};

#endif