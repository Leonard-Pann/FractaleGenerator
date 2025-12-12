#pragma region include

#include <algorithm>
#include <iostream>
#include <thread>
#include "FractalUpdater.hpp"
#include "Random.hpp"
#include "Math.hpp"

using namespace std;

#pragma endregion

#pragma region pallets

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

	// Fire
	{
		Vector3(20.0f / 255.0f,   0.0f / 255.0f,   0.0f / 255.0f),
		Vector3(255.0f / 255.0f,  20.0f / 255.0f,   0.0f / 255.0f),
		Vector3(255.0f / 255.0f, 175.0f / 255.0f,   0.0f / 255.0f),
		Vector3(255.0f / 255.0f,  20.0f / 255.0f,   0.0f / 255.0f),
		Vector3(60.0f / 255.0f,   40.0f / 255.0f,   0.0f / 255.0f),
		Vector3(40.0f / 255.0f,   20.0f / 255.0f,   0.0f / 255.0f)
	},

	// Electric
	{
		Vector3(0.0f / 255.0f,   0.0f / 255.0f,   0.0f / 255.0f),
		Vector3(0.0f / 255.0f,   0.0f / 255.0f, 200.0f / 255.0f),
		Vector3(220.0f / 255.0f, 220.0f / 255.0f, 255.0f / 255.0f),
		Vector3(0.0f / 255.0f,   0.0f / 255.0f, 200.0f / 255.0f),
		Vector3(0.0f / 255.0f,   0.0f / 255.0f,   100.0f / 255.0f),
		Vector3(0.0f / 255.0f,   0.0f / 255.0f,   0.0f / 255.0f)
	},

	// Gold
	{
		Vector3(85.f / 255.0f,  47.f / 255.0f,   0.0f / 255.0f),
		Vector3(255.0f / 255.0f, 171.f / 255.0f,  12.f / 255.0f),
		Vector3(255.0f / 255.0f, 247.f / 255.0f, 127.f / 255.0f),
		Vector3(255.0f / 255.0f, 171.f / 255.0f,  12.f / 255.0f),
		Vector3(85.0f / 255.0f,  47.f / 255.0f,   0.0f / 255.0f),
		Vector3(40.0f / 255.0f,  40.0f / 255.0f,   40.0f / 255.0f),
	},

	//// Black and white gradient
	//{
	//	Vector3(0.0f, 0.0f, 0.0f),
	//	Vector3(1.0f, 1.0f, 1.0f),
	//	Vector3(0.5f, 0.5f, 0.5f),
	//	Vector3(0.25f, 0.25f, 0.25f),
	//	Vector3(0.0f, 0.0f, 0.0f),
	//	Vector3(0.0f, 0.0f, 0.0f)
	//},

	//// Radoactive
	//{
	//	Vector3(40.0f / 255.0f,  90.0f / 255.0f,   25.0f / 255.0f),
	//	Vector3(40.0f / 255.0f,  255.0f / 255.0f,   0.0f / 255.0f),
	//	Vector3(40.0f / 255.0f,  150.0f / 255.0f,   35.0f / 255.0f),
	//	Vector3(40.0f / 255.0f,  255.0f / 255.0f,   0.0f / 255.0f),
	//	Vector3(40.0f / 255.0f,  90 / 255.0f,   25.0f / 255.0f),
	//	Vector3(40.0f / 255.0f,  40.0f / 255.0f,   40.0f / 255.0f)
	//}
};

#pragma endregion

#pragma region ctor

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
	minSize = Vector2(maxSize.x / 30000.0f, maxSize.y / 30000.0f);

	// Zoom
	zoomStartOffset = -0.4f;
	zoomMinDuration = 25.0f; 
	zoomMaxDuration = 25.0f;
	minZoom = 1.0f;
	maxZoom = 1.0f;
	zoomTweenIntensity = 0.0007f;

	// Dezoom
	dezoomMinDuration = 15.0f;
	dezoomMaxDuration = 15.0f;

	// For findJuliaOrigin method
	greyTextureWidth = 1920;
	greyTextureHeight = 1080;
	juliaGreyShader.load();
	greyMaxIter = 1000;
	juliaOriginThreshold = 0.0018f;

	// for findRandomPointToZoomInJulia
	float refineZoomIterOn1080pScreen = 10.0f;
	refiningPointToZoomIter = Math::ceil(((float)screenWidth / 1920.0f) * refineZoomIterOn1080pScreen);

	// Change Fractal
	changeFractalDuration = 25.0f;
	changeFractalStartOffset = -0.2f;
	minNbOrigines = 6; // min 3
	maxNbOrigines = 6;

	//target
	useGPUAcceleration = false;
	millisecondToSleepAfterCallComputerSharder = 75;
	generateNewTarget(nullptr);

	target = newTarget;
	generateNewTargetOtherThread(target);

	//Colors
	nbColorsInPalet = (int)colorPallets[0].size();
	nbColorsInAnUpdateCircle = 3;
	colorsSplines = generateNewPallets();
	colorTimer = 0.0f;
	vector<Vector3>* currentPallet = getCurrentColorPallet(colorsSplines);
	minColorRange = 2.0f;
	maxColorRange = 6.0f;
	colorRangeDuration = 300.0f;
	nbColorRangeInSpline = 15;
	colorRangeTimer = 0.0f;
	generateColorRangeSpline();

	//dezoom
	dezoomTarget = target;
	dezoomColorsSplines = colorsSplines;

	Vector3 colorIn(0.0f, 0.0f, 0.0f);
	int maxIter = 750;

	Vector2 startZoomPoint = target->getZoomPoint(0.0f);
	int xMin = startZoomPoint.x - (maxSize.x * 0.5f);
	int xMax = startZoomPoint.x + (maxSize.x * 0.5f);
	int yMin = startZoomPoint.y - (maxSize.y * 0.5f);
	int yMax = startZoomPoint.y + (maxSize.y * 0.5f);

	float a = colorRangeSpline.getStart();
	params = FractaleParam(target->getOrigin(0.0f), xMin, xMax, yMin, yMax, colorIn, *currentPallet, maxIter, colorRangeSpline.getStart().value());
	delete currentPallet;

	isChangingFractal = true;
	isZooming = false;
	isDezooming = false;
	changeFractalTimer = 0.0f;
	zoomTime = 0.0f;
	changeFractalSize = maxSize;
}

