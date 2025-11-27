#ifndef JULIA_GREY_SHADER_HPP
#define JULIA_GREY_SHADER_HPP

#include <vector>
#include "FragmentShader.hpp"

class JuliaGreyShader : public FragmentShader
{
private:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint fbo;
    GLuint greyTexture;

public:
	JuliaGreyShader();
	void load() override;
	std::vector<float>* computeTexture(int maxIter, Vector2 origin, const Vector4& window, int textWidth, int textHeight);
	~JuliaGreyShader() override;
};

#endif