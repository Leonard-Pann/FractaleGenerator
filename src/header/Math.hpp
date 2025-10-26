#ifndef MATH_HPP
#define MATH_HPP

#include <cmath>
#include "Vector.hpp"

class Math
{
public:

	#define M_PI 3.14159265358979323846f

	static inline float lerp(float start, float end, float t)
	{
		return (end - start) * t + start;
	}

	static inline float clamp01(float value)
	{
		return value <= 0.0f ? 0.0f : (value >= 1.0f ? 1.0f : value);
	}

	static inline int floor(float value)
	{
		return (int)value;
	}

	static inline int ceil(float value)
	{
		return (float)((int)value) ==  value ? (int)value : (int)value + 1;
	}

	static float linePointSqrDistance(const Vector2& A, const Vector2& B, const Vector2& point)
	{
		float r = (((point.x - A.x) * (B.x - A.x)) + ((point.y - A.y) * (B.y - A.y))) / Vector2::sqrDistance(A, B);
		Vector2 P = A + r * (B - A);
		return (0.0f <= r && r <= 1.0f) ? Vector2::sqrDistance(P, point) : (r < 0.0f ? Vector2::sqrDistance(A, point) : Vector2::sqrDistance(B, point));
	}

	static inline float easesin(float x)
	{
		return 0.5f * sinf(M_PI * x + (1.5f * M_PI)) + 0.5f;
	}

	static inline float tween(float x, float intensity = 0.2f)
	{
		return intensity * sinf((2.0f * M_PI * x) + (2.5f * M_PI)) + 1.0f;
	}

	static inline float inverseTween(float x, float intensity = 0.2f)
	{
		return intensity * sinf((2.0f * M_PI * x) + (1.5f * M_PI)) + 1.0f;
	}
};

#endif