#pragma endregion

#pragma region getter/utils

FractaleParam& FractalUpdater::getFractaleParam()
{
	return params;
}

Vector2 FractalUpdater::randomPoint() const
{
	return Vector2(Random::rand(xMin, xMax), Random::rand(yMin, yMax));
}

void FractalUpdater::generateColorRangeSpline()
{
	vector<Number> points(nbColorRangeInSpline + 2);
	if (colorRangeSpline.getPoints().size() <= 0)
	{
		for (size_t i = 0; i < points.size(); i++)
		{
			points[i] = Random::rand(minColorRange, maxColorRange);
		}
	}
	else
	{
		const vector<Number>& oldPoints = colorRangeSpline.getPoints();
		points[0] = oldPoints[oldPoints.size() - 2];
		points[1] = oldPoints[oldPoints.size() - 1];
		for (size_t i = 2; i < points.size(); i++)
		{
			points[i] = Random::rand(minColorRange, maxColorRange);
		}
	}

	colorRangeSpline = CatmulRomSpline<Number>(points);
}

#pragma endregion

#pragma region update

void FractalUpdater::update(float dt)
{
	{
		lock_guard<mutex> lock(mutexName);
		if (isAFunctionToCallInMainThread)
		{
			callbackResult = callback(callbackArg);
			isAFunctionToCallInMainThread = false;
			callback = nullptr;
		}
	}

	colorTimer += dt;

	if (isDezooming)
	{
		dezoom(dt);
	}

	if (isZooming)
	{
		zoom(dt);
	}

	if (isChangingFractal)
	{
		changeFractal(dt);
	}

	colorRangeTimer += dt;
	Number colorRange = colorRangeSpline.evaluateDistance(colorRangeTimer / colorRangeDuration);

	params.colorRange = colorRange.value();
	if (colorRangeTimer >= colorRangeDuration)
	{
		colorRangeTimer -= colorRangeDuration;
		generateColorRangeSpline();
	}
}

void FractalUpdater::zoom(float dt)
{
	zoomTime += dt;

	float startSizeX = maxSize.x;
	float endSizeX = Math::lerp(maxSize.x, minSize.x, target->zoom);
	float sizeX = params.xMax - params.xMin;
	float q = pow(endSizeX / startSizeX, dt / target->zoomDuration);
	float lerp = zoomTime / target->zoomDuration;
	float factor = Math::tween(lerp, zoomTweenIntensity);
	q *= factor;
	sizeX *= q;
	sizeX = max(sizeX, endSizeX);

	float startSizeY = maxSize.y;
	float endSizeY = Math::lerp(maxSize.y, minSize.y, target->zoom);
	float sizeY = params.yMax - params.yMin;
	q = pow(endSizeY / startSizeY, dt / target->zoomDuration);
	q *= factor;
	sizeY *= q;
	sizeY = max(sizeY, endSizeY);

	if (isChangingFractal)
	{
		changeFractalSize.x = sizeX;
		changeFractalSize.y = sizeY;
	}
	else
	{
		params.xMin = target->finalZoomPoint().x - (sizeX * 0.5f);
		params.xMax = target->finalZoomPoint().x + (sizeX * 0.5f);
		params.yMin = target->finalZoomPoint().y - (sizeY * 0.5f);
		params.yMax = target->finalZoomPoint().y + (sizeY * 0.5f);
	}

	vector<Vector3>* currentPallet = getCurrentColorPallet(colorsSplines);
	params.colorPalette = *currentPallet;
	delete currentPallet;

	if (zoomTime >= target->zoomDuration)
	{
		zoomTime = 0.0f;
		isZooming = false;
		isDezooming = true;
	}
}

