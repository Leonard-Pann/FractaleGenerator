#ifndef JULIA_GREY_COMPUTE_SHADER_HPP
#define JULIA_GREY_COMPUTE_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <map>
#include "Vector.hpp"

class JuliaGreyComputeShader
{
private:
	std::string path;
	GLuint ssbo;
	std::map<std::string, GLint> uniformIds;
	int width, height;

	void compile();

public:
	GLuint shaderId;

	JuliaGreyComputeShader(const std::string& path);
	void load();
	void setTextureSize(int width, int height);
	std::vector<float>* computeTexture(int maxIter, Vector2 origin, const Vector4& window);

	~JuliaGreyComputeShader();
};

#endif