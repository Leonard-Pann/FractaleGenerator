#ifndef JULIA_FRACTAL_HPP
#define JULIA_FRACTAL_HPP

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include <string>
#include "FragmentShader.hpp"
#include "FractaleParam.hpp"

class JuliaFractal
{
private:
    GLfloat const vertices_positions[8] =
    {
        -1.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, -1.0f,
        -1.0f, -1.0f,
    };

    GLuint const vertices_indices[6] =
    {
        0, 1, 2,
        0, 2, 3,
    };

    GLuint positionBuffer;
    GLuint vao;
    GLuint indicesBuffer;

public:
	FragmentShader shader;

	JuliaFractal();
    void setGenerationParam(const FractaleParam& params);
    void draw(GLFWwindow* window);
    ~JuliaFractal();
};

#endif