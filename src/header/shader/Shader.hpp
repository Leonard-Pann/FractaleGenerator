#ifndef SHADER_HPP
#define SHADER_HPP

#include <GLES3/gl32.h>
#include <EGL/egl.h>
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
    Shader(const std::string& path);

    GLuint shaderId() const;

    virtual void load();

    void addUniform(const std::string& name);
    void setUniform1i(const std::string& name, int value);
    void setUniform2i(const std::string& name, int value, int value2);
    void setUniform3i(const std::string& name, int value, int value2, int value3);
    void setUniform4i(const std::string& name, int value, int value2, int value3, int value4);
    void setUniform1f(const std::string& name, float value);
    void setUniform2f(const std::string& name, const Vector2& value);
    void setUniform3f(const std::string& name, const Vector3& value);
    void setUniform4f(const std::string& name, const Vector4& value);
    void setUniform1fv(const std::string& name, const std::vector<float>& values);
    void setUniform2fv(const std::string& name, const std::vector<Vector2>& values);
    void setUniform3fv(const std::string& name, const std::vector<Vector3>& values);
    void setUniform4fv(const std::string& name, const std::vector<Vector4>& values);
    virtual ~Shader();
};

#endif
