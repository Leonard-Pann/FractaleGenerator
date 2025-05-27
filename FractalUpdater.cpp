#pragma region include

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include "FractalUpdater.hpp"
#include "Random.hpp"
#include "Math.hpp"

using namespace std;

#pragma endregion

#pragma region palette

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
	minSize = Vector2(maxSize.x / 60000.0f, maxSize.y / 60000.0f);

	zoomMinDuration = 10.0f; 
	zoomMaxDuration = 10.0f;
	minZoom = 1.0f;
	maxZoom = 1.0f;

	dezoomMinDuration = 4.0f;
	dezoomMaxDuration = 6.0f;

	// For findJuliaOrigin method
	greyTextureWidth = 1920;
	greyTextureHeight = 1080;
	juliaGreyShader.load();
	greyMaxIter = 1000;
	juliaOriginThreshold = 0.0018f;
	textureVariationShader.load();

	// for findRandomPointToZoomInJulia
	float refineZoomIterOn1080pScreen = 10.0f;
	refiningPointToZoomIter = Math::ceil(((float)screenWidth / 1920.0f) * refineZoomIterOn1080pScreen);

	// Change Fractal
	changeFractalDuration = 7.0f;
	minNbOrigines = 4; // min 1
	maxNbOrigines = 4;

	Vector3 colorIn(0.0f, 0.0f, 0.0f);
	int maxIter = 1000;
	params = FractaleParam(Vector2(), xMin, xMax, yMin, yMax, colorIn, colorPallets[0], maxIter);

	generateNewTarget(nullptr);
	target = newTarget;
	generateNewTargetOtherThread(target);

	params.origin = target->getOrigin(0.0f);

	startZoomPoint = target->zoomPoint;
	params.xMin = startZoomPoint.x - (maxSize.x * 0.5f);
	params.xMax = startZoomPoint.x + (maxSize.x * 0.5f);
	params.yMin = startZoomPoint.y - (maxSize.y * 0.5f);
	params.yMax = startZoomPoint.y + (maxSize.y * 0.5f);

	state = TransitionState::changeFractal;
	changeFractalTimer = 0.0f;
	zoomTime = 0.0f;
}

#pragma endregion

const FractaleParam& FractalUpdater::getFractaleParam() const
{
	return params;
}

Vector2 FractalUpdater::randomPoint() const
{
	return Vector2(Random::rand(xMin, xMax), Random::rand(yMin, yMax));
}

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

	float endSizeX = Math::lerp(maxSize.x, minSize.x, target->zoom);
	float sizeX = params.xMax - params.xMin;;
	float growthFactor = pow(endSizeX / sizeX, dt / (target->zoomDuration - zoomTime));
	sizeX *= growthFactor;

	float endSizeY = Math::lerp(maxSize.y, minSize.y, target->zoom);
	float sizeY = params.yMax - params.yMin;;
	growthFactor = pow(endSizeY / sizeY, dt / (target->zoomDuration - zoomTime));
	sizeY *= growthFactor;

	params.xMin = target->zoomPoint.x - (sizeX * 0.5f);
	params.xMax = target->zoomPoint.x + (sizeX * 0.5f);
	params.yMin = target->zoomPoint.y - (sizeY * 0.5f);
	params.yMax = target->zoomPoint.y + (sizeY * 0.5f);

	if (zoomTime >= target->zoomDuration)
	{
		zoomTime = 0.0f;
		state = TransitionState::dezooming;
	}
}

void FractalUpdater::dezoom(float dt)
{
	zoomTime += dt;

	float sizeX = params.xMax - params.xMin;;
	float growthFactor = pow(maxSize.x / sizeX, dt / (target->dezoomDuration - zoomTime));
	sizeX *= growthFactor;

	float sizeY = params.yMax - params.yMin;;
	growthFactor = pow(maxSize.y / sizeY, dt / (target->dezoomDuration - zoomTime));
	sizeY *= growthFactor;

	params.xMin = target->zoomPoint.x - (sizeX * 0.5f);
	params.xMax = target->zoomPoint.x + (sizeX * 0.5f);
	params.yMin = target->zoomPoint.y - (sizeY * 0.5f);
	params.yMax = target->zoomPoint.y + (sizeY * 0.5f);

	if (zoomTime >= target->dezoomDuration)
	{
		zoomTime = 0.0f;
		startZoomPoint = target->zoomPoint;

		while (!isNewTargetReady)
		{
			this_thread::sleep_for(chrono::milliseconds(1));
		}

		delete target;
		target = newTarget;

		generateNewTargetOtherThread(target);

		changeFractalTimer = 0.0f;
		state = TransitionState::changeFractal;
	}
}

