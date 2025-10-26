#include "shader/Shader.hpp"

using namespace std;

Shader::Shader(const string& path) : m_shaderId(0), m_path(path)
{

}

void Shader::load()
{

}


void Shader::addUniform(const string& name)
{
    GLint location = glGetUniformLocation(m_shaderId, name.data());
    m_uniformIds[name] = location;
}

void Shader::setUniform1i(const string& name, int value)
{
    glUniform1i(m_uniformIds[name], value);
}

void Shader::setUniform2i(const string& name, int value, int value2)
{
    glUniform2i(m_uniformIds[name], value, value2);
}

void Shader::setUniform3i(const string& name, int value, int value2, int value3)
{
    glUniform3i(m_uniformIds[name], value, value2, value3);
}

void Shader::setUniform4i(const string& name, int value, int value2, int value3, int value4)
{
    glUniform4i(m_uniformIds[name], value, value2, value3, value4);
}

void Shader::setUniform1f(const string& name, float value)
{
    glUniform1f(m_uniformIds[name], value);
}

void Shader::setUniform2f(const string& name, const Vector2& value)
{
    glUniform2f(m_uniformIds[name], value.x, value.y);
}

void Shader::setUniform3f(const string& name, const Vector3& value)
{
    glUniform3f(m_uniformIds[name], value.x, value.y, value.z);
}

void Shader::setUniform4f(const string& name, const Vector4& value)
{
    glUniform4f(m_uniformIds[name], value.x, value.y, value.z, value.w);
}

void Shader::setUniform1fv(const string& name, vector<float> values)
{
    glUniform1fv(m_uniformIds[name], (int)values.size(), values.data());
}

void Shader::setUniform2fv(const string& name, vector<Vector2> values)
{
    vector<float> floatValues = vector<float>();
    floatValues.reserve(values.size() * 2);
    for (Vector2 v : values)
    {
        floatValues.push_back(v.x);
        floatValues.push_back(v.y);
    }
    glUniform2fv(m_uniformIds[name], (int)values.size(), floatValues.data());
}

void Shader::setUniform3fv(const string& name, vector<Vector3> values)
{
    vector<float> floatValues = vector<float>();
    floatValues.reserve(values.size() * 3);
    for (Vector3 v : values)
    {
        floatValues.push_back(v.x);
        floatValues.push_back(v.y);
        floatValues.push_back(v.z);
    }
    glUniform3fv(m_uniformIds[name], (int)values.size(), floatValues.data());
}

void Shader::setUniform4fv(const string& name, vector<Vector4> values)
{
    vector<float> floatValues = vector<float>();
    floatValues.reserve(values.size() * 4);
    for (Vector4 v : values)
    {
        floatValues.push_back(v.x);
        floatValues.push_back(v.y);
        floatValues.push_back(v.z);
        floatValues.push_back(v.w);
    }
    glUniform4fv(m_uniformIds[name], (int)values.size(), floatValues.data());
}

Shader::~Shader()
{
    glDeleteProgram(m_shaderId);
}
