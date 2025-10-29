#ifndef JULIA_GREY_FRAGMENT_SHADER_HPP
#define JULIA_GREY_FRAGMENT_SHADER_HPP

#include <GLES3/gl32.h>
#include <EGL/egl.h>
#include <string>
#include <vector>
#include "FragmentShader.hpp"
#include "Vector.hpp"

class JuliaGreyFragmentShader : public FragmentShader
{
private:
    GLuint fbo;
    GLuint textureId;
    int texWidth, texHeight;

public:
    JuliaGreyFragmentShader();
    void load() override;
    std::vector<float> computeTexture(int maxIter, Vector2 origin, const Vector4& window, int width, int height);
    ~JuliaGreyFragmentShader() override;
};

#endif
