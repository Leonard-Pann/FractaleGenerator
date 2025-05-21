#ifndef COMPUTE_SHADER_HPP
#define COMPUTE_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include "Shader.hpp"

class ComputeShader : public Shader
{
private:
	void compile();

public:

	ComputeShader(const std::string& path);
	void load() override;
	~ComputeShader() override;
};

#endif