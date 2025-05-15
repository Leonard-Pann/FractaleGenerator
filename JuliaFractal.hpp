#ifndef JULIA_FRACTAL_HPP
#define JULIA_FRACTAL_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include "Shader.hpp"

class JuliaFractal
{
private:
	Shader shader;

public:
	JuliaFractal(const std::string& path);
	void draw(GLFWwindow* window);
};


#endif