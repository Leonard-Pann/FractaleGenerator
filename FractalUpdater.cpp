#include "FractalUpdater.hpp"
#include "Random.hpp"

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

FractalUpdater::FractalUpdater(JuliaFractal* fractal)
{
	this->fractal = fractal;
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

	Vector3 colorIn = Vector3(0.0f, 0.0f, 0.0f);
	params = FractaleParam(random_point(), xMin, xMax, yMin, yMax, colorIn, colorPallets[0], 1000);

	// For find_julia_origin method
	grayTextureWidth = 1920;
	grayTextureHeight = 1080;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grayTextureWidth, grayTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Create and attach the frameBuffer
	//glGenFramebuffers(1, &fbo);
	//glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	//	cerr << "Erreur FBO" << endl;
}

void FractalUpdater::update(float dt)
{
	/* New algo :
	* Search a random c € |C such that the absolute difference of gray scale julia fractale is >= threshold (use gradient descent) *1
	* Find a random z € |C such that Julia(c)(z) <= threshold && meanSum(Julia(c)(zi)) >= threshold2 where zi neighborhood
	* Make a random zoom into z
	* Search another random c like *1
	* Dezoom a go to the new C using bezier curve
	*/
	findJuliaOrigin();
}

// return a random c € |C such that the absolute difference of gray scale julia fractale is >= threshold(use gradient descent)
Vector2 FractalUpdater::findJuliaOrigin()
{



	return Vector2();
}
