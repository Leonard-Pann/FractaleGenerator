#include "FractalUpdater.hpp"
#include "Random.hpp"

FractalUpdater::FractalUpdater()
{
	
}

const FractaleParam& FractalUpdater::getFractaleParam()
{
	return params;
}

Vector2 FractalUpdater::RandomPoint()
{
	return Vector2(Random::Rand(params.xMin, params.xMax), Random::Rand(params.yMin, params.yMax));
}

void FractalUpdater::init()
{
	params = FractaleParam(Vector2(), -1.7f, 1.7f, -1.7f, 1.7f, Vector3(0.937f, 1.0f, 0.541f), Vector3(0.6f, 0.176f, 0.0f), 1000);
	
	Vector2 startSeed = RandomPoint();
	std::vector<Vector2> points = std::vector<Vector2>(7);
	points[0] = startSeed;
	points[1] = RandomPoint();
	points[2] = RandomPoint();
	points[3] = RandomPoint();
	points[4] = RandomPoint();
	points[5] = RandomPoint();
	points[6] = RandomPoint();
	seedCurve = HermiteSpline(points);
	tSeed = 0.0f;
	seedSpeed = 0.025f;

	params.seed = startSeed;
}

void FractalUpdater::update(float dt)
{
	tSeed += dt * seedSpeed;

	if (tSeed > 1.0f)
	{
		params.seed = seedCurve.getEnd();
		std::vector<Vector2> points = std::vector<Vector2>(7);
		points[0] = params.seed;
		points[1] = RandomPoint();
		points[2] = RandomPoint();
		points[3] = RandomPoint();
		points[4] = RandomPoint();
		points[5] = RandomPoint();
		points[6] = RandomPoint();
		seedCurve = HermiteSpline(points);
		tSeed = 0.0f;
	}
	else
	{
		params.seed = seedCurve.EvaluateDistance(tSeed);
	}
}