#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <X11/Xlib.h>
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

GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader error: " << log << "\n";
        return 0;
    }
    return shader;
}

int main() {
    Display* xdisp = XOpenDisplay(nullptr);
    if (!xdisp) {
        std::cerr << "Cannot open X11 display\n";
        return -1;
    }

    int width = 800;
    int height = 600;
    Window root = DefaultRootWindow(xdisp);
    Window win = XCreateSimpleWindow(
        xdisp, root, 0, 0, width, height, 0,
        BlackPixel(xdisp, 0), WhitePixel(xdisp, 0)
    );

    XStoreName(xdisp, win, "EGL + GLES 3.2 Shader");
    XMapWindow(xdisp, win);

    // 3. EGL initialization
    EGLDisplay eglDpy = eglGetDisplay((EGLNativeDisplayType)xdisp);

    if (eglDpy == EGL_NO_DISPLAY) {
        std::cout << "eglGetDisplay failed: 0x" << std::hex << eglGetError() << "\n";
        return -1;
    }

	if (!eglInitialize(eglDpy, nullptr, nullptr)) {
        std::cout << "eglInitialize failed: 0x" << std::hex << eglGetError() << "\n";
        return -1;
    }

	if (!eglBindAPI(EGL_OPENGL_ES_API)) {
        std::cout << "eglBindAPI failed: 0x" << std::hex << eglGetError() << "\n";
        // continue — but this is likely fatal
    }

    EGLint cfg_attribs[] = {
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
	if (!eglChooseConfig(eglDpy, cfg_attribs, &cfg, 1, &num_cfg) || num_cfg == 0) {
        std::cout << "eglChooseConfig failed / no configs: 0x" << std::hex << eglGetError() << "\n";
        return -1;
    }

	EGLSurface surf = eglCreateWindowSurface(eglDpy, cfg, (EGLNativeWindowType)win, nullptr);
    if (surf == EGL_NO_SURFACE) {
        std::cout << "eglCreateWindowSurface failed: 0x" << std::hex << eglGetError() << "\n";
        return -1;
    }

    EGLint ctx_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
		EGL_CONTEXT_MINOR_VERSION, 2,
        EGL_NONE
    };
    EGLContext ctx = eglCreateContext(eglDpy, cfg, EGL_NO_CONTEXT, ctx_attribs);

	if (ctx == EGL_NO_CONTEXT) {
        std::cout << "eglCreateContext failed: 0x" << std::hex << eglGetError() << "\n";
        return -1;
    }

    if (!eglMakeCurrent(eglDpy, surf, surf, ctx)) {
        std::cout << "eglMakeCurrent failed: 0x" << std::hex << eglGetError() << "\n";
        return -1;
    }

	std::cout << "EGL vendor: " << eglQueryString(eglDpy, EGL_VENDOR) << "\n";
    std::cout << "GL_VERSION: " << (const char*)glGetString(GL_VERSION) << "\n";
    std::cout << "GL_RENDERER: " << (const char*)glGetString(GL_RENDERER) << "\n";
    std::cout << "GL_SHADING_LANGUAGE_VERSION: " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

	GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vs = compileShader(GL_VERTEX_SHADER, vert_shader_src);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, frag_shader_src);

	if (!vs || !fs) 
	{
    	std::cout << "Shader compilation failed, exiting.\n";
	}

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLint linked;
    glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (!linked) {
        char log[512];
        glGetProgramInfoLog(prog, 512, nullptr, log);
        std::cerr << "Link error: " << log << "\n";
        return -1;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    bool running = true;
    XEvent event;

    while (running) {
        while (XPending(xdisp)) {
            XNextEvent(xdisp, &event);
            if (event.type == DestroyNotify)
                running = false;
        }

        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);
        glDrawArrays(GL_TRIANGLES, 0, 3);

		if (!eglSwapBuffers(eglDpy, surf)) {
            std::cout << "eglSwapBuffers failed: 0x" << std::hex << eglGetError() << "\n";
        }
    }

    eglMakeCurrent(eglDpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroySurface(eglDpy, surf);
    eglDestroyContext(eglDpy, ctx);
    eglTerminate(eglDpy);

    XDestroyWindow(xdisp, win);
    XCloseDisplay(xdisp);

    return 0;
}
