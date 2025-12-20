#include <EGL/egl.h>
#include <GLES3/gl32.h>
#include <X11/Xlib.h>
#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include "FractaleParam.hpp"
#include "Vector.hpp"
#include "FractalUpdater.hpp"
#include "Random.hpp"
#include "Math.hpp"
#include "shader/JuliaFractal.hpp"

using namespace std;

using std_clock = chrono::high_resolution_clock;
auto lastTime = std_clock::now();

float getDeltaTime()
{
    auto now = std_clock::now();
    chrono::duration<float> delta = now - lastTime;
    float dt = delta.count();
    lastTime = now;
    return dt;
}

int main() 
{
    Random::setRandomSeed();
    Random::setSeed(42);

    Display* xdisp = XOpenDisplay(nullptr);
    if (xdisp == nullptr) 
    {
        cout << "Cannot open X11 display" << endl;
        return -1;
    }

    int windowWidth = 1920;
    int windowHeight = 1080;
    Window root = DefaultRootWindow(xdisp);
    Window win = XCreateSimpleWindow(
        xdisp, root, 0, 0, windowWidth, windowHeight, 0,
        BlackPixel(xdisp, 0), WhitePixel(xdisp, 0)
    );

    XStoreName(xdisp, win, "Fractal Generator");
    XMapWindow(xdisp, win);

    EGLDisplay eglDpy = eglGetDisplay((EGLNativeDisplayType)xdisp);
    eglInitialize(eglDpy, nullptr, nullptr);

    EGLint cfg_attribs[] = 
    {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_RED_SIZE,        8,
        EGL_GREEN_SIZE,      8,
        EGL_BLUE_SIZE,       8,
        EGL_DEPTH_SIZE,      24,
        EGL_NONE
    };

    EGLConfig cfg;
    EGLint num_cfg;
    eglChooseConfig(eglDpy, cfg_attribs, &cfg, 1, &num_cfg);

    EGLSurface surf = eglCreateWindowSurface(eglDpy, cfg, win, nullptr);
    if(surf == nullptr)
    {
        cout << "Error during window surface creation" << endl;
    }

    eglBindAPI(EGL_OPENGL_ES_API);

    EGLint ctx_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
		EGL_CONTEXT_MINOR_VERSION, 1,
        EGL_NONE
    };
    EGLContext ctx = eglCreateContext(eglDpy, cfg, EGL_NO_CONTEXT, ctx_attribs);

    eglMakeCurrent(eglDpy, surf, surf, ctx);

    cout << (const char*)glGetString(GL_VERSION) << endl;

    bool running = true;
    XEvent event;
    JuliaFractal juliaFractal;
    FractalUpdater fractalUpdater(windowWidth, windowHeight);
    lastTime = std_clock::now();

    // float totalTime(0.0);
    // int nbFrame(0);
    while (running) 
    {
        //Don't remove this loop, mandatory for EGL event
        while (XPending(xdisp)) 
        {

        }

        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        float dt = getDeltaTime();
        fractalUpdater.update(dt);

        juliaFractal.setGenerationParam(fractalUpdater.getFractaleParam());

        juliaFractal.draw();

        eglSwapBuffers(eglDpy, surf);

        // nbFrame++;
        // totalTime += dt;

        // if(totalTime > 60.0)
        // {
        //     ofstream outfile("bench.txt");
        //     outfile << static_cast<float>(nbFrame) / totalTime << endl;
        //     outfile.close();
        //     cout << "Benchmark Done" << endl;
        //     totalTime = -999999.0;
        // }
    }

    eglMakeCurrent(eglDpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroySurface(eglDpy, surf);
    eglDestroyContext(eglDpy, ctx);
    eglTerminate(eglDpy);

    XDestroyWindow(xdisp, win);
    XCloseDisplay(xdisp);

    return 0;
}