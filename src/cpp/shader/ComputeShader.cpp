#include "shader/ComputeShader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

ComputeShader::ComputeShader(const string& path) : Shader::Shader(path)
{

}

void ComputeShader::compile()
{
    ifstream file(m_path);
    stringstream ss;
    ss << file.rdbuf();
    string source = ss.str();
    const char* src = source.c_str();

    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success == 0)
    {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        cout << "Shader " << m_path << " compile error :\n" << log << endl;
        glDeleteShader(shader);
        return;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == 0)
    {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        cout << "Program "<< m_path << " link error :\n" << log << endl;
        glDeleteShader(shader);
        return;
    }

    glDeleteShader(shader);
    m_shaderId = program;
}

void ComputeShader::load()
{
    Shader::load();
    compile();
}

ComputeShader::~ComputeShader()
{
    
}