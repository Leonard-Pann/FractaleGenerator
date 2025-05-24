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

	enum class TransitionState
	{
		zooming = 0, dezooming = 1, changeFractal = 2
	};

	class StateTarget
	{
	public:
		Vector2 origin; // seed / c component of the julia fractal
		Vector2 zoomPoint;
		float zoomDuration;
		float dezoomDuration;
		float zoom;

		StateTarget() {};
		StateTarget(const StateTarget& state) : origin(state.origin), zoomPoint(state.zoomPoint), zoomDuration(state.zoomDuration), dezoomDuration(state.dezoomDuration), zoom(state.zoom) {}
		StateTarget(Vector2 origin, Vector2 zoomPoint, float zoomDuration, float dezoomDuration, float zoom) :
			origin(origin), zoomPoint(zoomPoint), zoomDuration(zoomDuration), dezoomDuration(dezoomDuration), zoom(zoom)
		{

		}
	};


	// fractal update param
	FractaleParam params;
	float xMin, xMax, yMin, yMax;
	Vector2 minSize, maxSize;
	float juliaOriginThreshold;
	float juliaCentroidPointBlackThreshold, juliaCentroidPointNeighborhoodThreshold;
	float centroidNeighborhoodRadius; // in %age of screen width

	float zoomMinDuration, zoomMaxDuration;
	float minZoom, maxZoom; // 0 => display julia of size maxSize, 1 => display of size minSize
	float dezoomMinDuration, dezoomMaxDuration;

	//internal members
	int greyTextureWidth, greyTextureHeight, greyMaxIter;
	JuliaGreyComputeShader juliaGreyShader;
	TextureVariationShader textureVariationShader;

	StateTarget target;
	TransitionState state;
	// zoom
	float zoomTime;
	// dezoom
	// changeFractal
	float changeFractalTimer;
	StateTarget newTarget;

	Vector2 randomPoint();
	std::tuple<Vector2, std::vector<float>*> findRandomJuliaOrigin();
	std::tuple<float, std::vector<float>*> getJuliaTotalGreyVariation(int maxIter, const Vector2 origin, const Vector4& window);
	Vector2 findRandomPointToZoomInJulia(Vector2 origin, std::vector<float>& juliaGreyText);
	std::tuple<Vector2, Vector2> findRandomOriginAndZoomPoint();
	StateTarget generateNewTarget();
	void zoom(float dt);
	void dezoom(float dt);
	void changeFractal(float dt);

public:
	FractalUpdater();
	FractalUpdater(int screenWidth, int screenHeight);
	FractaleParam& getFractaleParam();
	void update(float dt);
};

#endif
