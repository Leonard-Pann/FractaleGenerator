// main_gbm.cpp
// Minimal GBM + EGL + GLES3 + KMS pageflip example
// Renders a solid blue triangle (full-screen) like your shader did.
// Requires libdrm, libgbm, EGL, GLESv2

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>

#include <gbm.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>

#include <xf86drm.h>
#include <xf86drmMode.h>

static const char* vert_shader_src =
"#version 300 es\n"
"precision highp float;\n"
"const vec2 verts[3] = vec2[3](\n"
"    vec2(-1.0, -1.0),\n"
"    vec2( 3.0, -1.0),\n"
"    vec2(-1.0,  3.0)\n"
");\n"
"void main() {\n"
"    gl_Position = vec4(verts[gl_VertexID], 0.0, 1.0);\n"
"}\n";

static const char* frag_shader_src =
"#version 300 es\n"
"precision highp float;\n"
"out vec4 outColor;\n"
"void main() {\n"
"    outColor = vec4(0.0, 0.5, 1.0, 1.0);\n"
"}\n";

static EGLDisplay get_platform_display_gbm(struct gbm_device* gbm) {
    // Try to get eglGetPlatformDisplayEXT first
    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT =
        (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
    if (eglGetPlatformDisplayEXT) {
        EGLDisplay dpy = eglGetPlatformDisplayEXT(EGL_PLATFORM_GBM_KHR, gbm, nullptr);
        return dpy;
    } else {
        // fallback
        return eglGetDisplay((EGLNativeDisplayType)gbm);
    }
}

static GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len+1);
        glGetShaderInfoLog(shader, len, nullptr, log.data());
        std::cerr << "Shader compile error: " << log.data() << "\n";
        return 0;
    }
    return shader;
}

struct drm_state {
    int drm_fd = -1;
    drmModeConnector* connector = nullptr;
    drmModeModeInfo mode;
    drmModeCrtc* saved_crtc = nullptr;
    uint32_t crtc_id = 0;
    uint32_t connector_id = 0;
};

static bool find_crtc_and_connector(int fd, drm_state &st) {
    drmModeRes* resources = drmModeGetResources(fd);
    if (!resources) return false;

    drmModeConnector* best_conn = nullptr;
    drmModeEncoder* enc = nullptr;
    drmModeCrtc* crtc = nullptr;
    drmModeConnector* connector = nullptr;

    // Find first connected connector
    for (int i = 0; i < resources->count_connectors; ++i) {
        connector = drmModeGetConnector(fd, resources->connectors[i]);
        if (!connector) continue;
        if (connector->connection == DRM_MODE_CONNECTED && connector->count_modes > 0) {
            best_conn = connector;
            break;
        }
        drmModeFreeConnector(connector);
    }
    if (!best_conn) {
        drmModeFreeResources(resources);
        std::cerr << "No connected connector found\n";
        return false;
    }

    // pick first mode (usually preferred mode is index 0)
    st.mode = best_conn->modes[0];
    st.connector_id = best_conn->connector_id;

    // find encoder/crtc
    drmModeEncoder* encoder = nullptr;
    if (best_conn->encoder_id)
        encoder = drmModeGetEncoder(fd, best_conn->encoder_id);

    if (encoder) {
        st.crtc_id = encoder->crtc_id;
        drmModeFreeEncoder(encoder);
    } else {
        // fallback: choose first crtc that's free (simple heuristic)
        st.crtc_id = resources->crtcs[0];
    }

    st.connector = best_conn;

    // store saved CRTC for restoring later
    st.saved_crtc = drmModeGetCrtc(fd, st.crtc_id);

    drmModeFreeResources(resources);
    return true;
}

static void cleanup_drm(drm_state &st) {
    if (st.saved_crtc) {
        drmModeFreeCrtc(st.saved_crtc);
        st.saved_crtc = nullptr;
    }
    if (st.connector) {
        drmModeFreeConnector(st.connector);
        st.connector = nullptr;
    }
    if (st.drm_fd >= 0) {
        close(st.drm_fd);
        st.drm_fd = -1;
    }
}

struct fb_info {
    uint32_t fb_id = 0;
    struct gbm_bo* bo = nullptr;
};

static int drm_find_plane_for_crtc(int fd, uint32_t crtc_id) {
    // optional, not used in minimal example
    (void)fd; (void)crtc_id;
    return -1;
}