void FractalUpdater::dezoom(float dt)
{
	zoomTime += dt;

	float startSizeX = Math::lerp(maxSize.x, minSize.x, dezoomTarget->zoom);
	float endSizeX = maxSize.x;
	float sizeX = params.xMax - params.xMin;
	float q = pow(endSizeX / startSizeX, dt / dezoomTarget->dezoomDuration);
	float lerp = zoomTime / dezoomTarget->dezoomDuration;
	float factor = Math::inverseTween(lerp, zoomTweenIntensity);
	q *= factor;
	sizeX *= q;
	sizeX = min(sizeX, endSizeX);

	float startSizeY = Math::lerp(maxSize.y, minSize.y, dezoomTarget->zoom);
	float endSizeY = maxSize.y;
	float sizeY = params.yMax - params.yMin;
	q = pow(endSizeY / startSizeY, dt / dezoomTarget->dezoomDuration);
	q *= factor;
	sizeY *= q;
	sizeY = min(sizeY, endSizeY);

	if (isChangingFractal)
	{
		changeFractalSize.x = sizeX;
		changeFractalSize.y = sizeY;
	}
	else
	{
		params.xMin = dezoomTarget->finalZoomPoint().x - (sizeX * 0.5f);
		params.xMax = dezoomTarget->finalZoomPoint().x + (sizeX * 0.5f);
		params.yMin = dezoomTarget->finalZoomPoint().y - (sizeY * 0.5f);
		params.yMax = dezoomTarget->finalZoomPoint().y + (sizeY * 0.5f);
	}

	if (!isChangingFractal && zoomTime - changeFractalStartOffset >= dezoomTarget->dezoomDuration)
	{
		while (!isNewTargetReady)
		{
			this_thread::sleep_for(chrono::milliseconds(1));
		}

		target = newTarget;

		generateNewTargetOtherThread(target);

		colorsSplines = generateNewPallets();

		changeFractalSize.x = sizeX;
		changeFractalSize.y = sizeY;
		isChangingFractal = true;
		changeFractalTimer = 0.0f;
	}

	if (zoomTime >= dezoomTarget->dezoomDuration)
	{
		zoomTime = 0.0f;
		changeFractalSize = maxSize;

		delete dezoomTarget;
		delete dezoomColorsSplines;
		dezoomColorsSplines = colorsSplines;
		dezoomTarget = target;
		colorTimer = 0.0f;
		isDezooming = false;
	}

	vector<Vector3>* currentPallet = getCurrentColorPallet(dezoomColorsSplines);
	params.colorPalette = *currentPallet;
	delete currentPallet;
}

void FractalUpdater::changeFractal(float dt)
{
	changeFractalTimer += dt;
	float lerpTime = Math::clamp01(changeFractalTimer / changeFractalDuration);

	Vector2 newOrigin = target->getOrigin(lerpTime);
	Vector2 newZoomPoint = target->getZoomPoint(lerpTime);

	params.origin = newOrigin;
	params.xMin = newZoomPoint.x - (changeFractalSize.x * 0.5f);
	params.xMax = newZoomPoint.x + (changeFractalSize.x * 0.5f);
	params.yMin = newZoomPoint.y - (changeFractalSize.y * 0.5f);
	params.yMax = newZoomPoint.y + (changeFractalSize.y * 0.5f);

	if (!isDezooming && !isZooming)
	{
		vector<Vector3>* currentPallet = getCurrentColorPallet(colorsSplines);
		params.colorPalette = *currentPallet;
		delete currentPallet;
	}

	if (!isZooming && changeFractalTimer - zoomStartOffset >= changeFractalDuration)
	{
		isZooming = true;
		zoomTime = 0.0;
	}

	if (changeFractalTimer >= changeFractalDuration)
	{
		isChangingFractal = false;
	}
}

vector<CatmulRomSpline<Vector3>>* FractalUpdater::generateNewPallets()
{
	vector<vector<Vector3>> pallets(nbColorsInPalet);
	for (int i = 0; i < nbColorsInPalet; i++)
	{
		pallets[i].reserve(nbColorsInAnUpdateCircle);
	}

	if (colorsSplines == nullptr)
	{
		colorsSplines = new vector<CatmulRomSpline<Vector3>>();
		colorsSplines->reserve(nbColorsInPalet);
		int lastIndex = -1;
		for (int i = 0; i < nbColorsInAnUpdateCircle; i++)
		{
			int randIndex = Random::randExclude(0, colorPallets.size());
			while (randIndex == lastIndex)
			{
				randIndex = Random::randExclude(0, colorPallets.size());
			}

			for (int j = 0; j < nbColorsInPalet; j++)
			{
				pallets[j].push_back(colorPallets[randIndex][j]);
			}

			lastIndex = randIndex;
		}
	}
	else
	{
		for (int i = 0; i < nbColorsInPalet; i++)
		{
			pallets[i].push_back((*colorsSplines)[i].getEnd());
		}

		int lastIndex = -1;
		for (int i = 1; i < nbColorsInAnUpdateCircle; i++)
		{
			int randIndex = Random::randExclude(0, colorPallets.size());
			while (randIndex == lastIndex)
			{
				randIndex = Random::randExclude(0, colorPallets.size());
			}

			for (int j = 0; j < nbColorsInPalet; j++)
			{
				pallets[j].push_back(colorPallets[randIndex][j]);
			}

			lastIndex = randIndex;
		}
	}

	vector<CatmulRomSpline<Vector3>>* newPallet = new vector<CatmulRomSpline<Vector3>>();
	newPallet->reserve(nbColorsInPalet);
	for (int i = 0; i < nbColorsInPalet; i++)
	{
		newPallet->push_back(CatmulRomSpline<Vector3>(pallets[i]));
	}
	return newPallet;
}

