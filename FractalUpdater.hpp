#ifndef __FRACTAL_UPDATER_HPP
#define __FRACTAL_UPDATER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <tuple>
#include "FractaleParam.hpp"
#include "Vector.hpp"
#include "JuliaGreyComputeShader.hpp"
#include "TextureVariationShader.hpp"

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
	int greyTextureWidth, greyTextureHeight, greyMaxIter;
	JuliaGreyComputeShader juliaGreyShader;
	TextureVariationShader textureVariationShader;

	Vector2 randomPoint();
	std::tuple<Vector2, std::vector<float>*> findRandomJuliaOrigin();
	std::tuple<float, std::vector<float>*> getJuliaTotalGreyVariation(int maxIter, const Vector2 origin, const Vector4& window);
	Vector2 findRandomPointToZoomInJulia();

public:
	FractalUpdater();
	FractaleParam& getFractaleParam();
	void update(float dt, Vector2 origin);

	//tmp
	float timer;
};

#endif
