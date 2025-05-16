#ifndef __FRACTAL_UPDATER_HPP
#define __FRACTAL_UPDATER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "FractaleParam.hpp"
#include "Vector.hpp"
#include "HermiteSpline.hpp"
#include "JuliaFractal.hpp"

class FractalUpdater
{
private:

	static std::vector<std::vector<Vector3>> colorPallets;

	// fractal update param
	FractaleParam params;
	float xMin, xMax, yMin, yMax;
	Vector2 minSize;
	float juliaOriginThreshold;
	float juliaCentroidPointBlackThreshold, juliaCentroidPointNeighborhoodThreshold;
	float centroidNeighborhoodRadius; // in %age of screen width

	float zoomMinDuration, zoomMaxDuration;
	float minZoom, maxZoom; // 0 => display julia of size (xMax - xMin, yMax - yMin), 1 => display of size minSize

	float dezoomMinDuration, dezoomMaxDuration;

	//internal members
	int grayTextureWidth, grayTextureHeight;
	GLuint fbo, tex, juliaGrayShader;
	JuliaFractal* fractal;

	Vector2 currentJuliaOrigin;
	Vector2 random_point();
	Vector2 findJuliaOrigin();

public:
	FractalUpdater(JuliaFractal* fractal);
	FractaleParam& getFractaleParam();
	void init();
	void update(float dt);
};

#endif
