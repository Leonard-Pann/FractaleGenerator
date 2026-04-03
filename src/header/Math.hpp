#ifndef MATH_HPP
#define MATH_HPP

#include <cmath>
#include "Vector.hpp"
#include "Random.hpp"

class Math
{
public:

	// #define M_PI 3.14159265358979323846f

	static inline float lerp(float start, float end, float t)
	{
		return (end - start) * t + start;
	}

	static inline float clamp01(float value)
	{
		return value <= 0.0f ? 0.0f : (value >= 1.0f ? 1.0f : value);
	}

	static inline float clamp(float value, float min, float max)
	{
		return value <= min ? min : (value >= max ? max : value);
	}

	static inline int clamp(int value, int min, int max)
	{
		return value <= min ? min : (value >= max ? max : value);
	}

	static inline int floor(float value)
	{
		return (int)value;
	}

	static inline int ceil(float value)
	{
		return (float)((int)value) ==  value ? (int)value : (int)value + 1;
	}

	static inline int round(float value)
	{
		int floor = (int)value;
		return value - floor >= 0.5f ? floor + 1 : floor;
	}

	static inline float round(float value, int decimal)
	{
		decimal = clamp(decimal, 0, 8);
		double exp = pow(10.0, decimal);
		return ((double)(int64_t)(value * exp)) / exp;
	}

	static inline bool isOdd(int n)
	{
		return (n & 1) != 0;
	}

	static inline bool isEven(int n)
	{
		return (n & 1) == 0;
	}

	static float linePointSqrDistance(const Vector2& A, const Vector2& B, const Vector2& point)
	{
		float r = (((point.x - A.x) * (B.x - A.x)) + ((point.y - A.y) * (B.y - A.y))) / Vector2::sqrDistance(A, B);
		Vector2 P = A + r * (B - A);
		return (0.0f <= r && r <= 1.0f) ? Vector2::sqrDistance(P, point) : (r < 0.0f ? Vector2::sqrDistance(A, point) : Vector2::sqrDistance(B, point));
	}

	static bool collideLines(const Vector2& A, const Vector2& B, const Vector2& O, const Vector2& P)
	{
		return collideLineStraightLine(A, B, O, P) && collideLineStraightLine(O, P, A, B);
	}

	static bool collideLines(const Vector2& A, const Vector2& B, const Vector2& O, const Vector2& P, Vector2& inter)
	{
		if (!collideLineStraightLine(A, B, O, P) || !collideLineStraightLine(O, P, A, B))
			return false;

		return collideStraightLines(A, B, O, P, inter);
	}

	static bool collideLineStraightLine(const Vector2& O, const Vector2& P, const Vector2& A, const Vector2& B)
	{
		Vector2 AB(B - A);
		Vector2 AP(P - A);
		Vector2 AO(O - A);
		return (AB.x * AP.y - AB.y * AP.x) * (AB.x * AO.y - AB.y * AO.x) < 0.0f;
	}

	static bool collideLineStraightLine(const Vector2& O, const Vector2& P, const Vector2& A, const Vector2& B, Vector2& inter)
	{
		if (collideLineStraightLine(O, P, A, B))
		{
			return collideStraightLines(O, P, A, B, inter);
		}

		return false;
	}

	static bool collideStraightLines(const Vector2& A, const Vector2& B, const Vector2& O, const Vector2& P, Vector2& inter)
	{
		float num(B.x - A.x);
		float num2(B.y - A.y);
		float num3(O.x - P.x);
		float num4(O.y - P.y);
		float num5(num * num4 - num2 * num3);

		if (abs(num5) <= 1e-3f)
			return false;

		float num6(O.x - A.x);
		float num7(O.y - A.y);
		float num8((num6 * num4 - num7 * num3) / num5);
		inter.x = A.x + num8 * num;
		inter.y = A.y + num8 * num2;
		return true;
	}

	static Vector2 normalVector(const Vector2& v)
	{
		if (abs(v.x) > 1e-3f)
		{
			float y = sqrtf(1.0f / (((v.y * v.y) / (v.x * v.x)) + 1.0f));
			return { -v.y * y / v.x, y };
		}
		else if (abs(v.x) > 1e-3f)
		{
			float x = sqrtf(1.0f / (1.0f + ((v.x * v.x) / (v.y * v.y))));
			return { x, -v.x * x / v.y };
		}
		else
		{
			return { 1.0f, 0.0f };
		}
	}

	static int intersectSegment(const Vector2& A, const Vector2& B, const Vector2& I, const Vector2& P)
	{
		Vector2 D(B - A);
		Vector2 E(P - I);
		float denom = D.x * E.y - D.y * E.x;
		if (abs(denom) < 1e-3f)
			return -1; //error, limit case
		float t = -(A.x * E.y - I.x * E.y - E.x * A.y + E.x * I.y) / denom;
		if (t < 0.0f || t >= 1.0f)
			return 0;
		float u = -(-D.x * A.y + D.x * I.y + D.y * A.x - D.y * I.x) / denom;
		if (u < 0.0f || u >= 1.0f)
			return 0;
		return 1;
	}

	static bool polygoneContains(const std::vector<Vector2>& vertices, const Vector2& point)
	{
		if (vertices.size() < 3)
			return false;

		Vector2 center;

		for (const Vector2& p : vertices)
		{
			center = center + p;
		}
		center.x = center.x / vertices.size();
		center.y = center.y / vertices.size();

		float radius = 3.40282347e+38F;
		for (const Vector2& p : vertices)
		{
			float currentDist = Vector2::sqrDistance(p, center);
			if (currentDist < radius)
			{
				radius = currentDist;
			}
		}

		float randAngle = Random::rand(0.0f, M_PI * 2.0f);
		Vector2 I(2.0f * radius * cosf(randAngle) + center.x, 2.0f * radius * sinf(randAngle) + center.y);
		Vector2 A, B;
		int nbintersections(0);
		int iseg;
		for (int i = 0; i < vertices.size(); i++)
		{
			A = vertices[i];
			B = vertices[(i + 1) % vertices.size()];
			iseg = intersectSegment(A, B, I, point);
			if (iseg == -1)
				return polygoneContains(vertices, point); // cas limite, on relance la fonction.
			nbintersections += iseg;
		}

		return Math::isOdd(nbintersections);
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