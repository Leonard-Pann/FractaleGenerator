#ifndef LUT_HPP
#define LUT_HPP

#include <vector>

struct LUT
{
	std::vector<float> x;
	std::vector<float> t;
	int length;

	LUT(std::vector<float>& x, std::vector<float>& t, int length) : x(x), t(t), length(length)
	{

	}

	LUT(const LUT& lut) : x(lut.x), t(lut.t), length(lut.length)
	{

	}

	LUT() : length(0.0f)
	{

	}
};

#endif