#pragma once

inline float lerp(float start, float end, float t)
{
	return (end - start) * t + start;
}