vector<Vector3>* FractalUpdater::getCurrentColorPallet(vector<CatmulRomSpline<Vector3>>* pallet)
{
	float totalDuration = target->zoomDuration + target->dezoomDuration + changeFractalDuration + changeFractalStartOffset + zoomStartOffset;
	float t = Math::clamp01(colorTimer / totalDuration);

	vector<Vector3>* currentPallet = new vector<Vector3>(nbColorsInPalet);
	for (int i = 0; i < nbColorsInPalet; i++)
	{
		(*currentPallet)[i] = (*pallet)[i].evaluateDistance(t);
	}

	return currentPallet;
}


#pragma endregion

#pragma region getJuliaTotalGreyVariation

static float pow64(float x)
{
	float tmp = x * x;
	x = tmp * tmp;
	tmp = x * x;
	x = tmp * tmp;
	tmp = x * x;
	return tmp * tmp;
}

static vector<float>* computeGreyTexture(int maxIter, Vector2 seed, Vector4 window, int greyTextureWidth, int greyTextureHeight)
{
	vector<float>* pixels = new vector<float>(greyTextureWidth * greyTextureHeight);

	float cx(seed.x);
	float cy(seed.y);

	float xStep((window.y - window.x) / static_cast<float>(greyTextureWidth - 1));
	float yStep((window.w - window.z) / static_cast<float>(greyTextureHeight - 1));

	for(int row = 0; row < greyTextureHeight; row++)
	{
		for(int col = 0; col < greyTextureWidth; col++)
		{
			float x = static_cast<float>(col) * xStep + window.x;
			float y = static_cast<float>(row) * yStep + window.z;
			float currentx = x;
    		float currenty = y;
			float xTmp;
			int nbIter(0);
			while (currentx * currentx + (currenty * currenty) < 4.0 && nbIter < maxIter)
			{
				xTmp = currentx;
				currentx = (xTmp * xTmp) - (currenty * currenty) + cx;
				currenty = 2.0 * xTmp * currenty + cy;
				nbIter++;
			}

			int index = (row * greyTextureWidth) + col;
			if (nbIter >= maxIter)
			{
				(*pixels)[index] = 0.0f;
			}
			else
			{
				float v = static_cast<float>(nbIter) / static_cast<float>(maxIter);
				float value = pow64(1.0 - v);
				(*pixels)[index] = value;
			}
		}
	}

	return pixels;
}

static float computeGreyVariation(vector<float>* pixels, int greyTextureWidth, int greyTextureHeight)
{
	const float oneO8 = 1.0 / 8.0;

	float currentGreyscale(0.0);
	float totalSum(0.0);
	float partialSum(0.0);

	int start = greyTextureWidth + 1;
	int end = greyTextureWidth * (greyTextureHeight - 1) - 1;
	
	for (int i = start; i < end; i++)
	{
		int col = i % greyTextureWidth;
		if(col == greyTextureWidth - 1)
		{
			i++;
			totalSum += partialSum * oneO8;
			partialSum = 0.0;
			continue;
		}

		int row = i / greyTextureWidth;
		int colM1 = col - 1;
		int colP1 = col + 1;
		int wTRowM1 = greyTextureWidth * (row - 1);
		int wTRow = wTRowM1 + greyTextureWidth; 
		int wTRowP1 = wTRow + greyTextureWidth; 

		float currentGreyscale = (*pixels)[wTRow + col];
		partialSum += abs(currentGreyscale - (*pixels)[wTRowM1 + colM1]);
		partialSum += abs(currentGreyscale - (*pixels)[wTRowM1 + col]);
		partialSum += abs(currentGreyscale - (*pixels)[wTRowM1 + colP1]);
		partialSum += abs(currentGreyscale - (*pixels)[wTRow + colM1]);
		partialSum += abs(currentGreyscale - (*pixels)[wTRow + colP1]);
		partialSum += abs(currentGreyscale - (*pixels)[wTRowP1 + colM1]);
		partialSum += abs(currentGreyscale - (*pixels)[wTRowP1 + col]);
		partialSum += abs(currentGreyscale - (*pixels)[wTRowP1 + colP1]);
	}

	totalSum += partialSum * oneO8;
	float meanCPU = totalSum / (greyTextureWidth * greyTextureHeight);

	return meanCPU;
}

tuple<float, vector<float>*> FractalUpdater::getJuliaTotalGreyVariation(int maxIter, Vector2 origin, const Vector4& window)
{
	vector<float>* pixels(nullptr);
	if(useGPUAcceleration)
	{
		pixels = juliaGreyShader.computeTexture(maxIter, origin, window, greyTextureWidth, greyTextureHeight);
	}
	else
	{
		pixels = computeGreyTexture(maxIter, origin, window, greyTextureWidth, greyTextureHeight);
	}

	float meanCPU = computeGreyVariation(pixels, greyTextureWidth, greyTextureHeight);

	return make_tuple(meanCPU, pixels);
}

#pragma endregion

#pragma region findRandomJuliaOrigin

// return a random c € |C such that the absolute difference of gray scale julia fractale is >= threshold
tuple<Vector2, vector<float>*> FractalUpdater::findRandomJuliaOrigin()
{
	Vector4 window(xMin, xMax, yMin, yMax);

	Vector2 randPoint = randomPoint();

	tuple<float, vector<float>*> tuple = getJuliaTotalGreyVariation(greyMaxIter, randPoint, window);

	float mean = get<0>(tuple);
	vector<float>* juliaGreyTexture = get<1>(tuple);

	while (mean < juliaOriginThreshold)
	{
		delete juliaGreyTexture;

		randPoint = randomPoint();
		tuple = getJuliaTotalGreyVariation(greyMaxIter, randPoint, window);
		mean = get<0>(tuple);
		juliaGreyTexture = get<1>(tuple);
	}

	return make_tuple(randPoint, juliaGreyTexture);
}

