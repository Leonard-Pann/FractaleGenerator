#ifndef JULIA_GREY_COMPUTE_SHADER_HPP
#define JULIA_GREY_COMPUTE_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include "ComputeShader.hpp"

class JuliaGreyComputeShader : public ComputeShader
{
private:
	GLuint ssbo;

public:
	JuliaGreyComputeShader();
	void load() override;
	std::vector<float>* computeTexture(int maxIter, Vector2 origin, const Vector4& window, int textWidth, int textHeight);
	~JuliaGreyComputeShader() override;
};

#endif