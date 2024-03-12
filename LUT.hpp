#ifndef __LUT_HPP
#define __LUT_HPP

#include <vector>

struct LUT
{
	std::vector<float> x;
	std::vector<float> t;
	int length;

	LUT(std::vector<float>& x, std::vector<float>& t, int length)
	{
		this->x = x;
		this->t = t;
		this->length = length;
	}

	LUT()
	{
		this->length = 0;
	}
};

#endif