#include "JuliaGreyComputeShader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

JuliaGreyComputeShader::JuliaGreyComputeShader(const string& path) : path(path)
{

}

void JuliaGreyComputeShader::compile()
{
    ifstream file(path);
    stringstream ss;
    ss << file.rdbuf();
    string source = ss.str();
    const char* src = source.c_str();

    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        cout << "Shader compile error:\n" << log << endl;
        glDeleteShader(shader);
        return;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        cout << "Program link error:\n" << log << endl;
        glDeleteShader(shader);
        return;
    }

    glDeleteShader(shader);
    shaderId = program;
}

void JuliaGreyComputeShader::load()
{
    compile();
    glGenBuffers(1, &ssbo);

    GLint seedLocation = glGetUniformLocation(shaderId, "seed");
    uniformIds["seed"] = seedLocation;
    GLint windowLocation = glGetUniformLocation(shaderId, "window");
    uniformIds["window"] = windowLocation;
    GLint maxIterLocation = glGetUniformLocation(shaderId, "maxIter");
    uniformIds["maxIter"] = maxIterLocation;
    GLint textSizeLocation = glGetUniformLocation(shaderId, "textureSize");
    uniformIds["textureSize"] = textSizeLocation;
}

void JuliaGreyComputeShader::setTextureSize(int width, int height)
{
    this->width = width;
    this->height = height;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, width * height * sizeof(float), nullptr, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
}

vector<float>* JuliaGreyComputeShader::computeTexture(int maxIter, Vector2 origin, const Vector4& window)
{
    glUseProgram(shaderId);
    glUniform2f(uniformIds["seed"], origin.x, origin.y);
    glUniform4f(uniformIds["window"], window.x, window.y, window.z, window.w);
    glUniform1i(uniformIds["maxIter"], maxIter);
    glUniform2i(uniformIds["textureSize"], width, height);

    // Lancer le shader (on divise par local_size = 16)
    glDispatchCompute((GLuint)((width + 15) / 16), (GLuint)((height + 15) / 16), 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    float* data = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    if (data != nullptr)
    {
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

    return new vector<float>{ data, data + (width * height) };
}

JuliaGreyComputeShader::~JuliaGreyComputeShader()
{
    glDeleteBuffers(1, &ssbo);
    glDeleteProgram(shaderId);
}

