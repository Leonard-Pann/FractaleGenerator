#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <map>
#include <vector>
#include "Vector.hpp"

struct ShaderProgrammSource
{
	std::string vertexSource;
	std::string fragmentSource;
};

class Shader
{
private:
	std::string path;
	std::map<std::string, GLint> uniformIds;

	ShaderProgrammSource parse_shader(const std::string& filePath);
	GLuint compile_shader(unsigned int type, const std::string& source);
	GLuint create_shader(const std::string& vertexShader, const std::string& fragmentShader);

public:
	GLuint shaderId;

	Shader();
	Shader(const std::string& path);
	void load();
	void add_uniform(const std::string& name);
	void set_uniform1i(const std::string& name, int value);
	void set_uniform2i(const std::string& name, int value, int value2);
	void set_uniform3i(const std::string& name, int value, int value2, int value3);
	void set_uniform4i(const std::string& name, int value, int value2, int value3, int value4);
	void set_uniform1f(const std::string& name, float value);
	void set_uniform2f(const std::string& name, const Vector2& value);
	void set_uniform3f(const std::string& name, const Vector3& value);
	void set_uniform4f(const std::string& name, const Vector4& value);
	void set_uniform1fv(const std::string& name, std::vector<float> values);
	void set_uniform2fv(const std::string& name, std::vector<Vector2> values);
	void set_uniform3fv(const std::string& name, std::vector<Vector3> values);
	void set_uniform4fv(const std::string& name, std::vector<Vector4> values);
	~Shader();
};


#endif