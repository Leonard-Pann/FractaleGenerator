#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <iostream>
#include <string>

static wl_display* display = nullptr;
static wl_registry* registry = nullptr;
static wl_compositor* compositor = nullptr;
static wl_surface* surface = nullptr;
static wl_egl_window* egl_window = nullptr;

static int win_width = 800, win_height = 600;

// Wayland registry handler
static void registry_global(void*, wl_registry* reg, uint32_t name, const char* interface, uint32_t)
{
    std::string interface_str(interface);
    if (interface_str == "wl_compositor")
        compositor = (wl_compositor*)wl_registry_bind(reg, name, &wl_compositor_interface, 4);
}

static const wl_registry_listener registry_listener = {
    .global = registry_global,
    .global_remove = nullptr
};

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

GLuint compileShader(GLenum type, const char* src)
{
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);

    GLint ok;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(sh, 512, nullptr, log);
        std::cerr << "Shader error: " << log << "\n";
        return 0;
    }
    return sh;
}

int main()
{
    display = wl_display_connect(nullptr);
    if (!display) {
        std::cerr << "Could not connect to Wayland display\n";
        return -1;
    }

    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, nullptr);
    wl_display_roundtrip(display);

    if (!compositor) {
        std::cerr << "No wl_compositor\n";
        return -1;
    }

    surface = wl_compositor_create_surface(compositor);
    if (!surface) {
        std::cerr << "Could not create wl_surface\n";
        return -1;
    }

    egl_window = wl_egl_window_create(surface, win_width, win_height);
    if (!egl_window) {
        std::cerr << "wl_egl_window_create failed\n";
        return -1;
    }

    EGLDisplay eglDpy = eglGetDisplay((EGLNativeDisplayType)display);
    if (eglDpy == EGL_NO_DISPLAY) {
        std::cerr << "eglGetDisplay failed: 0x" << std::hex << eglGetError() << "\n";
        return -1;
    }

    eglInitialize(eglDpy, nullptr, nullptr);
    eglBindAPI(EGL_OPENGL_ES_API);

    EGLint cfg_attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_RED_SIZE,        8,
        EGL_GREEN_SIZE,      8,
        EGL_BLUE_SIZE,       8,
        EGL_NONE
    };

    EGLConfig cfg;
    EGLint num_cfg;
    eglChooseConfig(eglDpy, cfg_attribs, &cfg, 1, &num_cfg);

    EGLSurface eglSurf =
        eglCreateWindowSurface(eglDpy, cfg, (EGLNativeWindowType)egl_window, nullptr);

    EGLint ctx_attr[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    EGLContext ctx = eglCreateContext(eglDpy, cfg, EGL_NO_CONTEXT, ctx_attr);

    eglMakeCurrent(eglDpy, eglSurf, eglSurf, ctx);

    GLuint vs = compileShader(GL_VERTEX_SHADER, vert_shader_src);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, frag_shader_src);

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

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    while (wl_display_dispatch_pending(display) != -1)
    {
        glViewport(0, 0, win_width, win_height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        eglSwapBuffers(eglDpy, eglSurf);
    }

    return 0;
}
