#include "shader/JuliaFractal.hpp"
#include <iostream>

using namespace std;

JuliaFractal::JuliaFractal() : shader("shaders/julia.shader")
{
    shader.load();

    shader.addUniform("seed");
    shader.addUniform("window");
    shader.addUniform("maxIter");
    shader.addUniform("inColor");
    shader.addUniform("colorPalette");
    shader.addUniform("colorRange");

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

//vector<Vector3> currentPalette =
//{
//    Vector3(4 / 255.0f,  12 / 255.0f,   90 / 255.0f),
//    Vector3(75 / 255.0f,  150 / 255.0f,   200 / 255.0f),
//    Vector3(170.0f / 255.0f,  230 / 255.0f,   250 / 255.0f),
//    Vector3(150.0f / 255.0f,  200 / 255.0f,   225 / 255.0f),
//    Vector3(75 / 255.0f,  150 / 255.0f,   200 / 255.0f),
//    Vector3(4 / 255.0f,  12 / 255.0f,   90 / 255.0f),
//};

void JuliaFractal::setGenerationParam(const FractaleParam& params)
{
    glUseProgram(shader.shaderId());
    shader.setUniform2f("seed", params.origin);
    shader.setUniform3f("inColor", params.inColor);
    shader.setUniform4f("window", Vector4(params.xMin, params.xMax, params.yMin, params.yMax));
    //shader.setUniform3fv("colorPalette", currentPalette);
    shader.setUniform3fv("colorPalette", params.colorPalette);
    //shader.setUniform1f("colorRange", 0.01f);
    shader.setUniform1f("colorRange", params.colorRange);
}

void JuliaFractal::draw(GLFWwindow* window)
{
    glUseProgram(shader.shaderId());

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);

    glBindVertexArray(vao);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

JuliaFractal::~JuliaFractal()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &positionBuffer);
    glDeleteBuffers(1, &indicesBuffer);
}