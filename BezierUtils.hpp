#ifndef __BEZIER_UTILS_HPP
#define __BEZIER_UTILS_HPP

#include <vector>
#include "Vector.hpp"
#include <math.h>

int findRecur(float x, int start, int end, std::vector<float> arr)
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

float Lerp(float start, float end, float t)
{
	return (end - start) * t + start;
}

float clamp01(float t)
{
	return t <= 0.0f ? 0.0f : (t >= 1.0f ? 1.0 : t);
}

float distance(const Vector2& a, const Vector2& b)
{
	float x = b.x - a.x;
	float y = b.y - a.y;
	return sqrtf(x * x + (y * y));
}

#endif