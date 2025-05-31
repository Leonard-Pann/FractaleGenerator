#ifndef TEXTURE_VARIATION_SHADER_HPP
#define TEXTURE_VARIATION_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include "../../header/shader/ComputeShader.hpp"

class TextureVariationShader : public ComputeShader
{
private:
	GLuint ssboInput, ssboOutput;

public:
	TextureVariationShader();
	void load() override;
	float computeMeanTextureVariation(const std::vector<float>& texture, int width, int height);
	~TextureVariationShader() override;
};

#endif