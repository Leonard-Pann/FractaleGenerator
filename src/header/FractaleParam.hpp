#ifndef FRACTAL_PARAM_HPP
#define FRACTAL_PARAM_HPP

#include<vector>
#include "Vector.hpp"

struct FractaleParam
{
    Vector2 origin; // the c constant in the formula Zn+1 = Zn² + c
    float xMin, xMax, yMin, yMax;
    float colorRange;
    Vector3 inColor;
    std::vector<Vector3> colorPalette;

    FractaleParam() : origin(), xMin(0), xMax(0), yMin(0), yMax(0), inColor(), colorPalette(), colorRange(0.0)
    {

    }

    FractaleParam(Vector2 origin, float xMin, float xMax, float yMin, float yMax, Vector3 inColor, const std::vector<Vector3>& colorPalette, float colorRange) :
        origin(origin), xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax), inColor(inColor), colorPalette(colorPalette), maxIter(maxIter), colorRange(colorRange)
    {

    }
};

#endif