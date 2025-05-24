#include <algorithm>
#include "FractalUpdater.hpp"
#include "Random.hpp"
#include "Math.hpp"

using namespace std;

vector<vector<Vector3>> FractalUpdater::colorPallets =
{
	// Original
	{
		Vector3(0.0f / 255.0f,   7.f / 255.0f, 100.0f / 255.0f),
		Vector3(32.f / 255.0f, 107.f / 255.0f, 203.f / 255.0f),
		Vector3(237.f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f),
		Vector3(255.0f / 255.0f, 170.0f / 255.0f,   0.0f / 255.0f),
		Vector3(0.0f / 255.0f,   2.f / 255.0f,   0.0f / 255.0f),
		Vector3(0.0f / 255.0f,   7.f / 255.0f, 100.0f / 255.0f)
	},

	//// Fire
	{
		Vector3(20.0f / 255.0f,   0.0f / 255.0f,   0.0f / 255.0f),
		Vector3(255.0f / 255.0f,  20.0f / 255.0f,   0.0f / 255.0f),
		Vector3(255.0f / 255.0f, 200.0f / 255.0f,   0.0f / 255.0f),
		Vector3(255.0f / 255.0f,  20.0f / 255.0f,   0.0f / 255.0f),
		Vector3(20.0f / 255.0f,   0.0f / 255.0f,   0.0f / 255.0f)
	},

	// Electric
	{
		Vector3(0.0f / 255.0f,   0.0f / 255.0f,   0.0f / 255.0f),
		Vector3(0.0f / 255.0f,   0.0f / 255.0f, 200.0f / 255.0f),
		Vector3(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f),
		Vector3(0.0f / 255.0f,   0.0f / 255.0f, 200.0f / 255.0f),
		Vector3(0.0f / 255.0f,   0.0f / 255.0f,   0.0f / 255.0f)
	},

	// Gold
	{
		Vector3(85.f / 255.0f,  47.f / 255.0f,   0.0f / 255.0f),
		Vector3(255.0f / 255.0f, 171.f / 255.0f,  12.f / 255.0f),
		Vector3(255.0f / 255.0f, 247.f / 255.0f, 127.f / 255.0f),
		Vector3(255.0f / 255.0f, 171.f / 255.0f,  12.f / 255.0f),
		Vector3(85.f / 255.0f,  47.f / 255.0f,   0.0f / 255.0f)
	},

	// Black and white gradient
	{
		Vector3(0.0f / 255.0f,   0.0f / 255.0f,   0.0f / 255.0f),
		Vector3(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f),
		Vector3(0.0f / 255.0f,   0.0f / 255.0f,   0.0f / 255.0f)
	}
};

FractalUpdater::FractalUpdater()
{

}

FractalUpdater::FractalUpdater(int screenWidth, int screenHeight) : juliaGreyShader()
{
	//params
	yMin = -1.4f;
	yMax = 1.4f;
	float horizontalSize = ((yMax - yMin) * (float)screenWidth) / (float)screenHeight;
	xMin = -horizontalSize * 0.5f;
	xMax = horizontalSize * 0.5f;

	maxSize = Vector2(xMax - xMin, yMax - yMin);
	minSize = Vector2(maxSize.x / 80000.0f, maxSize.y / 80000.0f);

	juliaOriginThreshold = 0.05f;
	juliaCentroidPointBlackThreshold = 0.05f;
	juliaCentroidPointNeighborhoodThreshold = 0.15f;
	centroidNeighborhoodRadius = 0.015f;

	zoomMinDuration = 10.0f;
	zoomMaxDuration = 10;
	minZoom = 1.0f;
	maxZoom = 1.0f;

	dezoomMinDuration = 4.0f;
	dezoomMaxDuration = 6.0f;

	Vector3 colorIn(0.0f, 0.0f, 0.0f);
	int maxIter = 1000;
	params = FractaleParam(randomPoint(), xMin, xMax, yMin, yMax, colorIn, colorPallets[0], maxIter);
	params.origin = Vector2();

	// For findJuliaOrigin method
	greyTextureWidth = 1920;
	greyTextureHeight = 1080;
	juliaGreyShader.load();
	greyMaxIter = 1000;
	juliaOriginThreshold = 0.0018f;
	textureVariationShader.load();

	// for findRandomPointToZoomInJulia
	juliaCentroidPointBlackThreshold = 0.1f;
	juliaCentroidPointNeighborhoodThreshold = 0.3f;
	centroidNeighborhoodRadius = 0.01f;

	target = generateNewTarget();
	params.origin = target.origin;
	params.xMin = target.zoomPoint.x - (maxSize.x * 0.5f);
	params.xMax = target.zoomPoint.x + (maxSize.x * 0.5f);
	params.yMin = target.zoomPoint.y - (maxSize.y * 0.5f);
	params.yMax = target.zoomPoint.y + (maxSize.y * 0.5f);
	params.redPoint = Vector2(99990.0, 99990.0);

	state = TransitionState::zooming;
	zoomTime = 0.0;
}

