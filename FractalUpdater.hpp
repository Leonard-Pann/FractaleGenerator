#ifndef __FRACTAL_UPDATER_HPP
#define __FRACTAL_UPDATER_HPP

#include "FractaleParam.hpp"
#include "Vector.hpp"
#include "HermiteSpline.hpp"

class FractalUpdater
{
private:
	FractaleParam params;
	HermiteSpline seedCurve;
	float tSeed;
	float seedSpeed;

	Vector2 RandomPoint();

public:
	FractalUpdater();

	const FractaleParam& getFractaleParam();

	void init();

	HermiteSpline& getSpline() { return seedCurve; };

	void update(float dt);
};

#endif
