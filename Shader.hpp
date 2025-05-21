#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <map>
#include <vector>
#include "Vector.hpp"

class Shader
{
protected:
	std::map<std::string, GLint> m_uniformIds;
	std::string m_path;
	GLuint m_shaderId;

public:

	GLuint shaderId() { return m_shaderId; };

	Shader(const std::string& path);
	virtual void load() {};
	void addUniform(const std::string& name);
	void setUniform1i(const std::string& name, int value);
	void setUniform2i(const std::string& name, int value, int value2);
	void setUniform3i(const std::string& name, int value, int value2, int value3);
	void setUniform4i(const std::string& name, int value, int value2, int value3, int value4);
	void setUniform1f(const std::string& name, float value);
	void setUniform2f(const std::string& name, const Vector2& value);
	void setUniform3f(const std::string& name, const Vector3& value);
	void setUniform4f(const std::string& name, const Vector4& value);
	void setUniform1fv(const std::string& name, std::vector<float> values);
	void setUniform2fv(const std::string& name, std::vector<Vector2> values);
	void setUniform3fv(const std::string& name, std::vector<Vector3> values);
	void setUniform4fv(const std::string& name, std::vector<Vector4> values);
	virtual ~Shader();
};


#endif