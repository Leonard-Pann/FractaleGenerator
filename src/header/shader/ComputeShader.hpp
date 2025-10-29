#ifndef COMPUTE_SHADER_HPP
#define COMPUTE_SHADER_HPP

#include <GLES3/gl32.h>
#include <EGL/egl.h>
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