FractaleParam& FractalUpdater::getFractaleParam()
{
	return params;
}

Vector2 FractalUpdater::randomPoint()
{
	return Vector2(Random::rand(xMin, xMax), Random::rand(yMin, yMax));
}

void FractalUpdater::update(float dt)
{
	switch (state)
	{
		case FractalUpdater::TransitionState::zooming:
			zoom(dt);
			break;
		case FractalUpdater::TransitionState::dezooming:
			dezoom(dt);
			break;
		case FractalUpdater::TransitionState::changeFractal:
			changeFractal(dt);
			break;
		default:
			break;
	}
}

void FractalUpdater::zoom(float dt)
{
	zoomTime += dt;

	//float k = -log(0.001f) / target.zoomDuration;
	//float alpha = 1.0f - exp(-k * dt);

	float endSizeX = Math::lerp(maxSize.x, minSize.x, target.zoom);
	float sizeX = params.xMax - params.xMin;;
	float growthFactor = pow(endSizeX / sizeX, dt / (target.zoomDuration - zoomTime));
	sizeX *= growthFactor;

	float endSizeY = Math::lerp(maxSize.y, minSize.y, target.zoom);
	float sizeY = params.yMax - params.yMin;;
	growthFactor = pow(endSizeY / sizeY, dt / (target.zoomDuration - zoomTime));
	sizeY *= growthFactor;

	params.xMin = target.zoomPoint.x - (sizeX * 0.5f);
	params.xMax = target.zoomPoint.x + (sizeX * 0.5f);
	params.yMin = target.zoomPoint.y - (sizeY * 0.5f);
	params.yMax = target.zoomPoint.y + (sizeY * 0.5f);

	if (zoomTime >= target.zoomDuration)
	{
		zoomTime = 0.0f;
		state = TransitionState::dezooming;
	}
}

void FractalUpdater::dezoom(float dt)
{
	zoomTime += dt;
	
	float sizeX = params.xMax - params.xMin;;
	float growthFactor = pow(maxSize.x / sizeX, dt / (target.dezoomDuration - zoomTime));
	sizeX *= growthFactor;

	float sizeY = params.yMax - params.yMin;;
	growthFactor = pow(maxSize.y / sizeY, dt / (target.dezoomDuration - zoomTime));
	sizeY *= growthFactor;

	params.xMin = target.zoomPoint.x - (sizeX * 0.5f);
	params.xMax = target.zoomPoint.x + (sizeX * 0.5f);
	params.yMin = target.zoomPoint.y - (sizeY * 0.5f);
	params.yMax = target.zoomPoint.y + (sizeY * 0.5f);

	if (zoomTime >= target.dezoomDuration)
	{
		zoomTime = 0.0f;
		newTarget = generateNewTarget();
		changeFractalTimer = 0.0f;
		state = TransitionState::changeFractal;
	}
}

void FractalUpdater::changeFractal(float dt)
{
	changeFractalTimer += dt;
	float lerpTime = Math::clamp01(changeFractalTimer / 3.0f);

	float x = Math::lerp(target.origin.x, newTarget.origin.x, lerpTime);
	float y = Math::lerp(target.origin.y, newTarget.origin.y, lerpTime);
	Vector2 newOrigin = Vector2(x, y);
	x = Math::lerp(target.zoomPoint.x, newTarget.zoomPoint.x, lerpTime);
	y = Math::lerp(target.zoomPoint.y, newTarget.zoomPoint.y, lerpTime);
	Vector2 newZoomPoint = Vector2(x, y);

	params.origin = newOrigin;
	params.xMin = newZoomPoint.x - (maxSize.x * 0.5f);
	params.xMax = newZoomPoint.x + (maxSize.x * 0.5f);
	params.yMin = newZoomPoint.y - (maxSize.y * 0.5f);
	params.yMax = newZoomPoint.y + (maxSize.y * 0.5f);

	if (changeFractalTimer >= 3.0)
	{
		state = TransitionState::zooming;
		zoomTime = 0.0;
		target = newTarget;
	}
}

