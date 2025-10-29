#ifndef JULIA_FRACTAL_HPP
#define JULIA_FRACTAL_HPP

#include <GLES3/gl32.h>
#include <EGL/egl.h>
#include <string>
#include "FragmentShader.hpp"
#include "FractaleParam.hpp"

class JuliaFractal
{
private:
    GLfloat const verticesPositions[8] =
    {
        -1.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, -1.0f,
        -1.0f, -1.0f,
    };

    GLuint const verticesIndices[6] =
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
    void draw();
    ~JuliaFractal();
};

#endif