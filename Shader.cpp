#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

Shader::Shader() : shaderId(0), path()
{

}

Shader::Shader(const string& path) : shaderId(0)
{
    this->path = path;
}

ShaderProgrammSource Shader::parse_shader(const string& filePath)
{
    ifstream stream(filePath);

    enum class ShaderType
    {
        NONE = -1, Vertex = 0, FRAGMENT = 1
    };

    string line;
    ShaderType type = ShaderType::NONE;

    stringstream ss[2];

    while (getline(stream, line))
    {
        if (line.find("#shader") != string::npos)
        {
            if (line.find("vertex") != string::npos)
            {
                type = ShaderType::Vertex;
            }
            else if (line.find("fragment") != string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';

        }
    }

    return { ss[0].str(), ss[1].str() };
}

GLuint Shader::compile_shader(unsigned int type, const string& source)
{
    GLuint id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    //todo : error handleing
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        cout << "Fail to compile!" << endl;
        cout << message << endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

GLuint Shader::create_shader(const string& vertexShader, const string& fragmentShader)
{
    GLuint programm = glCreateProgram();
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vertexShader);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(programm, vs);
    glAttachShader(programm, fs);
    glLinkProgram(programm);
    glValidateProgram(programm);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return programm;
}

void Shader::load()
{
    ShaderProgrammSource source = parse_shader("Shaders/julia.shader");
    shaderId = create_shader(source.vertexSource, source.fragmentSource);
}

void Shader::add_uniform(const string& name)
{
    GLint location = glGetUniformLocation(shaderId, name.data());
    uniformIds[name] = location;
}

void Shader::set_uniform1i(const string& name, int value)
{
    glUniform1i(uniformIds[name], value);
}

void Shader::set_uniform2i(const string& name, int value, int value2)
{
    glUniform2i(uniformIds[name], value, value2);
}

void Shader::set_uniform3i(const string& name, int value, int value2, int value3)
{
    glUniform3i(uniformIds[name], value, value2, value3);
}

void Shader::set_uniform4i(const string& name, int value, int value2, int value3, int value4)
{
    glUniform4i(uniformIds[name], value, value2, value3, value4);
}

void Shader::set_uniform1f(const string& name, float value)
{
    glUniform1f(uniformIds[name], value);
}

void Shader::set_uniform2f(const string& name, const Vector2& value)
{
    glUniform2f(uniformIds[name], value.x, value.y);
}

void Shader::set_uniform3f(const string& name, const Vector3& value)
{
    glUniform3f(uniformIds[name], value.x, value.y, value.z);
}

void Shader::set_uniform4f(const string& name, const Vector4& value)
{
    glUniform4f(uniformIds[name], value.x, value.y, value.z, value.w);
}

void Shader::set_uniform1fv(const string& name, vector<float> values)
{
    glUniform1fv(uniformIds[name], (int)values.size(), values.data());
}

void Shader::set_uniform2fv(const string& name, vector<Vector2> values)
{
    vector<float> floatValues = vector<float>();
    floatValues.reserve(values.size() * 2);
    for (Vector2 v : values)
    {
        floatValues.push_back(v.x);
        floatValues.push_back(v.y);
    }
    glUniform2fv(uniformIds[name], (int)values.size(), floatValues.data());
}

void Shader::set_uniform3fv(const string& name, vector<Vector3> values)
{
    vector<float> floatValues = vector<float>();
    floatValues.reserve(values.size() * 3);
    for (Vector3 v : values)
    {
        floatValues.push_back(v.x);
        floatValues.push_back(v.y);
        floatValues.push_back(v.z);
    }
    glUniform3fv(uniformIds[name], (int)values.size(), floatValues.data());
}

void Shader::set_uniform4fv(const string& name, vector<Vector4> values)
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
    glUniform4fv(uniformIds[name], (int)values.size(), floatValues.data());
}