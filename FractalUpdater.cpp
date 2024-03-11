#include "FractalUpdater.hpp"
#include "Random.hpp"

FractalUpdater::FractalUpdater()
{
	
}

FractaleParam& FractalUpdater::getFractaleParam()
{
	return this->params;
}

void FractalUpdater::init()
{
	this->params = FractaleParam(Vector2(-0.75f, 0.0f), -2.0f, 2.0f, -2.0f, 2.0f, Vector3(0.8196, 0.3137, 0.0), 200);
}

void FractalUpdater::update(float dt)
{

}