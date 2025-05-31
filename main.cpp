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
#include "shader/JuliaFractal.hpp"

using namespace std;

//Vector2 mousePosition;
//Vector2 normalizeMousePosition; //between -1 and 1
int windowWidth;
int windowHeight;
const bool fullscreen = true;

double getDeltaTime()
{
    static double previousSeconds = 0.0f;
    double currentSeconds = glfwGetTime(); // seconds pass from the app start
    double dt = currentSeconds - previousSeconds;
    previousSeconds = currentSeconds;
    return dt;
}

void onFrameBufferResize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//void onMouseMove(GLFWwindow* window, double x, double y)
//{
//    mousePosition.x = x;
//    mousePosition.y = y;
//    normalizeMousePosition.x = (mousePosition.x / (windowWidth - 1.0f)) * 2.0f - 1.0f; //between -1 and 1
//    normalizeMousePosition.y = ((mousePosition.y / (windowHeight - 1.0f)) * -2.0f + 1.0f); //between -1 and 1
//}
//
//void onMouseScroll(GLFWwindow* window, double deltaX, double deltaY)
//{
//    // nothing yet
//}

int main()
{
    Random::setRandomSeed();

    if (glfwInit() == 0)
        return -1;

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    windowWidth = mode->width;
    windowHeight = mode->height;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "FractalGenerator", fullscreen ? primaryMonitor : nullptr, nullptr);

    if (window == nullptr)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    if (glewInit() != GLEW_OK)
    {
        cout << "Error in glewInit" << endl;
    }
    glewExperimental = GL_TRUE;

    //glfwSetScrollCallback(window, onMouseScroll);
    glfwSetFramebufferSizeCallback(window, onFrameBufferResize);
    //glfwSetCursorPosCallback(window, onMouseMove);
    //glfwSwapInterval(0); // disable VSync

    JuliaFractal juliaFractal("shaders/julia.shader");
    FractalUpdater fractalUpdater(windowWidth, windowHeight);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        float dt = getDeltaTime();
        fractalUpdater.update(dt);

        juliaFractal.setGenerationParam(fractalUpdater.getFractaleParam());

        juliaFractal.draw(window);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}