void FractalUpdater::changeFractal(float dt)
{
	changeFractalTimer += dt;
	float lerpTime = Math::clamp01(changeFractalTimer / changeFractalDuration);

	Vector2 newOrigin = target->getOrigin(lerpTime);

	float x = Math::lerp(startZoomPoint.x, target->zoomPoint.x, lerpTime);
	float y = Math::lerp(startZoomPoint.y, target->zoomPoint.y, lerpTime);
	Vector2 newZoomPoint = Vector2(x, y);

	params.origin = newOrigin;
	params.xMin = newZoomPoint.x - (maxSize.x * 0.5f);
	params.xMax = newZoomPoint.x + (maxSize.x * 0.5f);
	params.yMin = newZoomPoint.y - (maxSize.y * 0.5f);
	params.yMax = newZoomPoint.y + (maxSize.y * 0.5f);

	if (changeFractalTimer >= changeFractalDuration)
	{
		state = TransitionState::zooming;
		zoomTime = 0.0;
	}
}

#pragma endregion

#pragma region getJuliaTotalGreyVariation

tuple<float, vector<float>*> FractalUpdater::getJuliaTotalGreyVariation(int maxIter, Vector2 origin, const Vector4& window)
{
	vector<float>* pixels = juliaGreyShader.computeTexture(maxIter, origin, window, greyTextureWidth, greyTextureHeight);

	// The comment lines below is the CPU version for computing the mean variation
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

	float meanGPU = textureVariationShader.computeMeanTextureVariation(*pixels, greyTextureWidth, greyTextureHeight);

	return make_tuple(meanGPU, pixels);
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
	JuliaGreyComputeShader* juliaGreyShader;
	TextureVariationShader* textureVariationShader;
	int greyTextureWidth;
	int greyTextureHeight;
};

struct JuliaTotalGreyVariationOtherThreadReturn
{
	float mean;
	vector<float>* texture;
};

