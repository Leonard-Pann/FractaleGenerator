#include "FragmentShader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

FragmentShader::FragmentShader(const string& path) : Shader::Shader(path)
{

}

ShaderProgrammSource FragmentShader::parseShader(const string& filePath)
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

GLuint FragmentShader::compileShader(unsigned int type, const string& source)
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
        cout << "Fail to compile shader : " << m_path << endl;
        cout << message << endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

GLuint FragmentShader::createShader(const string& vertexShader, const string& fragmentShader)
{
    GLuint programm = glCreateProgram();
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(programm, vs);
    glAttachShader(programm, fs);
    glLinkProgram(programm);
    glValidateProgram(programm);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return programm;
}

void FragmentShader::load()
{
    Shader::load();
    ShaderProgrammSource source = parseShader(m_path);
    m_shaderId = createShader(source.vertexSource, source.fragmentSource);
}

FragmentShader::~FragmentShader()
{
   
}