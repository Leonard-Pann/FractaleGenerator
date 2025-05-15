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

    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(windowWidth, windowHeight, "FractalGenerator", NULL, NULL);
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

    Shader juliaShader = Shader("Shaders/julia.shader");
    juliaShader.load();

    juliaShader.add_uniform("seed");
    juliaShader.add_uniform("window");
    juliaShader.add_uniform("maxIter");
    juliaShader.add_uniform("inColor");
    juliaShader.add_uniform("colorPalette");
    juliaShader.add_uniform("nbColors");

    FractalUpdater fractalUpdater = FractalUpdater();
    fractalUpdater.init();

    GLfloat vertices_positions[] =
    {
        -1.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, -1.0f,
        -1.0f, -1.0f,
    };

    GLuint vertices_indices[] =
    {
        0, 1, 2,
        0, 2, 3,
    };
    GLuint positionBuffer;
    GLuint vao;

    while (!glfwWindowShouldClose(window))
    {
        glGenBuffers(1, &positionBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_positions), vertices_positions, GL_STATIC_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        GLuint indicesBuffer;
        glGenBuffers(1, &indicesBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices_indices), vertices_indices, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
        glEnableVertexAttribArray(0);


        glClear(GL_COLOR_BUFFER_BIT);

        float dt = getDeltaTime();
        std::cout << 1 / dt << std::endl;
        //fractalUpdater.update(dt);

        FractaleParam& param = fractalUpdater.getFractaleParam();

        float xNorm = lerp(-2.0f, 2.0f, (normalizeMousePosition.x * 0.5f) + 0.5f);
        float yNorm = lerp(-2.0f, 2.0f, (normalizeMousePosition.y * 0.5f) + 0.5f);
        Vector2 newOrigin = Vector2(xNorm, yNorm);
        param.origin = newOrigin;
        //std::cout << "x: " << newOrigin.x << " y: " << newOrigin.y << std::endl;

        glUseProgram(juliaShader.shaderId);
        juliaShader.set_uniform1i("maxIter", param.maxIter);
        juliaShader.set_uniform1i("nbColors", (int)param.colorPalette.size());
        juliaShader.set_uniform2f("seed", param.origin);
        juliaShader.set_uniform3f("inColor", param.inColor);
        juliaShader.set_uniform4f("window", Vector4(param.xMin, param.xMax, param.yMin, param.yMax));
        juliaShader.set_uniform3fv("colorPalette", param.colorPalette);

        glBindVertexArray(vao);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &positionBuffer);

    glDeleteProgram(juliaShader.shaderId);

    glfwTerminate();
    return 0;
}