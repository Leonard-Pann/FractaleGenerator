#include <SDL2/SDL.h>
#include <EGL/egl.h>
#include <GLES3/gl32.h>
#include <iostream>
#include <fstream>
#include <string>
// #include "FractaleParam.hpp"
// #include "Vector.hpp"
// #include "FractalUpdater.hpp"
// #include "Random.hpp"
// #include "Math.hpp"
// #include "shader/JuliaFractal.hpp"

using namespace std;

int windowWidth = 1920;
int windowHeight = 1080;
const bool fullscreen = false;

// double getDeltaTime()
// {
//     static double previousSeconds = 0.0f;
//     double currentSeconds = glfwGetTime(); // seconds pass from the app start
//     double dt = currentSeconds - previousSeconds;
//     previousSeconds = currentSeconds;
//     return dt;
// }

// void onFrameBufferResize(GLFWwindow* window, int width, int height)
// {
//     glViewport(0, 0, width, height);
// }

int main()
{
    // Random::setRandomSeed();

    SDL_SetHint(SDL_HINT_VIDEO_X11_FORCE_EGL, "1");

    if (SDL_Init(SDL_INIT_VIDEO) != 0) 
    {
        std::cout << "SDL_Init failed: " << SDL_GetError() << "\n";
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    windowWidth = fullscreen ? mode->width : windowWidth;
    windowHeight = fullscreen ? mode->height : windowHeight;

    SDL_Window* window = SDL_CreateWindow("Fractal Generator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                            windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (window == nullptr)
    {
        cout << "SDL_CreateWindow failed: " << SDL_GetError() << endl;
        SDL_Quit();
        return -1;
    }

    SDL_GLContext glctx = SDL_GL_CreateContext(window);
    if (glctx == nullptr)
    {
        cout << "SDL_GL_CreateContext failed: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	// glfwSetFramebufferSizeCallback(window, onFrameBufferResize);


    if (SDL_GL_MakeCurrent(window, glctx) != 0) 
    {
        cout << "SDL_GL_MakeCurrent failed: " << SDL_GetError() << endl;
    }

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version  = glGetString(GL_VERSION);
    const GLubyte* shading  = glGetString(GL_SHADING_LANGUAGE_VERSION);
    cout << "Renderer: " << (renderer ? (const char*)renderer : "N/A") << endl;
    cout << "GLES Version: " << (version ? (const char*)version : "N/A") << endl;
    cout << "GLSL ES Version: " << (shading ? (const char*)shading : "N/A") << endl;

    bool running = true;
    SDL_Event ev;
    
    // JuliaFractal juliaFractal;
    // FractalUpdater fractalUpdater(windowWidth, windowHeight);
    while (running) 
    {
        while (SDL_PollEvent(&ev)) 
        {
            if (ev.type == SDL_QUIT || ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) 
            {
                running = false;
            }
        }

        glViewport(0, 0, windowWidth, windowHeight);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    //     float dt = getDeltaTime();
    //     fractalUpdater.update(dt);
    //     const FractaleParam& fp = fractalUpdater.getFractaleParam();
    //     juliaFractal.setGenerationParam(fp);
    //     juliaFractal.draw(window);

        SDL_GL_SwapWindow(window);
        SDL_Delay(16); // ~60fps idle
    }

    SDL_GL_DeleteContext(glctx);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}