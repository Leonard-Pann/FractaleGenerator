#include <GLES3/gl3.h>
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

int windowWidth = 1600;
int windowHeight = 900;
const bool fullscreen = false;
Vector2 mousePosition;
Vector2 normalizeMousePosition;

static void errorCallback(int error, const char* description)
{
    cout << "GLFW error " << error << " : " << description;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

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

void onMouseMove(GLFWwindow* window, double x, double y)
{
   mousePosition.x = x;
   mousePosition.y = y;
   normalizeMousePosition.x = (mousePosition.x / (windowWidth - 1.0f)) * 2.0f - 1.0f; //between -1 and 1
   normalizeMousePosition.y = ((mousePosition.y / (windowHeight - 1.0f)) * -2.0f + 1.0f); //between -1 and 1
}

int main()
{
    Random::setRandomSeed();
    Random::setSeed(42);

    glfwSetErrorCallback(errorCallback);

    if (glfwInit() == 0)
    {
        cout << "Failed in glfwInit" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);


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
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSwapInterval(0); // disable v-sync

	glfwSetFramebufferSizeCallback(window, onFrameBufferResize);
    glfwSetCursorPosCallback(window, onMouseMove);

    JuliaFractal juliaFractal;
    FractalUpdater fractalUpdater(windowWidth, windowHeight);
    float totalTime(0.0);
    int nbFrame(0);
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClear(GL_COLOR_BUFFER_BIT);

        float dt = getDeltaTime();

        // fractalUpdater.update(dt);

        FractaleParam& fp = fractalUpdater.getFractaleParam();
        // fp.origin = normalizeMousePosition;

        juliaFractal.setGenerationParam(fp);

        juliaFractal.draw(window);

		glfwSwapBuffers(window);
		glfwPollEvents();

        nbFrame++;
        totalTime += dt;
	}

    ofstream outfile("bench.txt");
    outfile << static_cast<float>(nbFrame) / totalTime << endl;
    outfile.close();

    glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}