#include "JuliaFractal.hpp"

using namespace std;

JuliaFractal::JuliaFractal(const string& path)
{
    shader = Shader(path);
    shader.load();

    shader.add_uniform("seed");
    shader.add_uniform("window");
    shader.add_uniform("maxIter");
    shader.add_uniform("inColor");
    shader.add_uniform("colorPalette");
    shader.add_uniform("nbColors");

    glGenBuffers(1, &positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_positions), vertices_positions, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &indicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices_indices), vertices_indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
    glEnableVertexAttribArray(0);
}

void JuliaFractal::setGenerationParam(const FractaleParam& params)
{
    glUseProgram(shader.shaderId);
    shader.set_uniform1i("maxIter", params.maxIter);
    shader.set_uniform1i("nbColors", (int)params.colorPalette.size());
    shader.set_uniform2f("seed", params.origin);
    shader.set_uniform3f("inColor", params.inColor);
    shader.set_uniform4f("window", Vector4(params.xMin, params.xMax, params.yMin, params.yMax));
    shader.set_uniform3fv("colorPalette", params.colorPalette);
}

void JuliaFractal::draw(GLFWwindow* window)
{
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);

    glBindVertexArray(vao);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void JuliaFractal::drawInTexture(GLuint textureID, int textWidth, int textHeight, GLuint fbo)
{
    glViewport(0, 0, textWidth, textHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glUseProgram(shader.shaderId);

    glBindVertexArray(NULL);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void JuliaFractal::deleteProgram()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &positionBuffer);

    glDeleteProgram(shader.shaderId);
}