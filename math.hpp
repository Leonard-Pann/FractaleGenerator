#ifndef MATH_HPP
#define MATH_HPP

class Math
{
public:

	static inline float lerp(float start, float end, float t)
	{
		return (end - start) * t + start;
	}

	static inline float min(float a, float b)
	{
		return a < b ? a : b;
	}

	static inline float max(float a, float b)
	{
		return a > b ? a : b;
	}

	static inline float clamp(float min, float max, float value)
	{
		return Math::max(Math::min(max, value), min);
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
};

#endif