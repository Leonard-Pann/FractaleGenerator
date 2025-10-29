#include "shader/JuliaGreyFragmentShader.hpp"
#include <iostream>

using namespace std;

JuliaGreyFragmentShader::JuliaGreyFragmentShader() : FragmentShader("shaders/juliaGreyFragment.shader"), fbo(0), textureId(0), texWidth(0), texHeight(0)
{

}

void JuliaGreyFragmentShader::load()
{
    FragmentShader::load();

    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &textureId);

    addUniform("seed");
    addUniform("window");
    addUniform("maxIter");
    addUniform("textureSize");
}

std::vector<float> JuliaGreyFragmentShader::computeTexture(int maxIter, Vector2 origin, const Vector4& window, int width, int height)
{
    texWidth = width;
    texHeight = height;

    // Setup render target
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        cout << "FBO incomplete" << endl;
        return {};
    }

    glViewport(0, 0, width, height);
    glUseProgram(m_shaderId);

    setUniform2f("seed", origin);
    setUniform4f("window", window);
    setUniform1i("maxIter", maxIter);
    setUniform2i("textureSize", width, height);

    // Draw fullscreen quad
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3); // assume shader generates frag color

    std::vector<float> pixels(width * height);
    glReadPixels(0, 0, width, height, GL_RED, GL_FLOAT, pixels.data());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return pixels;
}

JuliaGreyFragmentShader::~JuliaGreyFragmentShader()
{
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &textureId);
}
