#ifndef HERMITE_SPLINE_HPP
#define HERMITE_SPLINE_HPP

#include <cfloat>
#include "Vector.hpp"
#include "LUT.hpp"
#include "Math.hpp"

template<typename T>
class HermiteSpline
{
protected:
	LUT lut;
	std::vector<T> points;
	std::vector<T> velocities;
	float length;

	void generateLUT()
	{
		int nbPoints = 200;
		std::vector<float> x = std::vector<float>(nbPoints);
		std::vector<float> t = std::vector<float>(nbPoints);
		t[0] = 0.0f;
		x[0] = 0.0f;

		T oldPoint = points[0];
		T currentPoint;
		float oneOnbPoints = 1.0f / ((float)nbPoints - 1.0f);
		for (int i = 1; i < nbPoints; i++)
		{
			t[i] = i * oneOnbPoints;
			currentPoint = evaluate(t[i]);
			x[i] = x[i - 1] + T::distance(currentPoint, oldPoint);
			oldPoint = currentPoint;
		}

		length = x[nbPoints - 1];
		float oneOlength = 1.0 / length;
		for (int i = 0; i < nbPoints; i++)
		{
			x[i] *= oneOlength;
		}

		x[nbPoints - 1] = 1.0f;
		t[nbPoints - 1] = 1.0f;

		lut = LUT(x, t, nbPoints);
	}

	static int findRecur(float x, int start, int end, std::vector<float> arr)
	{
		if (end - start <= 1)
			return arr[end] <= x ? end : start;

		int mid = (start + end) / 2;
		if (x < arr[mid])
		{
			return findRecur(x, start, mid, arr);
		}
		return findRecur(x, mid, end, arr);
	}

public:
	HermiteSpline() : points(), velocities(), length(0.0f) {};
	HermiteSpline(const std::vector<T>& points) : points(points), length(0.0f)
	{
		if (points.size() < 2)
		{
			std::cout << "A Hermite spline must have at least 4 points, got only " << points.size() << std::endl;
			return;
		}

		velocities = std::vector<T>(points.size());

		for (int i = 1; i < velocities.size() - 1; i++)
		{
			velocities[i] = points[i + 1] - points[i - 1];
			//velocities[i] = Vector2(points[i + 1].x - points[i - 1].x, points[i + 1].y - points[i - 1].y);
		}

		velocities[0] = 2.0f * (points[1] - points[0]);
		velocities[points.size() - 1] = 2.0f * (points[points.size() - 1] - points[points.size() - 2]);
		generateLUT();
	}
	HermiteSpline(const HermiteSpline& hermiteSpline) : points(hermiteSpline.points), velocities(hermiteSpline.velocities), lut(hermiteSpline.lut), length(hermiteSpline.length) {}

	const std::vector<T>& getPoints() { return this->points; }

	T evaluate(float t) const
	{
		t = Math::clamp01(t);

		float interLength = 1.0f / (float)(points.size() - 1);
		int i = t < 1.0f ? Math::floor(t / interLength) : points.size() - 2;
		float newT = (t - (i * interLength)) / interLength;

		float cache = newT * newT;
		float c0 = cache * newT;
		return points[i] + (newT * velocities[i]) + cache * (3.0f * (points[i + 1] - points[i]) - (2.0f * velocities[i]) - velocities[i + 1]) + (c0 * (2.0f * (points[i] - points[i + 1]) + velocities[i] + velocities[i + 1]));
	}

	T evaluateDistance(float x) const
	{
		return evaluate(convertDistanceToTime(x));
	}

	float convertDistanceToTime(float x) const
	{
		if (x <= 0.0f)
			return 0.0f;
		if (x >= 1.0f)
			return 1.0f;

		int index = findRecur(x, 0, lut.length - 1, lut.x);
		return Math::lerp(lut.t[index], lut.t[index + 1], (x - lut.x[index]) / (lut.x[index + 1] - lut.x[index]));
	}

	const T getStart() const { return points[0]; }
	const T getEnd() const { return points[points.size() - 1]; }
};

#endif