int main() {
    drm_state st;
    // open drm device
    const char* card = "/dev/dri/card0";
    st.drm_fd = open(card, O_RDWR | O_CLOEXEC);
    if (st.drm_fd < 0) {
        perror("open /dev/dri/card0");
        return -1;
    }

    if (!find_crtc_and_connector(st.drm_fd, st)) {
        std::cerr << "Failed to find connector/crtc\n";
        cleanup_drm(st);
        return -1;
    }

    int width = st.mode.hdisplay;
    int height = st.mode.vdisplay;
    std::cout << "Using mode " << st.mode.hdisplay << "x" << st.mode.vdisplay
              << " @ " << st.mode.vrefresh << "Hz\n";

    // create GBM device
    struct gbm_device* gbm = gbm_create_device(st.drm_fd);
    if (!gbm) {
        std::cerr << "gbm_create_device failed\n";
        cleanup_drm(st);
        return -1;
    }

    // get EGL display for GBM platform
    // EGLDisplay eglDpy = get_platform_display_gbm(gbm);
    // if (eglDpy == EGL_NO_DISPLAY) {
    //     std::cerr << "eglGetPlatformDisplayEXT/eglGetDisplay failed: " << eglGetError() << "\n";
    //     gbm_device_destroy(gbm);
    //     cleanup_drm(st);
    //     return -1;
    // }

    EGLDisplay eglDpy = eglGetPlatformDisplay(EGL_PLATFORM_GBM_KHR, gbm, nullptr);

    if (!eglInitialize(eglDpy, nullptr, nullptr)) {
        std::cerr << "eglInitialize failed: " << eglGetError() << "\n";
        gbm_device_destroy(gbm);
        cleanup_drm(st);
        return -1;
    }

    if (!eglBindAPI(EGL_OPENGL_ES_API)) {
        std::cerr << "eglBindAPI failed: " << eglGetError() << " (continuing)\n";
    }

    // choose config
    EGLint cfg_attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 0,
        EGL_NONE
    };
    EGLConfig cfg;
    EGLint num_cfg = 0;
    if (!eglChooseConfig(eglDpy, cfg_attribs, &cfg, 1, &num_cfg) || num_cfg == 0) {
        std::cerr << "eglChooseConfig failed: " << eglGetError() << "\n";
        eglTerminate(eglDpy);
        gbm_device_destroy(gbm);
        cleanup_drm(st);
        return -1;
    }

    // create EGL context
    EGLint ctx_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    EGLContext ctx = eglCreateContext(eglDpy, cfg, EGL_NO_CONTEXT, ctx_attribs);
    if (ctx == EGL_NO_CONTEXT) {
        std::cerr << "eglCreateContext failed: " << eglGetError() << "\n";
        eglTerminate(eglDpy);
        gbm_device_destroy(gbm);
        cleanup_drm(st);
        return -1;
    }

    // create GBM surface (used as native window for EGL)
    uint32_t gbm_format = GBM_FORMAT_ARGB8888;
    uint32_t gbm_flags = GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING;

    struct gbm_surface* gbm_surf = gbm_surface_create(gbm, width, height, gbm_format, gbm_flags);
    if (!gbm_surf) {
        std::cerr << "gbm_surface_create failed\n";
        eglDestroyContext(eglDpy, ctx);
        eglTerminate(eglDpy);
        gbm_device_destroy(gbm);
        cleanup_drm(st);
        return -1;
    }

    // create EGL window surface from gbm_surface
    // Note: eglCreateWindowSurface accepts a GBM surface as native window on this platform.
    EGLSurface surf = eglCreateWindowSurface(eglDpy, cfg, (EGLNativeWindowType)gbm_surf, nullptr);
    if (surf == EGL_NO_SURFACE) {
        std::cerr << "eglCreateWindowSurface failed: " << eglGetError() << "\n";
        gbm_surface_destroy(gbm_surf);
        eglDestroyContext(eglDpy, ctx);
        eglTerminate(eglDpy);
        gbm_device_destroy(gbm);
        cleanup_drm(st);
        return -1;
    }

    if (!eglMakeCurrent(eglDpy, surf, surf, ctx)) {
        std::cerr << "eglMakeCurrent failed: " << eglGetError() << "\n";
        eglDestroySurface(eglDpy, surf);
        gbm_surface_destroy(gbm_surf);
        eglDestroyContext(eglDpy, ctx);
        eglTerminate(eglDpy);
        gbm_device_destroy(gbm);
        cleanup_drm(st);
        return -1;
    }

    std::cout << "GL_VERSION: " << (const char*)glGetString(GL_VERSION) << "\n";
    std::cout << "GL_RENDERER: " << (const char*)glGetString(GL_RENDERER) << "\n";
    std::cout << "GL_SHADING_LANGUAGE_VERSION: " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

    // simple GL setup: VAO (no VBO; using gl_VertexID)
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vs = compileShader(GL_VERTEX_SHADER, vert_shader_src);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, frag_shader_src);
    if (!vs || !fs) {
        std::cerr << "Shader compile failed\n";
        // cleanup
    }

    GLuint prog = glCreateProgram();
    if (!prog) {
        std::cerr << "glCreateProgram failed\n";
        return -1;
    }
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    GLint linked = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint len = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len+1);
        glGetProgramInfoLog(prog, len, nullptr, log.data());
        std::cerr << "Program link error: " << log.data() << "\n";
        return -1;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);

    // DRM/KMS helper state
    fb_info previous = {}; // previous fb/bo to release
    bool running = true;

    // Make sure DRM FD is non-blocking? we'll use select()
    // Render loop (render once per second for demo). Press Ctrl+C to exit.
    while (running) {
        // simple draw
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // swap EGL buffers -> this posts a buffer to GBM
        if (!eglSwapBuffers(eglDpy, surf)) {
            std::cerr << "eglSwapBuffers failed: " << eglGetError() << "\n";
            break;
        }

        // lock front buffer of gbm surface
        struct gbm_bo* bo = gbm_surface_lock_front_buffer(gbm_surf);
        if (!bo) {
            std::cerr << "gbm_surface_lock_front_buffer failed\n";
            break;
        }

        // uint32_t handle = gbm_bo_get_handle(bo).u32;
        // uint32_t stride = gbm_bo_get_stride(bo);
        // uint32_t width_b = width;
        // uint32_t height_b = height;

        // // create fb for that bo
        // uint32_t fb;
        // uint32_t handles[4] = { handle, 0, 0, 0 };
        // uint32_t strides[4] = { stride, 0, 0, 0 };
        // uint32_t offsets[4] = { 0, 0, 0, 0 };

        // int ret = drmModeAddFB2(st.drm_fd, width_b, height_b, gbm_format,
        //                          handles, strides, offsets, &fb, 0);


        uint32_t drm_format = gbm_bo_get_format(bo);
        uint64_t modifier   = gbm_bo_get_modifier(bo);
        uint32_t fb;
        uint32_t handles[4]  = { gbm_bo_get_handle(bo).u32, 0, 0, 0 };
        uint32_t strides[4]  = { gbm_bo_get_stride(bo),     0, 0, 0 };
        uint32_t offsets[4]  = { 0, 0, 0, 0 };
        uint64_t modifiers[4] = { modifier, 0, 0, 0 };

        int ret = drmModeAddFB2WithModifiers(
            st.drm_fd,
            width, height,
            drm_format,
            handles,
            strides,
            offsets,
            modifiers,
            &fb,
            DRM_MODE_FB_MODIFIERS
        );
        if (ret) {
            std::cerr << "drmModeAddFB2WithModifiers failed: "
                    << strerror(errno) << "\n";
            gbm_surface_release_buffer(gbm_surf, bo);
            break;
        }


        if (ret) {
            std::cerr << "drmModeAddFB2 failed: " << strerror(errno) << "\n";
            // release bo and continue
            gbm_surface_release_buffer(gbm_surf, bo);
            break;
        }

        // perform pageflip to show this fb on the chosen CRTC
        ret = drmModePageFlip(st.drm_fd, st.crtc_id, fb, gbm_format, (void*)(uintptr_t)fb);
        if (ret) {
            std::cerr << "drmModePageFlip failed: " << strerror(errno) << "\n";
            drmModeRmFB(st.drm_fd, fb);
            gbm_surface_release_buffer(gbm_surf, bo);
            break;
        }

        // Wait for pageflip event (we used DRM_MODE_PAGE_FLIP_EVENT)
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(st.drm_fd, &fds);
        // block until the event is delivered (or interrupted)
        if (select(st.drm_fd + 1, &fds, nullptr, nullptr, nullptr) < 0) {
            perror("select");
            // cleanup and exit
            drmModeRmFB(st.drm_fd, fb);
            gbm_surface_release_buffer(gbm_surf, bo);
            break;
        }

        // drain the event from DRM
        drmEventContext evctx = {};
        evctx.version = DRM_EVENT_CONTEXT_VERSION;
        // minimal handler: just call default
        evctx.page_flip_handler = [](int fd, unsigned int frame, unsigned int sec, unsigned int usec, void* data) {
            (void)fd; (void)frame; (void)sec; (void)usec;
            // data contains fb id
            // nothing needed; just return so select() unblocks
        };
        drmHandleEvent(st.drm_fd, &evctx);

        // release previous fb/bo if any
        if (previous.fb_id) {
            drmModeRmFB(st.drm_fd, previous.fb_id);
            if (previous.bo) {
                gbm_surface_release_buffer(gbm_surf, previous.bo);
            }
        }

        // store current as previous for next loop
        previous.fb_id = fb;
        previous.bo = bo;

        // In this minimal example we just render repeatedly once per second,
        // but you could render at vsync or as often as you need.
        usleep(16000); // ~60fps-ish (not strict)
    }

    // cleanup
    if (previous.fb_id) {
        drmModeRmFB(st.drm_fd, previous.fb_id);
        if (previous.bo) gbm_surface_release_buffer(gbm_surf, previous.bo);
    }

    if (st.saved_crtc && st.saved_crtc->crtc_id) {
        // Optionally restore saved CRTC
        drmModeSetCrtc(st.drm_fd, st.saved_crtc->crtc_id,
                       st.saved_crtc->buffer_id,
                       st.saved_crtc->x, st.saved_crtc->y,
                       &st.connector_id, 1, &st.saved_crtc->mode);
    }

    eglMakeCurrent(eglDpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroySurface(eglDpy, surf);
    eglDestroyContext(eglDpy, ctx);
    eglTerminate(eglDpy);

    gbm_surface_destroy(gbm_surf);
    gbm_device_destroy(gbm);
    cleanup_drm(st);

    return 0;
}
