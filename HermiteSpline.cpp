#include "HermiteSpline.hpp"
#include "BezierUtils.hpp"

HermiteSpline::HermiteSpline()
{
	points = std::vector<Vector2>();
	velocities = std::vector<Vector2>();
	length = 0.0f;
}

HermiteSpline::HermiteSpline(std::vector<Vector2> points)
{
	this->points = points;

	velocities = std::vector<Vector2>(points.size());

	for (int i = 1; i < velocities.size() - 1; i++)
	{
		velocities[i] = Vector2(points[i + 1].x - points[i - 1].x, points[i + 1].y - points[i - 1].y);
	}

	velocities[0] = Vector2(2.0f * (points[1].x - points[0].x), 2.0f * (points[1].y - points[0].y));
	velocities[points.size() - 1] = Vector2(2.0f * (points[points.size() - 1].x - points[points.size() - 2].x), 2.0f * (points[points.size() - 1].y - points[points.size() - 2].y));

	generateLUT();
}

Vector2 HermiteSpline::Evaluate(float t) const
{
	t = clamp01(t);

	float interLength = 1.0f / (float)(points.size() - 1);
	int i = t < 1.0f ? (int)floor(t / interLength) : points.size() - 2;
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
	return Lerp(lut.t[index], lut.t[index + 1], (x - lut.x[index]) / (lut.x[index + 1] - lut.x[index]));
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
	std::vector<float> x = std::vector<float>(nbPoints);
	std::vector<float> t = std::vector<float>(nbPoints);
	t[0] = 0.0f;
	x[0] = 0.0f;

	Vector2 oldPoint = points[0];
	Vector2 currentPoint;
	float oneOnbPoints = 1.0f / ((float)nbPoints - 1.0f);
	for (int i = 1; i < nbPoints; i++)
	{
		t[i] = i * oneOnbPoints;
		currentPoint = Evaluate(t[i]);
		x[i] = x[i - 1] + distance(currentPoint, oldPoint);
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