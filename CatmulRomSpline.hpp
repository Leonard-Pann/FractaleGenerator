#ifndef CATMUL_ROM_SPLINE_HPP
#define CATMUL_ROM_SPLINE_HPP

#include <iostream>
#include "Math.hpp"
#include "Vector.hpp"
#include "LUT.hpp"
#include "HermiteSpline.hpp"

template<typename T>
class CatmulRomSpline : public HermiteSpline<T>
{
private:


public:
	CatmulRomSpline() {}
	CatmulRomSpline(const std::vector<T>& points)
	{
		if (points.size() < 2u)
		{
			std::cout << "A Cardinal Spline have at least two points, got only " << points.size() << "point." << std::endl;
		}

		this->points = points;
		this->velocities.reserve(points.size());
		this->velocities.push_back(points[1] - points[0]);

		for (int i = 1; i < points.size() - 1; i++)
		{
			this->velocities.push_back(0.5f * (points[i + 1] - points[i - 1]));
		}

		this->velocities.push_back(points[points.size() - 1] - points[points.size() - 2]);
		this->generateLUT();
	}
	CatmulRomSpline(const CatmulRomSpline& catmulRomSpline)
	{
		this->points = catmulRomSpline.points;
		this->velocities = catmulRomSpline.velocities;
		this->length = catmulRomSpline.length;
		this->lut = catmulRomSpline.lut;
	}
};

#endif