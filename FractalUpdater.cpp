#include <algorithm>
#include "FractalUpdater.hpp"
#include "Random.hpp"
#include <chrono> //tmp

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

FractalUpdater::FractalUpdater() : juliaGreyShader()
{
	//params
	xMin = -1.7f;
	xMax = 1.7f;
	yMin = -1.7f;
	yMax = 1.7f;

	minSize = Vector2(4.0f / 5000.0f, 4.0f / 5000.0f);

	juliaOriginThreshold = 0.05f;
	juliaCentroidPointBlackThreshold = 0.05f;
	juliaCentroidPointNeighborhoodThreshold = 0.15f;
	centroidNeighborhoodRadius = 0.015f;

	zoomMinDuration = 3.0f;
	zoomMaxDuration = 5.0f;
	minZoom = 0.2f;
	maxZoom = 1.0f;

	dezoomMinDuration = 3.0f;
	dezoomMaxDuration = 5.0f;

	Vector3 colorIn(0.0f, 0.0f, 0.0f);
	params = FractaleParam(randomPoint(), xMin, xMax, yMin, yMax, colorIn, colorPallets[0], 1000);
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


	timer = 2.0f;
}

FractaleParam& FractalUpdater::getFractaleParam()
{
	return params;
}

Vector2 FractalUpdater::randomPoint()
{
	return Vector2(Random::rand(xMin, xMax), Random::rand(yMin, yMax));
}

void FractalUpdater::update(float dt, Vector2 origin)
{
	/* New algo :
	* Search a random c € |C such that the absolute difference of gray scale julia fractale is >= threshold (use gradient descent) *1
	* Find a random z € |C such that Julia(c)(z) <= threshold && meanSum(Julia(c)(zi)) >= threshold2 where zi neighborhood
	* Make a random zoom into z
	* Search another random c like *1
	* Dezoom a go to the new C using bezier curve
	*/

	Vector2 c = Vector2(0.373287f, 0.349525f);

	if(timer > 2.0f)
	{
		params.origin = findRandomPointToZoomInJulia();
		timer -= 2.0f;
	}
	timer += dt;

	params.origin = c;//tmp
}

//tuple<float, vector<float>*>  FractalUpdater::getJuliaTotalGreyVariation(int maxIter, Vector2 origin, const Vector4& window)
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
Vector2 FractalUpdater::findRandomPointToZoomInJulia()
{
	tuple<Vector2, vector<float>*> tuple = findRandomJuliaOrigin();
	Vector2 origin = get<0>(tuple);
	vector<float>& juliaGreyText = *get<1>(tuple);

	int randomRow(0);
	int randomCol(0);
	float meanNeighborhoodSum(0.0f);
	int neigPixelSize = max((int)(centroidNeighborhoodRadius * (xMax - xMin)), 1);

	do
	{
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



	delete& juliaGreyText;
	return Vector2();
}
