#include "shader/TextureVariationFragmentShader.hpp"
#include <iostream>
#include <numeric>

using namespace std;

TextureVariationFragmentShader::TextureVariationFragmentShader() : FragmentShader("shaders/textureVariationFragment.shader"), fbo(0), inputTexture(0), outputTexture(0)
{

}

void TextureVariationFragmentShader::load()
{
    FragmentShader::load();

    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &inputTexture);
    glGenTextures(1, &outputTexture);

    addUniform("width");
    addUniform("height");
    addUniform("inputTex");
}

float TextureVariationFragmentShader::computeMeanTextureVariation(const std::vector<float>& texture, int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, texture.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, outputTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, nullptr);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTexture, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        cout << "FBO incomplete" << endl;
        return 0.0f;
    }

    glViewport(0, 0, width, height);
    glUseProgram(m_shaderId);

    setUniform1i("width", width);
    setUniform1i("height", height);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    setUniform1i("inputTex", 0);

    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    vector<float> pixels(width * height);
    glReadPixels(0, 0, width, height, GL_RED, GL_FLOAT, pixels.data());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Compute average variation on CPU
    float sum = accumulate(pixels.begin(), pixels.end(), 0.0f);
    return sum / (width * height);
}

TextureVariationFragmentShader::~TextureVariationFragmentShader()
{
    glDeleteTextures(1, &inputTexture);
    glDeleteTextures(1, &outputTexture);
    glDeleteFramebuffers(1, &fbo);
}