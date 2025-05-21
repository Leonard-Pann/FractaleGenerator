#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include "FractaleParam.hpp"
#include "Vector.hpp"
#include "FractalUpdater.hpp"
#include "Random.hpp"
#include "math.hpp"
#include "Shader.hpp"
#include "JuliaFractal.hpp"

using namespace std;

Vector2 mousePosition;
Vector2 normalizeMousePosition; //between -1 and 1
const int windowWidth = 1920;
const int windowHeight = 1080;

double getDeltaTime()
{
    static double previousSeconds = 0.0f;
    double currentSeconds = glfwGetTime(); // nombre de secondes depuis le début de l'exécution
    double dt = currentSeconds - previousSeconds;
    previousSeconds = currentSeconds;
    return dt;
}

void onFrameBufferResize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void onMouseMove(GLFWwindow* window, double x, double y)
{
    mousePosition.x = x;
    mousePosition.y = y;
    normalizeMousePosition.x = (mousePosition.x / (windowWidth - 1.0f)) * 2.0f - 1.0f; //between -1 and 1
    normalizeMousePosition.y = ((mousePosition.y / (windowHeight - 1.0f)) * -2.0f + 1.0f); //between -1 and 1
}

void onMouseScroll(GLFWwindow* window, double deltaX, double deltaY)
{
    // nothing yet
}

int main(void)
{
    Random::set_random_seed();

    if (glfwInit() == 0)
        return -1;

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "FractalGenerator", NULL, NULL);
    if (window == nullptr)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        cout << "Error in glewInit" << endl;
    }
    glewExperimental = GL_TRUE;

    glfwSetScrollCallback(window, onMouseScroll);
    glfwSetFramebufferSizeCallback(window, onFrameBufferResize);
    glfwSetCursorPosCallback(window, onMouseMove);
    //glfwSwapInterval(0); // Désactiver la VSync

    //JuliaFractal juliaFractal("Shaders/julia.shader");
    JuliaFractal juliaFractal("Shaders/juliaGrey.shader");

    FractalUpdater fractalUpdater;

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        float dt = getDeltaTime();
        //std::cout << 1 / dt << std::endl;

        float xNorm = lerp(-2.0f, 2.0f, (normalizeMousePosition.x * 0.5f) + 0.5f);
        float yNorm = lerp(-2.0f, 2.0f, (normalizeMousePosition.y * 0.5f) + 0.5f);
        Vector2 newOrigin = Vector2(xNorm, yNorm);
        //std::cout << "x: " << newOrigin.x << " y: " << newOrigin.y << std::endl;

        fractalUpdater.update(dt, newOrigin);

        FractaleParam& param = fractalUpdater.getFractaleParam();
        juliaFractal.setGenerationParam(param);
        juliaFractal.draw(window);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}