tuple<float, vector<float>*> FractalUpdater::getJuliaTotalGreyVariation(int maxIter, Vector2 origin, const Vector4& window)
{
	vector<float>& pixels = *juliaGreyShader.computeTexture(maxIter, origin, window, greyTextureWidth, greyTextureHeight);

	//int widthEnd = greyTextureWidth - 1;
	//int heightEnd = greyTextureHeight - 1;
	//float currentGreyscale;
	//float totalSum(0.0);

	//for (int i = 0; i < greyTextureWidth * greyTextureHeight; i++)
	//{
	//	int row = i / greyTextureWidth;
	//	int col = i % greyTextureWidth;

	//	if (row != 0 && row != greyTextureHeight - 1 && col != 0 && col != greyTextureWidth - 1)
	//	{
	//		int width = greyTextureWidth;
	//		float currentGreyscale = pixels[(width * row) + col];
	//		float sum = abs(currentGreyscale - pixels[(width * (row - 1)) + (col - 1)]);
	//		sum += abs(currentGreyscale - pixels[(width * (row - 1)) + col]);
	//		sum += abs(currentGreyscale - pixels[(width * (row - 1)) + (col + 1)]);
	//		sum += abs(currentGreyscale - pixels[(width * row) + (col - 1)]);
	//		sum += abs(currentGreyscale - pixels[(width * row) + (col + 1)]);
	//		sum += abs(currentGreyscale - pixels[(width * (row + 1)) + (col - 1)]);
	//		sum += abs(currentGreyscale - pixels[(width * (row + 1)) + col]);
	//		sum += abs(currentGreyscale - pixels[(width * (row + 1)) + (col + 1)]);
	//		totalSum += sum / 8.0;
	//	}
	//}

	//float meanCPU = totalSum / (greyTextureWidth * greyTextureHeight);

	float meanGPU = textureVariationShader.computeMeanTextureVariation(pixels, greyTextureWidth, greyTextureHeight);

	//delete &pixels;
	return make_tuple(meanGPU, &pixels);
}

// return a random c € |C such that the absolute difference of gray scale julia fractale is >= threshold
tuple<Vector2, vector<float>*> FractalUpdater::findRandomJuliaOrigin()
{
	Vector4 window(xMin, xMax, yMin, yMax);

	Vector2 randPoint = randomPoint();
	tuple<float, vector<float>*> tuple = getJuliaTotalGreyVariation(greyMaxIter, randPoint, window);
	float mean = get<0>(tuple);
	vector<float>* juliaGreyTexture = get<1>(tuple);

	int i = 1;
	while (mean < juliaOriginThreshold)
	{
		randPoint = randomPoint();
		tuple = getJuliaTotalGreyVariation(greyMaxIter, randPoint, window);
		mean = get<0>(tuple);
		juliaGreyTexture = get<1>(tuple);
		i++;
	}

	return make_tuple(randPoint, juliaGreyTexture);
}

// Find a random z € |C such that Julia(c)(z) <= threshold && meanSum(Julia(c)(zi)) >= threshold2 where zi neighborhood
Vector2 FractalUpdater::findRandomPointToZoomInJulia(Vector2 origin, vector<float>& juliaGreyText)
{
	int randomRow(0);
	int randomCol(0);
	float meanNeighborhoodSum(0.0f);
	int neigPixelSize = max((int)(centroidNeighborhoodRadius * (xMax - xMin)), 1);

	int iter = 0;
	do
	{
		iter++;

		randomRow = Random::randExclude(neigPixelSize, greyTextureHeight - neigPixelSize);
		randomCol = Random::randExclude(neigPixelSize, greyTextureWidth - neigPixelSize);
		float midValue = juliaGreyText[(randomRow * greyTextureWidth) + randomCol];
		if (midValue > juliaCentroidPointBlackThreshold)
			continue;

		meanNeighborhoodSum = 0.0f;

		for (int i = -neigPixelSize; i <= neigPixelSize; i++)
		{
			for (int j = -neigPixelSize; j <= neigPixelSize; j++)
			{
				if (i == 0 && i == j)
				{
					continue;
				}

				int newRow = randomRow + i;
				int newCol = randomCol + j;
				meanNeighborhoodSum += juliaGreyText[(newRow * greyTextureWidth) + newCol];
			}
		}

		meanNeighborhoodSum /= neigPixelSize * neigPixelSize;

		if (meanNeighborhoodSum < juliaCentroidPointNeighborhoodThreshold)
		{
			continue;
		}

		break;
	} while (true);

	float x = Math::lerp(xMin, xMax, (float)randomCol / (float)(greyTextureWidth - 1));
	float y = Math::lerp(yMin, yMax, (float)randomRow / (float)(greyTextureHeight - 1));
	return Vector2(x, y);
}

tuple<Vector2, Vector2> FractalUpdater::findRandomOriginAndZoomPoint()
{
	tuple<Vector2, vector<float>*> tuple = findRandomJuliaOrigin();
	Vector2 origin = get<0>(tuple);
	vector<float>* juliaGreyText = get<1>(tuple);
	Vector2 pointToZoom = findRandomPointToZoomInJulia(origin, *juliaGreyText);

	delete juliaGreyText;

	return make_tuple(origin, pointToZoom);
}

FractalUpdater::StateTarget FractalUpdater::generateNewTarget()
{
	tuple<Vector2, Vector2> tuple = findRandomOriginAndZoomPoint();

	Vector2 randOrigin = get<0>(tuple);
	Vector2 randZoomPoint = get<1>(tuple);
	float zoomDuration = Random::rand(zoomMinDuration, zoomMaxDuration);
	float dezoomDuration = Random::rand(dezoomMinDuration, dezoomMaxDuration);
	float zoom = Random::rand(minZoom, maxZoom);
	
	StateTarget newState = StateTarget(randOrigin, randZoomPoint, zoomDuration, dezoomDuration, zoom);
	return newState;
}