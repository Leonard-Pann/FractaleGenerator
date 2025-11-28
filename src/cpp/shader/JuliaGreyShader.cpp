#include "shader/JuliaGreyShader.hpp"

using namespace std;

static const float quadVertices[] =
{
    -1.0f, -1.0f,
     1.0f, -1.0f,
     1.0f,  1.0f,
    -1.0f,  1.0f
};

static const GLuint quadIndices[] =
{
    0, 1, 2,
    0, 2, 3
};

JuliaGreyShader::JuliaGreyShader() : FragmentShader::FragmentShader("shaders/juliaGrey.shader"), vao(0), vbo(0), ebo(0), fbo(0), greyTexture(0)
{

}

void JuliaGreyShader::load()
{
    FragmentShader::load();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);

    glBindVertexArray(0);

    glGenFramebuffers(1, &fbo);

    addUniform("seed");
    addUniform("window");
    addUniform("maxIter");
    addUniform("textureSize");
}

vector<float>* JuliaGreyShader::computeTexture(int maxIter, Vector2 origin, const Vector4& window, int textWidth, int textHeight)
{
    glUseProgram(shaderId());

    if (greyTexture == 0)
    {
        glGenTextures(1, &greyTexture);
    }

    glBindTexture(GL_TEXTURE_2D, greyTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, textWidth, textHeight, 0, GL_RED, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, greyTexture, 0);

    glViewport(0, 0, textWidth, textHeight);

    setUniform2f("seed", origin);
    setUniform4f("window", window);
    setUniform1i("maxIter", maxIter);
    setUniform2i("textureSize", textWidth, textHeight);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    vector<float>* output = new vector<float>(textWidth * textHeight);
    glReadPixels(0, 0, textWidth, textHeight, GL_RED, GL_FLOAT, output->data());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);
    
    return output;
}

JuliaGreyShader::~JuliaGreyShader()
{
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &fbo);
    glDeleteTextures(1, &greyTexture);
}
