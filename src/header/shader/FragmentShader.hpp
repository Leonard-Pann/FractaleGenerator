#ifndef FRAGMENT_SHADER_HPP
#define FRAGMENT_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include "Shader.hpp"

struct ShaderProgrammSource
{
	std::string vertexSource;
	std::string fragmentSource;
};

class FragmentShader : public Shader
{
private:

	ShaderProgrammSource parseShader(const std::string& filePath);
	GLuint compileShader(unsigned int type, const std::string& source);
	GLuint createShader(const std::string& vertexShader, const std::string& fragmentShader);

public:

	FragmentShader(const std::string& path);
	void load() override;
	~FragmentShader() override;
};

#endif