#ifndef __FRACTAL_UPDATER_HPP
#define __FRACTAL_UPDATER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <tuple>
#include <functional>
#include <mutex>
#include "FractaleParam.hpp"
#include "Vector.hpp"
#include "JuliaGreyComputeShader.hpp"
#include "TextureVariationShader.hpp"
#include "HermiteSpline.hpp"

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
	private:
		HermiteSpline spline; // seed / c component of the julia fractal

	public:
		Vector2 zoomPoint;
		float zoomDuration;
		float dezoomDuration;
		float zoom;

		StateTarget() : zoomDuration(0.0f), dezoomDuration(0.0f), zoom(0.0f) {};
		StateTarget(const StateTarget& state) : spline(state.spline), zoomPoint(state.zoomPoint), zoomDuration(state.zoomDuration), dezoomDuration(state.dezoomDuration), zoom(state.zoom) {}
		StateTarget(std::vector<Vector2> origines, Vector2 zoomPoint, float zoomDuration, float dezoomDuration, float zoom) :
			spline(origines), zoomPoint(zoomPoint), zoomDuration(zoomDuration), dezoomDuration(dezoomDuration), zoom(zoom)
		{

		}

		Vector2 finalOrigin() const
		{
			return spline.getEnd();
		}

		Vector2 getOrigin(float t) const
		{
			return spline.EvaluateDistance(t);
		}
	};

	// fractal update param
	FractaleParam params;
	float xMin, xMax, yMin, yMax;
	Vector2 minSize, maxSize;
	float juliaOriginThreshold;

	float zoomMinDuration, zoomMaxDuration;
	float minZoom, maxZoom; // 0 => display julia of size maxSize, 1 => display of size minSize
	float dezoomMinDuration, dezoomMaxDuration;

	//internal members
	int greyTextureWidth, greyTextureHeight, greyMaxIter;
	int refiningPointToZoomIter;
	JuliaGreyComputeShader juliaGreyShader;
	TextureVariationShader textureVariationShader;

	StateTarget* target;
	StateTarget* newTarget;
	TransitionState state;
	// zoom
	float zoomTime;
	// dezoom
	// changeFractal
	Vector2 startZoomPoint;
	float changeFractalTimer;
	float changeFractalDuration;
	int minNbOrigines, maxNbOrigines;
	bool isNewTargetReady;

	//multi thread
	bool isAFunctionToCallInMainThread;
	std::mutex mutexName;
	std::function<void*(void*)> callback;
	void* callbackArg;
	void* callbackResult;

	Vector2 randomPoint() const;
	std::tuple<Vector2, std::vector<float>*> findRandomJuliaOrigin();
	std::tuple<Vector2, std::vector<float>*> findRandomJuliaOriginOtherThread();
	std::tuple<float, std::vector<float>*> getJuliaTotalGreyVariation(int maxIter, const Vector2 origin, const Vector4& window);
	std::tuple<bool, Vector2> initRandomPointToZoom(Vector2 origin, std::vector<float>& initJuliaGreyText);
	Vector2 findRandomPointToZoomInJuliaInternal(Vector2 origin, std::vector<float>& juliaGreyText, bool otherThread);
	Vector2 findRandomPointToZoomInJulia(Vector2 origin, std::vector<float>& juliaGreyText);
	Vector2 findRandomPointToZoomInJuliaOtherThread(Vector2 origin, std::vector<float>& juliaGreyText);
	std::tuple<Vector2, Vector2> findRandomOriginAndZoomPoint();
	std::tuple<Vector2, Vector2> findRandomOriginAndZoomPointOtherThread();
	void generateNewTarget(StateTarget* oldTarget);
	void generateNewTargetOtherThread(StateTarget* oldTarget);

	void zoom(float dt);
	void dezoom(float dt);
	void changeFractal(float dt);

public:
	FractalUpdater();
	FractalUpdater(int screenWidth, int screenHeight);
	const FractaleParam& getFractaleParam() const;
	void update(float dt);
};

#endif
