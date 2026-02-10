#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#ifndef _DEBUG
#include <windows.h>
#endif
#include "FractaleParam.hpp"
#include "Vector.hpp"
#include "FractalUpdater.hpp"
#include "Random.hpp"
#include "Math.hpp"
#include "shader/JuliaFractal.hpp"

using namespace std;

int windowWidth = 1600;
int windowHeight = 900;
const bool fullscreen = false;

double getDeltaTime()
{
    static double previousSeconds = 0.0f;
    double currentSeconds = glfwGetTime(); // seconds pass from the app start
    double dt = currentSeconds - previousSeconds;
    previousSeconds = currentSeconds;
    return dt;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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
//     //nothing yet
//}

#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
    //Random::setRandomSeed();
    Random::setSeed(42);

    if (glfwInit() == 0)
        return -1;

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    windowWidth = fullscreen ? mode->width : windowWidth;
    windowHeight = fullscreen ? mode->height : windowHeight;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "FractalGenerator", fullscreen ? primaryMonitor : nullptr, nullptr);

    if (window == nullptr)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glewInit();

    glfwSetFramebufferSizeCallback(window, onFrameBufferResize);
    //glfwSetScrollCallback(window, onMouseScroll);
    //glfwSetCursorPosCallback(window, onMouseMove);
    //glfwSwapInterval(0); // disable VSync

    JuliaFractal juliaFractal;
    FractalUpdater fractalUpdater(windowWidth, windowHeight);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

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