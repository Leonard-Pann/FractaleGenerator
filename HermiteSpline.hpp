#ifndef __HERMITE_SPLINE_HPP
#define __HERMITE_SPLINE_HPP

#include "Vector.hpp"
#include "LUT.hpp"

class HermiteSpline
{
private:
	LUT lut;
	std::vector<Vector2> points;
	std::vector<Vector2> velocities;
	void generateLUT();
	float length;

public:
	HermiteSpline();
	HermiteSpline(std::vector<Vector2> points);
	Vector2 Evaluate(float t) const;
	Vector2 EvaluateDistance(float x) const;
	float convertDistanceToTime(float x) const;

	const Vector2 getStart() const;
	const Vector2 getEnd() const;
};
#endif