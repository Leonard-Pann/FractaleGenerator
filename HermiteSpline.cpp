#include "HermiteSpline.hpp"
#include "Math.hpp"
#include <iostream>

using namespace std;

int findRecur(float x, int start, int end, vector<float> arr)
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

HermiteSpline::HermiteSpline() : points(), velocities(), length(0.0f)
{

}

HermiteSpline::HermiteSpline(const vector<Vector2>& points) : points(points), length(0.0f)
{
	if (points.size() < 2)
	{
		cout << "A Hermite spline must have at least 4 points, got only " << points.size() << endl;
		return;
	}

	velocities = vector<Vector2>(points.size());

	for (int i = 1; i < velocities.size() - 1; i++)
	{
		velocities[i] = Vector2(points[i + 1].x - points[i - 1].x, points[i + 1].y - points[i - 1].y);
	}

	velocities[0] = Vector2(2.0f * (points[1].x - points[0].x), 2.0f * (points[1].y - points[0].y));
	velocities[points.size() - 1] = Vector2(2.0f * (points[points.size() - 1].x - points[points.size() - 2].x), 2.0f * (points[points.size() - 1].y - points[points.size() - 2].y));

	generateLUT();
}

HermiteSpline::HermiteSpline(const HermiteSpline& hermiteSpline) : points(hermiteSpline.points), velocities(hermiteSpline.velocities), lut(hermiteSpline.lut), length(hermiteSpline.length)
{

}

Vector2 HermiteSpline::Evaluate(float t) const
{
	t = Math::clamp01(t);

	float interLength = 1.0f / (float)(points.size() - 1);
	int i = t < 1.0f ? Math::floor(t / interLength) : points.size() - 2;
	float newT = (t - (i * interLength)) / interLength;

	float cache = newT * newT;
	float c0 = cache * newT;
	return Vector2(points[i].x + (newT * velocities[i].x) + cache * (3.0f * (points[i + 1].x - points[i].x) - (2.0f * velocities[i].x) - velocities[i + 1].x) + (c0 * (2.0f * (points[i].x - points[i + 1].x) + velocities[i].x + velocities[i + 1].x)),
		points[i].y + (newT * velocities[i].y) + cache * (3.0f * (points[i + 1].y - points[i].y) - (2.0f * velocities[i].y) - velocities[i + 1].y) + (c0 * (2.0f * (points[i].y - points[i + 1].y) + velocities[i].y + velocities[i + 1].y)));
}

Vector2 HermiteSpline::EvaluateDistance(float x) const
{
	return Evaluate(convertDistanceToTime(x));
}

float HermiteSpline::convertDistanceToTime(float x) const
{
	if (x <= 0.0f)
		return 0.0f;
	if (x >= 1.0f)
		return 1.0f;

	int index = findRecur(x, 0, lut.length - 1, lut.x);
	return Math::lerp(lut.t[index], lut.t[index + 1], (x - lut.x[index]) / (lut.x[index + 1] - lut.x[index]));
}

const Vector2 HermiteSpline::getStart() const
{
	return points[0];
}

const Vector2 HermiteSpline::getEnd() const
{
	return points[points.size() - 1];
}

void HermiteSpline::generateLUT()
{
	int nbPoints = 200;
	vector<float> x = vector<float>(nbPoints);
	vector<float> t = vector<float>(nbPoints);
	t[0] = 0.0f;
	x[0] = 0.0f;

	Vector2 oldPoint = points[0];
	Vector2 currentPoint;
	float oneOnbPoints = 1.0f / ((float)nbPoints - 1.0f);
	for (int i = 1; i < nbPoints; i++)
	{
		t[i] = i * oneOnbPoints;
		currentPoint = Evaluate(t[i]);
		x[i] = x[i - 1] + Vector2::distance(currentPoint, oldPoint);
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