#ifndef TEXTURE_VARIATION_FRAGMENT_SHADER_HPP
#define TEXTURE_VARIATION_FRAGMENT_SHADER_HPP

#include <GLES3/gl32.h>
#include <EGL/egl.h>
#include <string>
#include <vector>
#include "FragmentShader.hpp"

class TextureVariationFragmentShader : public FragmentShader
{
private:
    GLuint fbo;
    GLuint inputTexture;
    GLuint outputTexture;

public:
    TextureVariationFragmentShader();
    void load() override;
    float computeMeanTextureVariation(const std::vector<float>& texture, int width, int height);
    ~TextureVariationFragmentShader() override;
};

#endif
