#ifndef HERMITE_SPLINE_2D_HPP
#define HERMITE_SPLINE_2D_HPP

#include <iostream>
#include <vector>
#include "HermiteSpline.hpp"
#include "Vector.hpp"

struct Rect 
{
    float minX, minY, maxX, maxY;
};

class HermiteSpline2D : public HermiteSpline<Vector2> 
{
public:
    using HermiteSpline<Vector2>::HermiteSpline;

    std::vector<Rect> getSegmentBoundingBoxes() const 
    {
        std::vector<Rect> boxes;
        if (points.size() < 2u) 
            return boxes;

        for (size_t i = 0; i < points.size() - 1; ++i) 
        {
            boxes.push_back(computeSegmentBBox(i));
        }
        return boxes;
    }

    Rect getTotalBoundingBox() const 
    {
        auto boxes = getSegmentBoundingBoxes();
        if (boxes.empty()) 
            return { 0.0f, 0.0f, 0.0f, 0.0f };

        Rect total = boxes[0];
        for (const Rect& b : boxes) 
        {
            total.minX = std::min(total.minX, b.minX);
            total.minY = std::min(total.minY, b.minY);
            total.maxX = std::max(total.maxX, b.maxX);
            total.maxY = std::max(total.maxY, b.maxY);
        }
        return total;
    }

private:
    Rect computeSegmentBBox(size_t index) const 
    {
        Vector2 p0 = points[index];
        Vector2 p1 = points[index + 1];
        Vector2 m0 = velocities[index];
        Vector2 m1 = velocities[index + 1];

        float minX = std::min(p0.x, p1.x);
        float maxX = std::max(p0.x, p1.x);
        float minY = std::min(p0.y, p1.y);
        float maxY = std::max(p0.y, p1.y);

        auto solveExtrema = [&](float p0c, float p1c, float m0c, float m1c, float& minC, float& maxC) 
        {
            float a = 6.0f * p0c - 6.0f * p1c + 3.0f * m0c + 3.0f * m1c;
            float b = -6.0f * p0c + 6.0f * p1c - 4.0f * m0c - 2.0f * m1c;
            float c = m0c;

            float discriminant = b * b - 4 * a * c;
            if (discriminant >= 0) 
            {
                float sqrtD = std::sqrt(discriminant);
                float ts[2] = { (-b + sqrtD) / (2.0f * a), (-b - sqrtD) / (2.0f * a) };

                for (float t : ts) 
                {
                    if (t > 0.0f && t < 1.0f) 
                    {
                        float h0 = 2 * t * t * t - 3 * t * t + 1;
                        float h1 = t * t * t - 2 * t * t + t;
                        float h2 = -2 * t * t * t + 3 * t * t;
                        float h3 = t * t * t - t * t;
                        float val = h0 * p0c + h1 * m0c + h2 * p1c + h3 * m1c;

                        minC = std::min(minC, val);
                        maxC = std::max(maxC, val);
                    }
                }
            }
        };

        solveExtrema(p0.x, p1.x, m0.x, m1.x, minX, maxX);
        solveExtrema(p0.y, p1.y, m0.y, m1.y, minY, maxY);

        return { minX, minY, maxX, maxY };
    }
};

#endif