struct JuliaTotalGreyVariationOtherThreadParams
{
	int maxIter;
	Vector2 origin;
	Vector4 window;
	JuliaGreyShader* juliaGreyShader;
	int greyTextureWidth;
	int greyTextureHeight;
};

struct JuliaTotalGreyVariationOtherThreadReturn
{
	vector<float>* texture;
};

void* getJuliaTotalGreyVariationOtherThread(void* params)
{
	JuliaTotalGreyVariationOtherThreadParams* paramsCast = (JuliaTotalGreyVariationOtherThreadParams*)params;
	int maxIter = paramsCast->maxIter;
	Vector2 origin = paramsCast->origin;
	Vector4 window = paramsCast->window;
	JuliaGreyShader* juliaGreyShader = paramsCast->juliaGreyShader;
	int greyTextureWidth = paramsCast->greyTextureWidth;
	int greyTextureHeight = paramsCast->greyTextureHeight;

	vector<float>* pixels = juliaGreyShader->computeTexture(maxIter, origin, window, greyTextureWidth, greyTextureHeight);

	JuliaTotalGreyVariationOtherThreadReturn* res = new JuliaTotalGreyVariationOtherThreadReturn();
	res->texture = pixels;
	return res;
}

// return a random c € |C such that the absolute difference of gray scale julia fractale is >= threshold
tuple<Vector2, vector<float>*> FractalUpdater::findRandomJuliaOriginOtherThread()
{
	JuliaTotalGreyVariationOtherThreadParams params;
	params.maxIter = greyMaxIter;
	params.window = Vector4(xMin, xMax, yMin, yMax);
	params.greyTextureHeight = greyTextureHeight;
	params.greyTextureWidth = greyTextureWidth;
	params.juliaGreyShader = &juliaGreyShader;
	JuliaTotalGreyVariationOtherThreadReturn* result;

	Vector2 randonPoint;
	vector<float>* juliaGreyTexture = nullptr;
	float mean;
	bool isCallFinish = false;

	do
	{
		if (juliaGreyTexture != nullptr)
		{
			delete juliaGreyTexture;
		}

		randonPoint = randomPoint();
		params.origin = randonPoint;

		{
			lock_guard<mutex> lock(mutexName);
			callbackArg = &params;
			callback = getJuliaTotalGreyVariationOtherThread;
			isCallFinish = false;
			isAFunctionToCallInMainThread = true;
		}

		while (!isCallFinish)
		{
			this_thread::sleep_for(std::chrono::milliseconds(1));
			lock_guard<mutex> lock(mutexName);
			isCallFinish = !isAFunctionToCallInMainThread;
		}

		result = (JuliaTotalGreyVariationOtherThreadReturn*)callbackResult;
		mean = computeGreyVariation(result->texture, greyTextureWidth, greyTextureHeight);
		juliaGreyTexture = result->texture;

		delete result;

		this_thread::sleep_for(chrono::milliseconds(millisecondToSleepAfterCallComputerSharder));

	} while (mean < juliaOriginThreshold);

	return make_tuple(randonPoint, juliaGreyTexture);
}

#pragma endregion

#pragma region findRandomPointToZoomInJulia

tuple<bool, Vector2> FractalUpdater::initRandomPointToZoom(Vector2 origin, vector<float>& initJuliaGreyText)
{
	vector<int> zeroPoints = vector<int>();
	zeroPoints.reserve(initJuliaGreyText.size());

	int end = greyTextureWidth * (greyTextureHeight - 1);
	for (int i = greyTextureWidth; i < end; i++)
	{
		if (initJuliaGreyText[i] <= 0.0f)
		{
			zeroPoints.push_back(i);
		}
	}

	if (zeroPoints.size() > 0)
	{
		Random::shuffle(zeroPoints);

		int randomRow(-1);
		int randomCol(-1);
		for (int index : zeroPoints)
		{
			int col = index % greyTextureWidth;
			if (col == 0 || col == greyTextureHeight - 1)
			{
				continue;
			}

			int row = index / greyTextureWidth;
			bool find = false;

			if (initJuliaGreyText[(greyTextureWidth * (row + 1)) + col] > 0.0f)
			{
				randomRow = row;
				randomCol = col;
				find = true;
			}

			if (initJuliaGreyText[(greyTextureWidth * (row - 1)) + col] > 0.0f)
			{
				randomRow = row;
				randomCol = col;
				find = true;
			}

			if (initJuliaGreyText[(greyTextureWidth * row) + (col + 1)] > 0.0f)
			{
				randomRow = row;
				randomCol = col;
				find = true;
			}

			if (initJuliaGreyText[(greyTextureWidth * (row + 1)) + (col - 1)] > 0.0f)
			{
				randomRow = row;
				randomCol = col;
				find = true;
			}

			if (find)
				break;
		}

		if (randomRow >= 0 && randomCol >= 0)
		{
			float x = Math::lerp(xMin, xMax, (float)randomCol / (float)(greyTextureWidth - 1));
			float y = Math::lerp(yMin, yMax, (float)randomRow / (float)(greyTextureHeight - 1));
			return make_tuple(true, Vector2(x, y));
		}
	}

	float minValue = initJuliaGreyText[0];
	int minIndex = 0;

	end = greyTextureWidth * greyTextureHeight;
	for (int i = 1; i < end; i++)
	{
		if (initJuliaGreyText[i] < minValue)
		{
			minValue = initJuliaGreyText[i];
			minIndex = i;
		}
	}

	int col = minIndex % greyTextureWidth;
	int row = minIndex / greyTextureWidth;
	float x = Math::lerp(xMin, xMax, (float)col / (float)(greyTextureWidth - 1));
	float y = Math::lerp(yMin, yMax, (float)row / (float)(greyTextureHeight - 1));

	return make_tuple(false, Vector2(x, y));
}

