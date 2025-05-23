#include "TextureVariationShader.hpp"

using namespace std;

TextureVariationShader::TextureVariationShader() : ComputeShader::ComputeShader("Shaders/squareTextureVariation.shader"), ssboInput(0), ssboOutput(0)
{

}

void TextureVariationShader::load()
{
    ComputeShader::load();

    glGenBuffers(1, &ssboInput);

    glGenBuffers(1, &ssboOutput);

    addUniform("width");
    addUniform("height");
}

float TextureVariationShader::computeMeanTextureVariation(const std::vector<float>& texture, int width, int height)
{
    glUseProgram(m_shaderId);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboInput);
    glBufferData(GL_SHADER_STORAGE_BUFFER, (width * height) * sizeof(float), texture.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboInput);

    uint32_t zero = 0.0f;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboOutput);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32_t), &zero, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboOutput);

    setUniform1i("width", width);
    setUniform1i("height", height);

    GLuint localSize = 256;
    GLuint numGroups = ((width * height) + localSize - 1) / localSize;

    glDispatchCompute(numGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboOutput);
    uint32_t* result = (uint32_t*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    if (result != nullptr)
    {
        double nbWorkingGroup = double(width * height) / double(localSize);
        double maxSumFloat = 256.0;
        double factor = 4000000000.0 / (maxSumFloat * nbWorkingGroup);

        float res = (float)((double)*result / factor);
        res /= width * height;
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        return res;
    }
    return 0.0f;
}


TextureVariationShader::~TextureVariationShader()
{
    glDeleteBuffers(1, &ssboInput);
    glDeleteBuffers(1, &ssboOutput);
}

