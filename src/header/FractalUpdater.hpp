#ifndef FRACTAL_UPDATER_HPP
#define FRACTAL_UPDATER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <tuple>
#include <functional>
#include <mutex>
#include "FractaleParam.hpp"
#include "Vector.hpp"
#include "shader/JuliaGreyComputeShader.hpp"
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

		Vector2 getZoomPoint(float t) const
		{
			return zoomSpline.evaluate(t);
		}

		Vector2 finalOrigin() const
		{
			return spline.getEnd();
		}

		Vector2 getOrigin(float t) const
		{
			return spline.evaluate(t);
		}
	};

	// fractal update param
	FractaleParam params;
	float xMin, xMax, yMin, yMax;
	Vector2 minSize, maxSize;
	float juliaOriginThreshold;
	int juliaOriginCostThreshold;

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
	int minNbOrigines, maxNbOrigines;
	bool isNewTargetReady;

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
	std::tuple<bool, int, int> initRandomPointToZoom(Vector2 origin, std::vector<float>& initJuliaGreyText);
	Vector2 findRandomPointToZoomInJuliaInternal(Vector2 origin, std::vector<float>& juliaGreyText, bool otherThread);
	Vector2 findRandomPointToZoomInJulia(Vector2 origin, std::vector<float>& juliaGreyText);
	Vector2 findRandomPointToZoomInJuliaOtherThread(Vector2 origin, std::vector<float>& juliaGreyText);
	std::tuple<Vector2, Vector2> findRandomOriginAndZoomPoint();
	std::tuple<Vector2, Vector2> findRandomOriginAndZoomPointOtherThread();
	void generateNewTarget(StateTarget* oldTarget);
	void generateNewTargetOtherThread(StateTarget* oldTarget);
	std::vector<Vector3>* getCurrentColorPallet(std::vector<CatmulRomSpline<Vector3>>* pallet);
	std::vector<CatmulRomSpline<Vector3>>* generateNewPallets();
	void generateColorRangeSpline();

	void zoom(float dt);
	void dezoom(float dt);
	void changeFractal(float dt);

public:
	FractalUpdater();
	FractalUpdater(int screenWidth, int screenHeight);
	FractaleParam& getFractaleParam();
	//DEBUG
	int64_t getJuliaFractalCost(Vector2 origin);
	void update(float dt);
};

#endif