struct ComputeJuliaTextureParams
{
	int maxIter;
	Vector2 origin;
	Vector4 window;
	JuliaGreyShader* juliaGreyShader;
	int greyTextureWidth;
	int greyTextureHeight;
};

struct ComputeJuliaTextureReturn
{
	vector<float>* texture;
};

void* computeJuliaTexture(void* params)
{
	ComputeJuliaTextureParams* paramsCast = (ComputeJuliaTextureParams*)params;
	int maxIter = paramsCast->maxIter;
	Vector2 origin = paramsCast->origin;
	Vector4 window = paramsCast->window;
	JuliaGreyShader* juliaGreyShader = paramsCast->juliaGreyShader;
	int greyTextureWidth = paramsCast->greyTextureWidth;
	int greyTextureHeight = paramsCast->greyTextureHeight;

	vector<float>* pixels = juliaGreyShader->computeTexture(maxIter, origin, window, greyTextureWidth, greyTextureHeight);

	ComputeJuliaTextureReturn* res = new ComputeJuliaTextureReturn();
	res->texture = pixels;
	return res;
}


// Find a random z € |C such that Julia(c)(z) = 0 && it exist a neighborhood of z name zi such as Julia(c)(zi) > 0
Vector2 FractalUpdater::findRandomPointToZoomInJuliaInternal(Vector2 origin, vector<float>& initJuliaGreyText, bool otherThread)
{
	tuple<bool, Vector2> tuple = initRandomPointToZoom(origin, initJuliaGreyText);
	bool isFrontier = get<0>(tuple);
	Vector2 currentPoint = get<1>(tuple);

	if (!isFrontier)
	{
		//cout << "No frontier" << endl;
		return currentPoint;
	}

	int verifRow = (int)(((currentPoint.y - yMin) / (yMax - yMin)) * (float)(greyTextureHeight - 1));
	int verifCol = (int)(((currentPoint.x - xMin) / (xMax - xMin)) * (float)(greyTextureWidth - 1));
	float value = initJuliaGreyText[(greyTextureWidth * verifRow) + verifCol];
	if (value > 0.0f)
	{
		cout << "Pixel initialisation by initRandomPointToZoom function must have a value of 0, line: " << __LINE__ << " in function: " << __FUNCTION__ << " in file: " << __FILE__ << endl;
	}

	float scaleFactor = 0.5f;
	Vector2 size = Vector2(xMax - xMin, yMax - yMin);

	ComputeJuliaTextureParams params;
	params.greyTextureWidth = greyTextureWidth;
	params.greyTextureHeight = greyTextureHeight;
	params.juliaGreyShader = &juliaGreyShader;
	params.maxIter = greyMaxIter;
	params.origin = origin;

	int nbIter = refiningPointToZoomIter;
	while (nbIter > 0)
	{
		size = Vector2(size.x * scaleFactor, size.y * scaleFactor);
		float xMin = currentPoint.x - (size.x * 0.5f);
		float xMax = currentPoint.x + (size.x * 0.5f);
		float yMin = currentPoint.y - (size.x * 0.5f);
		float yMax = currentPoint.y + (size.x * 0.5f);
		Vector4 window = Vector4(xMin, xMax, yMin, yMax);

		vector<float>* texture;
		if (otherThread)
		{
			params.window = window;

			bool isCallFinish = false;
			{
				lock_guard<mutex> lock(mutexName);
				callbackArg = &params;
				callback = computeJuliaTexture;
				isAFunctionToCallInMainThread = true;
			}

			while (!isCallFinish)
			{
				this_thread::sleep_for(std::chrono::milliseconds(1));
				lock_guard<mutex> lock(mutexName);
				isCallFinish = !isAFunctionToCallInMainThread;
			}

			ComputeJuliaTextureReturn* result = (ComputeJuliaTextureReturn*)this->callbackResult;
			texture = result->texture;
			delete result;

			this_thread::sleep_for(chrono::milliseconds(millisecondToSleepAfterCallComputerSharder));
		}
		else
		{
			texture = juliaGreyShader.computeTexture(greyMaxIter, origin, window, greyTextureWidth, greyTextureHeight);
		}

		int col = greyTextureWidth / 2;
		int row = greyTextureHeight / 2;

		int radius = 1;
		int newRow, newCol;
		int textSize = greyTextureWidth * greyTextureHeight;

		float currentValue = (*texture)[(greyTextureWidth * row) + col];
		if (currentValue <= 0.0f)
		{
			bool find = false;

			if ((*texture)[(greyTextureWidth * (row + 1)) + col] > 0.0f || 
				(*texture)[(greyTextureWidth * (row - 1)) + col] > 0.0f ||
				(*texture)[(greyTextureWidth * row) + (col + 1)] > 0.0f ||
				(*texture)[(greyTextureWidth * row) + (col - 1)] > 0.0f)
			{
				newRow = row;
				newCol = col;
				find = true;
			}

			if (!find && (*texture)[(greyTextureWidth * (row + 1)) + (col + 1)] > 0.0f)
			{
				newRow = row + 1;
				newCol = col;
				find = true;
			}

			if (!find && (*texture)[(greyTextureWidth * (row + 1)) + (col - 1)] > 0.0f)
			{
				newRow = row + 1;
				newCol = col;
				find = true;
			}

			if (!find && (*texture)[(greyTextureWidth * (row - 1)) + (col + 1)] > 0.0f)
			{
				newRow = row - 1;
				newCol = col;
				find = true;
			}

			if (!find && (*texture)[(greyTextureWidth * (row - 1)) + (col - 1)] > 0.0f)
			{
				newRow = row - 1;
				newCol = col;
				find = true;
			}

			if (!find)
			{
				radius = 2;

				while (true)
				{
					int newRow1 = row + radius;
					int newRow2 = row - radius;
					for (int i = 1 - radius; i <= radius - 1; i++)
					{
						int index = (greyTextureWidth * newRow1) + (col + i);
						if (index >= 0 && index < textSize && (*texture)[index] > 0.0f)
						{
							newRow = newRow1 - 1;
							newCol = col + i;
							find = true;
							break;
						}

						index = (greyTextureWidth * newRow2) + (col + i);
						if (index >= 0 && index < textSize && (*texture)[index] > 0.0f)
						{
							newRow = newRow2 + 1;
							newCol = col + i;
							find = true;
							break;
						}
					}

					if (find)
						break;

					int newCol1 = col + radius;
					int newCol2 = col - radius;
					for (int i = 1 - radius; i <= radius - 1; i++)
					{
						int index = (greyTextureWidth * (row + i)) + newCol1;
						if ((*texture)[index] > 0.0f)
						{
							newRow = row + i;
							newCol = newCol1 - 1;
							find = true;
							break;
						}

						index = (greyTextureWidth * (row + i)) + newCol2;
						if (index >= 0 && index < textSize && (*texture)[index] > 0.0f)
						{
							newRow = row + i;
							newCol = newCol2 + 1;
							find = true;
							break;
						}
					}

					if (find)
						break;

					if ((*texture)[(greyTextureWidth * (row + radius)) + (col + radius)] > 0.0f)
					{
						newRow = row + radius;
						newCol = col + radius - 1;
						break;
					}

					if ((*texture)[(greyTextureWidth * (row + radius)) + (col - radius)] > 0.0f)
					{
						newRow = row + radius;
						newCol = col - radius + 1;
						break;
					}

					if ((*texture)[(greyTextureWidth * (row - radius)) + (col + radius)] > 0.0f)
					{
						newRow = row - radius;
						newCol = col + radius - 1;
						break;
					}

					if ((*texture)[(greyTextureWidth * (row - radius)) + (col - radius)] > 0.0f)
					{
						newRow = row - radius;
						newCol = col - radius + 1;
						break;
					}

					radius++;
				}
			}
		}
		else
		{
			while (true)
			{
				int newRow1 = row + radius;
				int newRow2 = row - radius;
				bool find = false;

				for (int i = -radius; i <= radius; i++)
				{
					int index = (greyTextureWidth * newRow1) + (col + i);
					if (index >= 0 && index < textSize && (*texture)[index] <= 0.0f)
					{
						newRow = newRow1;
						newCol = col + i;
						find = true;
						break;
					}

					index = (greyTextureWidth * newRow2) + (col + i);
					if (index >= 0 && index < textSize && (*texture)[index] <= 0.0f)
					{
						newRow = newRow2;
						newCol = col + i;
						find = true;
						break;
					}
				}

				if (find)
					break;

				int newCol1 = col + radius;
				int newCol2 = col - radius;
				for (int i = -radius; i <= radius; i++)
				{
					int index = (greyTextureWidth * (row + i)) + newCol1;
					if ((*texture)[index] <= 0.0f)
					{
						newRow = row + i;
						newCol = newCol1;
						find = true;
						break;
					}

					index = (greyTextureWidth * (row + i)) + newCol2;
					if (index >= 0 && index < textSize && (*texture)[index] <= 0.0f)
					{
						newRow = row + i;
						newCol = newCol2;
						find = true;
						break;
					}
				}

				if (find)
					break;

				radius++;
			}
		}

		float val = (*texture)[(greyTextureWidth * newRow) + newCol];
		if (val > 0.0f)
		{
			cout << "New pixel found must have a value of 0, line: " << __LINE__ << " in function: " << __FUNCTION__ << " in file: " << __FILE__ << endl;
		}

		float x = Math::lerp(xMin, xMax, (float)newCol / (float)(greyTextureWidth - 1));
		float y = Math::lerp(yMin, yMax, (float)newRow / (float)(greyTextureHeight - 1));
		currentPoint = Vector2(x, y);

		delete texture;
		nbIter--;
	}

	return currentPoint;
}

