#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>

const char* frag_shader_src =
"#version 320 es\n"
"precision highp float;\n"
"out vec4 outColor;\n"
"void main() {\n"
"    outColor = vec4(0.0, 0.5, 1.0, 1.0);\n"
"}\n";

const char* vert_shader_src =
"#version 320 es\n"
"precision highp float;\n"
"const vec2 verts[3] = vec2[3](\n"
"    vec2(-1.0, -1.0),\n"
"    vec2( 3.0, -1.0),\n"
"    vec2(-1.0,  3.0)\n"
");\n"
"void main() {\n"
"    gl_Position = vec4(verts[gl_VertexID], 0.0, 1.0);\n"
"}\n";

// Compile helper
GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Shader compile error: " << log << "\n";
        return 0;
    }
    return shader;
}

int main() {
    // -------------------------------
    // X11 DISPLAY
    // -------------------------------
    Display* xdisp = XOpenDisplay(nullptr);
    if (!xdisp) {
        std::cerr << "Cannot open X11 display\n";
        return -1;
    }

    int width = 800, height = 600;

    // -------------------------------
    // EGL DISPLAY
    // -------------------------------
    EGLDisplay eglDpy = eglGetDisplay((EGLNativeDisplayType)xdisp);
    if (eglDpy == EGL_NO_DISPLAY) {
        std::cerr << "eglGetDisplay failed: 0x" << std::hex << eglGetError() << "\n";
        return -1;
    }

    if (!eglInitialize(eglDpy, nullptr, nullptr)) {
        std::cerr << "eglInitialize failed: 0x" << std::hex << eglGetError() << "\n";
        return -1;
    }

    eglBindAPI(EGL_OPENGL_ES_API);

    EGLint cfg_attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_RED_SIZE,        8,
        EGL_GREEN_SIZE,      8,
        EGL_BLUE_SIZE,       8,
        EGL_DEPTH_SIZE,      0,
        EGL_NONE
    };

    EGLConfig config;
    EGLint num_cfg = 0;
    if (!eglChooseConfig(eglDpy, cfg_attribs, &config, 1, &num_cfg) || num_cfg == 0) {
        std::cerr << "eglChooseConfig failed: 0x" << std::hex << eglGetError() << "\n";
        return -1;
    }

    // -------------------------------
    // MATCH X11 VISUAL WITH EGL CONFIG
    // -------------------------------
    EGLint visual_id = 0;
    if (!eglGetConfigAttrib(eglDpy, config, EGL_NATIVE_VISUAL_ID, &visual_id)) {
        std::cerr << "eglGetConfigAttrib(EGL_NATIVE_VISUAL_ID) failed\n";
        return -1;
    }

    XVisualInfo visTemplate;
    visTemplate.visualid = visual_id;

    int nvis = 0;
    XVisualInfo* visInfo = XGetVisualInfo(xdisp, VisualIDMask, &visTemplate, &nvis);
    if (!visInfo) {
        std::cerr << "XGetVisualInfo failed\n";
        return -1;
    }

    Colormap cmap = XCreateColormap(
        xdisp,
        DefaultRootWindow(xdisp),
        visInfo->visual,
        AllocNone
    );

    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;

    Window win = XCreateWindow(
        xdisp,
        DefaultRootWindow(xdisp),
        0, 0, width, height,
        0,
        visInfo->depth,
        InputOutput,
        visInfo->visual,
        CWColormap | CWEventMask,
        &swa
    );

    XStoreName(xdisp, win, "EGL GLES 3.2 Example");
    XMapWindow(xdisp, win);

    XFree(visInfo);

    // -------------------------------
    // EGL CONTEXT + SURFACE
    // -------------------------------
    EGLSurface surf = eglCreateWindowSurface(eglDpy, config, (EGLNativeWindowType)win, nullptr);
    if (surf == EGL_NO_SURFACE) {
        std::cerr << "eglCreateWindowSurface failed: 0x" << std::hex << eglGetError() << "\n";
        return -1;
    }

    EGLint ctx_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_CONTEXT_MINOR_VERSION, 2,
        EGL_NONE
    };

    EGLContext ctx = eglCreateContext(eglDpy, config, EGL_NO_CONTEXT, ctx_attribs);
    if (ctx == EGL_NO_CONTEXT) {
        std::cerr << "eglCreateContext failed: 0x" << std::hex << eglGetError() << "\n";
        return -1;
    }

    if (!eglMakeCurrent(eglDpy, surf, surf, ctx)) {
        std::cerr << "eglMakeCurrent failed: 0x" << std::hex << eglGetError() << "\n";
        return -1;
    }

    std::cout << "GL_VERSION: " << glGetString(GL_VERSION) << "\n";

    // -------------------------------
    // SHADERS
    // -------------------------------
    GLuint vs = compileShader(GL_VERTEX_SHADER, vert_shader_src);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, frag_shader_src);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLint linked = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (!linked) {
        char log[512];
        glGetProgramInfoLog(prog, sizeof(log), nullptr, log);
        std::cerr << "LINK ERROR: " << log << "\n";
        return -1;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // -------------------------------
    // MAIN LOOP
    // -------------------------------
    bool running = true;
    while (running) {
        while (XPending(xdisp)) {
            XEvent ev;
            XNextEvent(xdisp, &ev);
            if (ev.type == DestroyNotify)
                running = false;
        }

        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        eglSwapBuffers(eglDpy, surf);
    }

    // Cleanup
    eglMakeCurrent(eglDpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(eglDpy, ctx);
    eglDestroySurface(eglDpy, surf);
    eglTerminate(eglDpy);

    XDestroyWindow(xdisp, win);
    XCloseDisplay(xdisp);
    return 0;
}
