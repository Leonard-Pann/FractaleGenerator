#ifndef FRACTAL_UPDATER_HPP
#define FRACTAL_UPDATER_HPP

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <tuple>
#include <functional>
#include <mutex>
#include "FractaleParam.hpp"
#include "Vector.hpp"
#include "shader/JuliaGreyShader.hpp"
#include "bezierCurve/CatmulRomSpline.hpp"
#include "Number.hpp"

class FractalUpdater
{
private:

	static std::vector<std::vector<Vector3>> colorPallets;

	class StateTarget
	{
	private:
		HermiteSpline<Vector2> spline; // seed / c component of the julia fractal
		HermiteSpline<Vector2> zoomSpline;

	public:
		float zoomDuration;
		float dezoomDuration;
		float zoom;

		StateTarget() : zoomDuration(0.0f), dezoomDuration(0.0f), zoom(0.0f) {};
		StateTarget(const StateTarget& state) : spline(state.spline), zoomSpline(state.zoomSpline), zoomDuration(state.zoomDuration), dezoomDuration(state.dezoomDuration), zoom(state.zoom) {}
		StateTarget(std::vector<Vector2> origines, std::vector<Vector2> zoomPoints, float zoomDuration, float dezoomDuration, float zoom) :
			spline(origines), zoomSpline(zoomPoints), zoomDuration(zoomDuration), dezoomDuration(dezoomDuration), zoom(zoom)
		{

		}

		Vector2 finalZoomPoint() const
		{
			return zoomSpline.getEnd();
		}

		Vector2 getZoomPoint(float x) const
		{
			return zoomSpline.evaluateDistance(x);
		}

		Vector2 finalOrigin() const
		{
			return spline.getEnd();
		}

		Vector2 getOrigin(float x) const
		{
			return spline.evaluateDistance(x);
		}
	};

	// fractal update param
	FractaleParam params;
	float xMin, xMax, yMin, yMax;
	Vector2 minSize, maxSize;
	float juliaOriginThreshold;
	int zoomJuliaOriginCostThreshold;
	bool useZoomJuliaOriginCostThreshold;

	float zoomMinDuration, zoomMaxDuration;
	float minZoom, maxZoom; // 0 => display julia of size maxSize, 1 => display of size minSize
	float dezoomMinDuration, dezoomMaxDuration;

	//internal members
	int greyTextureWidth, greyTextureHeight, greyMaxIter;
	int refiningPointToZoomIter;
	JuliaGreyComputeShader juliaGreyShader;

	StateTarget* target;
	StateTarget* newTarget;

	// zoom
	float zoomTime;
	bool isZooming;
	float zoomStartOffset;
	float zoomTweenIntensity;

	// dezoom
	bool isDezooming;
	StateTarget* dezoomTarget;
	std::vector<CatmulRomSpline<Vector3>>* dezoomColorsSplines;

	// changeFractal
	Vector2 changeFractalSize;
	bool isChangingFractal;
	float changeFractalTimer;
	float changeFractalDuration;
	float changeFractalStartOffset;
	float changeFractaleOriginTweenIntensity;
	int minNbOrigines, maxNbOrigines;
	bool isNewTargetReady;
	float avoidingBlackZoneOffset;
	const std::vector<Vector2> blackZone = 
	{
		{ 0.0f, 0.63f },
		{ 0.1f, 0.59f },
		{ 0.2f, 0.53f },
		{ 0.3f, 0.43f },
		{ 0.3f, -0.43f },
		{ 0.2f, -0.53f },
		{ 0.1f, -0.59f },
		{ 0.0f, -0.63f },
		{ -0.1f, -0.64f },
		{ -0.2f, -0.64f },
		{ -0.3f, -0.62f },
		{ -0.4f, -0.57f },
		{ -0.5f, -0.50f },
		{ -0.6f, -0.40f },
		{ -0.7f, -0.22f },
		{ -0.7f, 0.22f },
		{ -0.6f, 0.40f },
		{ -0.5f, 0.50f },
		{ -0.4f, 0.57f },
		{ -0.3f, 0.62f },
		{ -0.2f, 0.64f },
		{ -0.1f, 0.64f }
	};

	//multi thread
	std::mutex mutexName;

	//colors
	int nbColorsInPalet;
	int nbColorsInAnUpdateCircle;
	std::vector<CatmulRomSpline<Vector3>>* colorsSplines = nullptr;
	float colorTimer;
	float minColorRange, maxColorRange;
	CatmulRomSpline<Number> colorRangeSpline;
	float colorRangeDuration;
	int nbColorRangeInSpline;
	float colorRangeTimer;

	Vector2 randomPoint() const;
	std::tuple<Vector2, std::vector<float>*> findRandomJuliaOrigin();
	std::tuple<Vector2, std::vector<float>*> findRandomJuliaOriginOtherThread();
	std::tuple<float, std::vector<float>*> getJuliaTotalGreyVariation(int maxIter, const Vector2 origin, const Vector4& window);
	std::tuple<bool, int, int> initRandomPointToZoom(const Vector2& origin, const std::vector<float>& initJuliaGreyText);
	std::tuple<Vector2, Vector2> findRandomOriginAndZoomPointInternal(bool otherThread);
	std::tuple<Vector2, Vector2> findRandomOriginAndZoomPoint();
	std::tuple<Vector2, Vector2> findRandomOriginAndZoomPointOtherThread();
	void generateFirstTarget();
	void generateNewTargetOtherThread(StateTarget* oldTarget);
	std::vector<Vector3>* getCurrentColorPallet(std::vector<CatmulRomSpline<Vector3>>* pallet);
	std::vector<CatmulRomSpline<Vector3>>* generateNewPallets();
	void generateColorRangeSpline();

	void zoom(float dt);
	void dezoom(float dt);
	void changeFractal(float dt);
	void completeOriginToAvoidBlackZone(std::vector<Vector2>& origins);

public:
	FractalUpdater();
	FractalUpdater(int screenWidth, int screenHeight);
	FractaleParam& getFractaleParam();
#ifdef _DEBUG
	int64_t getJuliaFractalCost(Vector2 origin);
#endif
	void update(float dt);
};

#endif