Vector2 FractalUpdater::findRandomPointToZoomInJulia(Vector2 origin, vector<float>& initJuliaGreyText)
{
	return findRandomPointToZoomInJuliaInternal(origin, initJuliaGreyText, false);
}

Vector2 FractalUpdater::findRandomPointToZoomInJuliaOtherThread(Vector2 origin, vector<float>& initJuliaGreyText)
{
	return findRandomPointToZoomInJuliaInternal(origin, initJuliaGreyText, true);
}

#pragma endregion

#pragma region generateNewTarget

void sortOrigin(vector<Vector2>& origins, vector<vector<float>*>& textures)
{
	for (int i = 3; i < origins.size(); i++)
	{
		Vector2& newPoint = origins[i];
		float minSqrDist = Math::linePointSqrDistance(origins[0], origins[1], origins[i]);
		int minIndex = 0;

		for (int j = 1; j < i; j++)
		{
			float sqrDist = Math::linePointSqrDistance(origins[j], origins[(j + 1) % i], origins[i]);
			if (sqrDist < minSqrDist)
			{
				minSqrDist = sqrDist;
				minIndex = j;
			}
		}

		for (int j = i - 1; j > minIndex; j--)
		{
			// swap j and j + 1
			Vector2 tmp = origins[j];
			origins[j] = origins[j + 1];
			origins[j + 1] = tmp;
			vector<float>* tmpText = textures[j];
			textures[j] = textures[j + 1];
			textures[j + 1] = tmpText;
		}
	}
}

