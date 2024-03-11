#ifndef __FRACTAL_UPDATER_HPP
#define __FRACTAL_UPDATER_HPP

#include "FractaleParam.hpp"

class FractalUpdater
{
private:
	FractaleParam params;

public:
	FractalUpdater();

	FractaleParam& getFractaleParam();

	void init();

	void update(float dt);
};

#endif