void* getJuliaTotalGreyVariationOtherThread(void* params)
{
	JuliaTotalGreyVariationOtherThreadParams* paramsCast = (JuliaTotalGreyVariationOtherThreadParams*)params;
	int maxIter = paramsCast->maxIter;
	Vector2 origin = paramsCast->origin;
	Vector4 window = paramsCast->window;
	JuliaGreyComputeShader* juliaGreyShader = paramsCast->juliaGreyShader;
	TextureVariationShader* textureVariationShader = paramsCast->textureVariationShader;
	int greyTextureWidth = paramsCast->greyTextureWidth;
	int greyTextureHeight = paramsCast->greyTextureHeight;

	vector<float>* pixels = juliaGreyShader->computeTexture(maxIter, origin, window, greyTextureWidth, greyTextureHeight);
	float meanGPU = textureVariationShader->computeMeanTextureVariation(*pixels, greyTextureWidth, greyTextureHeight);

	JuliaTotalGreyVariationOtherThreadReturn* res = new JuliaTotalGreyVariationOtherThreadReturn();
	res->mean = meanGPU;
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
	params.textureVariationShader = &textureVariationShader;
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
		mean = result->mean;
		juliaGreyTexture = result->texture;

		delete result;

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
	JuliaGreyComputeShader* juliaGreyShader;
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
	JuliaGreyComputeShader* juliaGreyShader = paramsCast->juliaGreyShader;
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

#pragma region findRandomOriginAndZoomPoint

tuple<Vector2, Vector2> FractalUpdater::findRandomOriginAndZoomPoint()
{
	tuple<Vector2, vector<float>*> tuple = findRandomJuliaOrigin();
	Vector2 origin = get<0>(tuple);
	vector<float>* juliaGreyText = get<1>(tuple);

	Vector2 pointToZoom = findRandomPointToZoomInJulia(origin, *juliaGreyText);

	delete juliaGreyText;

	return make_tuple(origin, pointToZoom);
}


tuple<Vector2, Vector2> FractalUpdater::findRandomOriginAndZoomPointOtherThread()
{
	tuple<Vector2, vector<float>*> tuple = findRandomJuliaOriginOtherThread();
	Vector2 origin = get<0>(tuple);
	vector<float>* juliaGreyText = get<1>(tuple);

	Vector2 pointToZoom = findRandomPointToZoomInJuliaOtherThread(origin, *juliaGreyText);

	delete juliaGreyText;

	return make_tuple(origin, pointToZoom);
}

#pragma endregion

#pragma region generateNewTarget

void FractalUpdater::generateNewTarget(FractalUpdater::StateTarget* oldTarget)
{
	isNewTargetReady = false;

	auto start = std::chrono::high_resolution_clock::now();

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

	Vector2 lastOrigin = origines[0];
	for (uint32_t i = 1; i < origines.size() - 1; i++)
	{
		int minIndex = i;
		float minDist = Vector2::distance(lastOrigin, origines[i]);
		for (uint32_t j = i + 1; j < origines.size(); j++)
		{
			float dist = Vector2::distance(lastOrigin, origines[j]);
			if (dist < minDist)
			{
				minDist = dist;
				minIndex = j;
			}
		}

		if (minIndex != i)
		{
			Vector2 tmpVector = origines[i];
			origines[i] = origines[minIndex];
			origines[minIndex] = tmpVector;

			vector<float>* tmpTexture = textures[i];
			textures[i] = textures[minIndex];
			textures[minIndex] = tmpTexture;
		}

		lastOrigin = origines[i];
	}

	Vector2 randZoomPoint = findRandomPointToZoomInJulia(origines[origines.size() - 1], *textures[textures.size() - 1]);

	for (int i = 1; i < textures.size(); i++)
	{
		delete textures[i];
	}

	float zoomDuration = Random::rand(zoomMinDuration, zoomMaxDuration);
	float dezoomDuration = Random::rand(dezoomMinDuration, dezoomMaxDuration);
	float zoom = Random::rand(minZoom, maxZoom);
	
	newTarget = new StateTarget(origines, randZoomPoint, zoomDuration, dezoomDuration, zoom);

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "lastNewPoint{" << origines[origines.size() - 1].x << ", " << origines[origines.size() - 1].y << "} " << "Elapsed time: " << elapsed.count() << " seconds" << endl;

	isNewTargetReady = true;
}

void FractalUpdater::generateNewTargetOtherThread(FractalUpdater::StateTarget* oldTarget)
{
	isNewTargetReady = false;
	Vector2 finalOrigin = oldTarget == nullptr ? Vector2() : oldTarget->finalOrigin();
	auto lambda = [this, finalOrigin]()
	{
		auto start = std::chrono::high_resolution_clock::now();
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

		Vector2 lastOrigin = origines[0];
		for (uint32_t i = 1; i < origines.size() - 1; i++)
		{
			int minIndex = i;
			float minDist = Vector2::distance(lastOrigin, origines[i]);
			for (uint32_t j = i + 1; j < origines.size(); j++)
			{
				float dist = Vector2::distance(lastOrigin, origines[j]);
				if (dist < minDist)
				{
					minDist = dist;
					minIndex = j;
				}
			}

			if (minIndex != i)
			{
				Vector2 tmpVector = origines[i];
				origines[i] = origines[minIndex];
				origines[minIndex] = tmpVector;

				vector<float>* tmpTexture = textures[i];
				textures[i] = textures[minIndex];
				textures[minIndex] = tmpTexture;
			}

			lastOrigin = origines[i];
		}

		Vector2 randZoomPoint = findRandomPointToZoomInJuliaOtherThread(origines[origines.size() - 1], *textures[textures.size() - 1]);

		for (int i = 1; i < textures.size(); i++)
		{
			delete textures[i];
		}

		float zoomDuration = Random::rand(zoomMinDuration, zoomMaxDuration);
		float dezoomDuration = Random::rand(dezoomMinDuration, dezoomMaxDuration);
		float zoom = Random::rand(minZoom, maxZoom);

		this->newTarget = new StateTarget(origines, randZoomPoint, zoomDuration, dezoomDuration, zoom);

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = end - start;
		std::cout << "lastNewPoint{" << origines[origines.size() - 1].x << ", " << origines[origines.size() - 1].y << "} " << "Elapsed time: " << elapsed.count() << " seconds" << endl;

	};

	std::thread thread(lambda);
	thread.detach();

	isNewTargetReady = true;
}

#pragma endregion