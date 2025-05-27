#include "JuliaGreyComputeShader.hpp"

using namespace std;

JuliaGreyComputeShader::JuliaGreyComputeShader() : ComputeShader::ComputeShader("Shaders/juliaGreyCompute.shader"), ssbo(0)
{

}

void JuliaGreyComputeShader::load()
{
    ComputeShader::load();

    glGenBuffers(1, &ssbo);

    addUniform("seed");
    addUniform("window");
    addUniform("maxIter");
    addUniform("textureSize");
}

vector<float>* JuliaGreyComputeShader::computeTexture(int maxIter, Vector2 origin, const Vector4& window, int textWidth, int textHeight)
{
    glUseProgram(m_shaderId);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, textWidth * textHeight * sizeof(float), nullptr, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    setUniform2f("seed", origin);
    setUniform4f("window", window);
    setUniform1i("maxIter", maxIter);
    setUniform2i("textureSize", textWidth, textHeight);

    // Lancer le shader (on divise par local_size = 16)
    glDispatchCompute((GLuint)((textWidth + 15) / 16), (GLuint)((textHeight + 15) / 16), 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

    float* data = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    vector<float>* res = new vector<float>{ data, data + (textWidth * textHeight) };

    if (data != nullptr)
    {
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

    return res;
}

JuliaGreyComputeShader::~JuliaGreyComputeShader()
{
    glDeleteBuffers(1, &ssbo);
}

