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

FractalUpdater::FractalUpdater() : juliaGreyShader("Shaders/juliaGreyCompute.shader")
{
	juliaGreyShader.load();
	//juliaGreyShader.add_uniform("seed");
	//juliaGreyShader.add_uniform("window");
	//juliaGreyShader.add_uniform("maxIter");
}

FractaleParam& FractalUpdater::getFractaleParam()
{
	return params;
}

Vector2 FractalUpdater::random_point()
{
	return Vector2(Random::rand(xMin, xMax), Random::rand(yMin, yMax));
}

void FractalUpdater::init()
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
	params = FractaleParam(random_point(), xMin, xMax, yMin, yMax, colorIn, colorPallets[0], 1000);

	// For find_julia_origin method
	greyTextureWidth = 1920;
	greyTextureHeight = 1080;
	juliaGreyShader.setTextureSize(greyTextureWidth, greyTextureHeight);
	greyMaxIter = 500;

	timer = 2.0f;
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
	if(timer > 2.0f)
	{
		params.origin = findJuliaOrigin(origin);
		timer -= 2.0f;
	}
	timer += dt;
}

float FractalUpdater::getJuliaTotalGreyVariation(int maxIter, Vector2 origin, const Vector4& window)
{
	vector<float>& pixels = *juliaGreyShader.computeTexture(maxIter, origin, window);

	int widthEnd = greyTextureWidth - 1;
	int heightEnd = greyTextureHeight - 1;
	int currentIndex;
	float currentGreyscale;
	double totalSum = 0.0;
	for (int i = 1; i < widthEnd; i++)
	{
		for (int j = 1; j < heightEnd; j++)
		{
			if ((greyTextureWidth * (i + 1)) + (j + 1) > pixels.size())
			{
				int a = 12;
			}

			currentIndex = (greyTextureWidth * j) + i;
			currentGreyscale = pixels[currentIndex];
			int sum = abs(currentGreyscale - pixels[(greyTextureWidth * (j - 1)) + (i - 1)]);
			sum += abs(currentGreyscale - pixels[(greyTextureWidth * (j - 1)) + i]);
			sum += abs(currentGreyscale - pixels[(greyTextureWidth * (j - 1)) + (i + 1)]);
			sum += abs(currentGreyscale - pixels[(greyTextureWidth * j) + (i - 1)]);
			sum += abs(currentGreyscale - pixels[(greyTextureWidth * j) + (i + 1)]);
			sum += abs(currentGreyscale - pixels[(greyTextureWidth * (j + 1)) + (i - 1)]);
			sum += abs(currentGreyscale - pixels[(greyTextureWidth * (j + 1)) + i]);
			sum += abs(currentGreyscale - pixels[(greyTextureWidth * (j + 1)) + (i + 1)]);
			totalSum += (double)sum / 8.0;
		}
	}

	float mean = totalSum / (double)(greyTextureWidth * greyTextureHeight);

	delete &pixels;
	return mean;
}

// return a random c € |C such that the absolute difference of gray scale julia fractale is >= threshold(use gradient descent)
Vector2 FractalUpdater::findJuliaOrigin(Vector2 origin)
{
	Vector4 window(xMin, xMax, yMin, yMax);

	auto start = std::chrono::high_resolution_clock::now();
	float mean = getJuliaTotalGreyVariation(greyMaxIter, origin, window);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;

	cout << "mean : " << mean << " and takes " << elapsed.count() << " seconds" << endl;
	return origin;
}