void FractalUpdater::generateNewTarget(FractalUpdater::StateTarget* oldTarget)
{
	isNewTargetReady = false;

	int nbOrigines = Random::rand(minNbOrigines, maxNbOrigines - 1);
	vector<Vector2> origines;
	vector<vector<float>*> textures;
	origines.reserve(nbOrigines + 1);
	textures.reserve(nbOrigines + 1);

	if (oldTarget == nullptr)
	{
		origines.push_back(Vector2());
	}
	else
	{
		origines.push_back(oldTarget->finalOrigin());
	}
	textures.push_back(nullptr);

	for (int i = 0; i < nbOrigines; i++)
	{
		tuple<Vector2, vector<float>*> originTuple = findRandomJuliaOrigin();
		origines.push_back(get<0>(originTuple));
		textures.push_back(get<1>(originTuple));
	}

	sortOrigin(origines, textures);

	Vector2 randZoomPoint = findRandomPointToZoomInJulia(origines[origines.size() - 1], *textures[textures.size() - 1]);

	for (int i = 1; i < textures.size(); i++)
	{
		delete textures[i];
	}

	vector<Vector2> zoomPoints(3);
	zoomPoints[0] = oldTarget == nullptr ? Vector2() : oldTarget->finalZoomPoint();
	zoomPoints[2] = randZoomPoint;

	float zoomDuration = Random::rand(zoomMinDuration, zoomMaxDuration);
	float dezoomDuration = Random::rand(dezoomMinDuration, dezoomMaxDuration);
	float zoom = Random::rand(minZoom, maxZoom);
	
	newTarget = new StateTarget(origines, zoomPoints, zoomDuration, dezoomDuration, zoom);

	isNewTargetReady = true;
}

void FractalUpdater::generateNewTargetOtherThread(FractalUpdater::StateTarget* oldTarget)
{
	isNewTargetReady = false;
	Vector2 finalOrigin = oldTarget == nullptr ? Vector2() : oldTarget->finalOrigin();
	Vector2 startZoom = oldTarget == nullptr ? Vector2() : oldTarget->finalZoomPoint();
	auto lambda = [this, finalOrigin, startZoom]()
	{
		Random::setRandomSeed();
		int nbOrigines = Random::rand(minNbOrigines, maxNbOrigines - 1);
		vector<Vector2> origines;
		vector<vector<float>*> textures;
		origines.reserve(nbOrigines + 1);
		origines.push_back(finalOrigin);
		textures.reserve(nbOrigines + 1);
		textures.push_back(nullptr);

		for (int i = 0; i < nbOrigines; i++)
		{
			tuple<Vector2, vector<float>*> originTuple = findRandomJuliaOriginOtherThread();
			origines.push_back(get<0>(originTuple));
			textures.push_back(get<1>(originTuple));
		}

		sortOrigin(origines, textures);

		Vector2 randZoomPoint = findRandomPointToZoomInJuliaOtherThread(origines[origines.size() - 1], *textures[textures.size() - 1]);

		for (int i = 1; i < textures.size(); i++)
		{
			delete textures[i];
		}

		vector<Vector2> zoomPoints(3);
		zoomPoints[0] = startZoom;
		zoomPoints[2] = randZoomPoint;

		float zoomDuration = Random::rand(zoomMinDuration, zoomMaxDuration);
		float dezoomDuration = Random::rand(dezoomMinDuration, dezoomMaxDuration);
		float zoom = Random::rand(minZoom, maxZoom);

		this->newTarget = new StateTarget(origines, zoomPoints, zoomDuration, dezoomDuration, zoom);
	};

	thread thread(lambda);
	thread.detach();

	isNewTargetReady = true;
}

#pragma endregion