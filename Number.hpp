#ifndef NUMBER_HPP
#define NUMBER_HPP

#include "Math.hpp"

struct Number
{
private:
	float m_value;

public:
	Number() : m_value(0.0f) {}
	Number(float value) : m_value(value) {}
	Number(const Number& number) : m_value(number.m_value){}

	float value() const { return this->m_value; }
	void value(float value) { this->m_value = value; }
	operator float() const { return m_value; }
	static float distance(const Number& a, const Number& b)
	{
		return Math::abs(a.m_value - b.m_value);
	}
};

static Number operator*(const Number& number, float other)
{
	return Number(number.value() * other);
}

static Number operator*(float other, const Number& number)
{
	return Number(number.value() * other);
}

static Number operator+(const Number& left, const Number& right)
{
	return Number(left.value() + right.value());
}

static Number operator-(const Number& left, const Number& right)
{
	return Number(left.value() - right.value());
}

#endif