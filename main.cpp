#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include "FractaleParam.hpp"
#include "Vector.hpp"
#include "FractalUpdater.hpp"
#include "Random.hpp"
#include "Math.hpp"
#include "shader/JuliaFractal.hpp"

using namespace std;

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
//Vector2 mousePosition;
//Vector2 normalizeMousePosition; //between -1 and 1
int windowWidth = 1920;
int windowHeight = 1080;
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

int windowWidth = 1600;
int windowHeight = 900;
const bool fullscreen = false;

int main()
{
    Random::setRandomSeed();

    if (glfwInit() == 0)
        return -1;

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    windowWidth = fullscreen ? mode->width : windowWidth;
    windowHeight = fullscreen ? mode->height : windowHeight;
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "FractalGenerator", fullscreen ? primaryMonitor : nullptr, nullptr);
    
	if (window == nullptr)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
    
	glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glewInit();
	glfwSetFramebufferSizeCallback(window, onFrameBufferResize);

    JuliaFractal juliaFractal;
    FractalUpdater fractalUpdater(windowWidth, windowHeight);
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT);

        float dt = getDeltaTime();

        fractalUpdater.update(dt);

        const FractaleParam& fp = fractalUpdater.getFractaleParam();

        juliaFractal.setGenerationParam(fp);

        juliaFractal.draw(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

    glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}