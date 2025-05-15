#include "JuliaFractal.hpp"

using namespace std;

JuliaFractal::JuliaFractal(const string& path)
{
    shader = Shader(path);
    shader.load();
}


void JuliaFractal::draw(GLFWwindow* window)
{

}