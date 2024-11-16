#include "protocols/xdg-decoration-protocol.h"
#include "protocols/xdg-shell-client-protocol.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/input-event-codes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

#include <EGL/egl.h>
#include <wayland-egl-backend.h>
#include <wayland-egl-core.h>
#include <wayland-egl.h>

#include "glad/glad.h"
#include "input.h"

// #define DEBUG 1
#define VERBOSE 1
#include "utility.h"

static void randname(char *buf) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long r = ts.tv_nsec;
    for (int i = 0; i < 6; ++i) {
        buf[i] = 'A' + (r & 15) + (r & 16) * 2;
        r >>= 5;
    }
}

static int create_shm_file(void) {
    int retries = 100;
    do {
        char name[] = "/wl_shm-XXXXXX";
        randname(name + sizeof(name) - 7);
        --retries;
        int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
        if (fd >= 0) {
            shm_unlink(name);
            return fd;
        }
    } while (retries > 0 && errno == EEXIST);
    return -1;
}

static int allocate_shm_file(size_t size) {
    int fd = create_shm_file();
    if (fd < 0)
        return -1;
    int ret;
    do {
        ret = ftruncate(fd, size);
    } while (ret < 0 && errno == EINTR);
    if (ret < 0) {
        close(fd);
        return -1;
    }
    return fd;
}

static void wl_buffer_release_event_handler(void *data,
                                            struct wl_buffer *wl_buffer) {
    /* Sent by the compositor when it's no longer using this buffer */
    verbose("wl_buffer::release event recieved\n");
    wl_buffer_destroy(wl_buffer);
}

static const struct wl_buffer_listener wl_buffer_listener = {
    .release = wl_buffer_release_event_handler,
};

static struct wl_buffer *draw_frame_cpu(struct WaylandClientContext *state) {
    /*verbose("draw_frame_cpu called!\n");*/
    const int width = state->width;
    const int height = state->height;
    int stride = width * 4;
    int size = stride * height;

    int fd = allocate_shm_file(size);
    if (fd == -1) {
        return NULL;
    }

    uint32_t *data =
        mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        close(fd);
        return NULL;
    }

    struct wl_shm_pool *pool = wl_shm_create_pool(state->shm, fd, size);
    struct wl_buffer *buffer = wl_shm_pool_create_buffer(
        pool, 0, width, height, stride, WL_SHM_FORMAT_XRGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);

    /* Draw checkerboxed background */
    int offset = (int)state->offset % 8;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (((x + offset) + (y + offset) / 8 * 8) % 16 < 8)
                data[y * width + x] = 0xFF666666;
            else
                data[y * width + x] = 0xFFEEEEEE;
        }
    }

    munmap(data, size);
    wl_buffer_add_listener(buffer, &wl_buffer_listener, NULL);
    return buffer;
}

static void on_resize(struct WaylandClientContext *state) {
    wl_egl_window_resize(state->egl_window, state->width, state->height, 0, 0);
    glViewport(0, 0, state->width, state->height);
    glScissor(0, 0, state->width, state->height);
}

static void draw_frame_gpu(struct WaylandClientContext *state) {
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    eglSwapBuffers(state->egl_display, state->egl_surface);
}

static void xdg_surface_configure_event_handler(void *data,
                                                struct xdg_surface *xdg_surface,
                                                uint32_t serial) {
    verbose("xdg_surface::configure event recieved!\n");
    struct WaylandClientContext *state = data;
    xdg_surface_ack_configure(xdg_surface, serial);

    /*struct wl_buffer *buffer = draw_frame_cpu(state);
    wl_surface_attach(state->wl_surface, buffer, 0, 0);*/
    wl_surface_commit(state->wl_surface);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure_event_handler,
};

static void xdg_toplevel_configure_event_handler(
    void *data, struct xdg_toplevel *xdg_toplevel, int32_t width,
    int32_t height, struct wl_array *states) {
    verbose("xdg_toplevel::configure event recieved!\n");
    struct WaylandClientContext *state = data;
    state->width = width;
    state->height = height;
    on_resize(state);
}

static void
xdg_toplevel_close_event_handler(void *data,
                                 struct xdg_toplevel *xdg_toplevel) {
    verbose("close button clicked!\n");
    struct WaylandClientContext *state = data;
    state->shouldClose = true;
}

static void
wm_capabilities_broadcast_event_handler(void *data,
                                        struct xdg_toplevel *xdg_toplevel,
                                        struct wl_array *capabilities) {
    // Process the list of cabapilities supported by the compositor
    // and enable or disable the equivalent UI features.
    verbose("xdg_toplevel::wm_capabilities_broadcast event fired!\n");
}

static void
xdg_toplevel_configure_bounds_event_handler(void *data,
                                            struct xdg_toplevel *xdg_toplevel,
                                            int32_t width, int32_t height) {
    // TODO: handle later !
}

static const struct xdg_toplevel_listener xdg_toplevel_events_listener = {
    .configure = xdg_toplevel_configure_event_handler,
    .close = xdg_toplevel_close_event_handler,
    .configure_bounds = xdg_toplevel_configure_bounds_event_handler,
    .wm_capabilities = wm_capabilities_broadcast_event_handler,
};

static void xdg_wm_base_ping_handler(void *data,
                                     struct xdg_wm_base *xdg_wm_base,
                                     uint32_t serial) {
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping_handler,
};

static const struct wl_callback_listener wl_surface_frame_listener;

static void wl_surface_frame_done(void *data, struct wl_callback *cb,
                                  uint32_t time) {
    if (cb) {
        wl_callback_destroy(cb);
    }
    struct WaylandClientContext *state = data;
    state->redraw_signal_callback = wl_surface_frame(state->wl_surface);
    wl_callback_add_listener(state->redraw_signal_callback,
                             &wl_surface_frame_listener, state);
    /* Update scroll amount at 24 pixels per second */
    if (state->last_frame != 0) {
        int elapsed = time - state->last_frame;
        state->offset += elapsed / 1000.0 * 24;
    }

    /*
    struct wl_buffer *buffer = draw_frame_cpu(state);
    wl_surface_attach(state->wl_surface, buffer, 0, 0);
    wl_surface_damage_buffer(state->wl_surface, 0, 0, INT32_MAX, INT32_MAX);
    wl_surface_commit(state->wl_surface);
    */

    draw_frame_gpu(state);
    wl_surface_commit(state->wl_surface);
    state->last_frame = time;
}

static const struct wl_callback_listener wl_surface_frame_listener = {
    .done = wl_surface_frame_done,
};

static void registry_global_event_handler(void *data,
                                          struct wl_registry *registry,
                                          uint32_t name,
                                          const char *interface_string,
                                          uint32_t version) {
    WaylandClientContext *state = data;
    if (strcmp(interface_string, wl_compositor_interface.name) == 0) {
        state->compositor =
            wl_registry_bind(registry, name, &wl_compositor_interface, 6);
    } else if (strcmp(interface_string, wl_shm_interface.name) == 0) {
        state->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    } else if (strcmp(interface_string, xdg_wm_base_interface.name) == 0) {
        state->xdg_wm_base =
            wl_registry_bind(registry, name, &xdg_wm_base_interface, 6);
        xdg_wm_base_add_listener(state->xdg_wm_base, &xdg_wm_base_listener,
                                 state);
    } else if (strcmp(interface_string, wl_seat_interface.name) == 0) {
        state->wl_seat =
            wl_registry_bind(registry, name, &wl_seat_interface, 9);
        wl_seat_add_listener(state->wl_seat, &wl_seat_listener, state);
    } else if (strcmp(interface_string,
                      zxdg_decoration_manager_v1_interface.name) == 0) {
        state->xdg_decoration_manager = wl_registry_bind(
            registry, name, &zxdg_decoration_manager_v1_interface, 1);
    }
}

static void registry_global_remove(void *data, struct wl_registry *registry,
                                   uint32_t name) {
    // handle this
}

static const struct wl_registry_listener registry_listener = {
    .global = registry_global_event_handler,
    .global_remove = registry_global_remove,
};

static const EGLint egl_attrib_list[] = {
    EGL_RED_SIZE,
    8,
    EGL_GREEN_SIZE,
    8,
    EGL_BLUE_SIZE,
    8,
    EGL_ALPHA_SIZE,
    8,
    EGL_BUFFER_SIZE,
    32,
    EGL_DEPTH_SIZE,
    0,
    EGL_STENCIL_SIZE,
    0,
    EGL_SAMPLES,
    0,
    EGL_SURFACE_TYPE,
    EGL_WINDOW_BIT,
    EGL_RENDERABLE_TYPE,
    EGL_OPENGL_BIT,
    EGL_CONFIG_CAVEAT,
    EGL_NONE,
    EGL_MAX_SWAP_INTERVAL,
    1,
    EGL_NONE,
};

int main(int argc, char **argv) {
    WaylandClientContext wlClientState = {0};
    wlClientState.width = 640;
    wlClientState.height = 480;

    wlClientState.display = wl_display_connect(NULL);
    if (!wlClientState.display) {
        verbose("Failed to connect to the wayland compositor!\n");
        return 1;
    }
    wlClientState.registry = wl_display_get_registry(wlClientState.display);
    wlClientState.xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    wl_registry_add_listener(wlClientState.registry, &registry_listener,
                             &wlClientState);
    wl_display_roundtrip(wlClientState.display);

    wlClientState.wl_surface =
        wl_compositor_create_surface(wlClientState.compositor);
    wlClientState.xdg_surface = xdg_wm_base_get_xdg_surface(
        wlClientState.xdg_wm_base, wlClientState.wl_surface);

    xdg_surface_add_listener(wlClientState.xdg_surface, &xdg_surface_listener,
                             &wlClientState);
    wlClientState.xdg_toplevel =
        xdg_surface_get_toplevel(wlClientState.xdg_surface);
    xdg_toplevel_add_listener(wlClientState.xdg_toplevel,
                              &xdg_toplevel_events_listener, &wlClientState);
    xdg_toplevel_set_title(wlClientState.xdg_toplevel, "hello wayland");
    wlClientState.xdg_toplevel_decoration =
        zxdg_decoration_manager_v1_get_toplevel_decoration(
            wlClientState.xdg_decoration_manager, wlClientState.xdg_toplevel);
    wl_surface_commit(wlClientState.wl_surface);

    /*struct wl_callback *cb = wl_surface_frame(wlClientState.wl_surface);
    wl_callback_add_listener(cb, &wl_surface_frame_listener, &wlClientState);*/

    verbose("Trying to initialize EGL!\n");
    // Step 1: Initialize egl
    wlClientState.egl_display =
        eglGetDisplay((EGLNativeDisplayType)wlClientState.display);
    EGLint egl_major, egl_minor;
    eglInitialize(wlClientState.egl_display, &egl_major, &egl_minor);
    verbose("Step 1 passed!\n");

    // Step 2: Select an appropriate configuration
    EGLint no_of_matching_configs;
    if (!eglChooseConfig(wlClientState.egl_display, egl_attrib_list,
                         &wlClientState.egl_config, 1,
                         &no_of_matching_configs)) {
        ERROR("failed to get valid egl config!\n");
    }
    if (no_of_matching_configs == 0) {
        ERROR("No matching egl config found!");
        ERROR("Failed to obtain OpenGL context!");
        goto exit;
    }
    verbose("Step 2 passed!\n");

    // Step 3: Create a EGL Surface
    wlClientState.egl_window = wl_egl_window_create(
        wlClientState.wl_surface, wlClientState.width, wlClientState.height);
    const EGLint egl_surface_attrib_list[] = {
        EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_LINEAR,
        EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
        EGL_NONE,
    };
    wlClientState.egl_surface = eglCreateWindowSurface(
        wlClientState.egl_display, wlClientState.egl_config,
        (EGLNativeWindowType)wlClientState.egl_window, egl_surface_attrib_list);
    verbose("Step 3 passed!\n");

    // Step 4: Bind the OpenGL API
    eglBindAPI(EGL_OPENGL_API);
    verbose("Step 4 passed!\n");

    // Step 5: Create a OpenGL Context and make it current
    const EGLint opengl_context_attribs[] = {
        EGL_CONTEXT_MAJOR_VERSION,
        4,
        EGL_CONTEXT_MINOR_VERSION,
        6,
        EGL_CONTEXT_OPENGL_PROFILE_MASK,
        EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
        EGL_NONE,
    };
    wlClientState.egl_context =
        eglCreateContext(wlClientState.egl_display, wlClientState.egl_config,
                         EGL_NO_CONTEXT, opengl_context_attribs);
    eglMakeCurrent(wlClientState.egl_display, wlClientState.egl_surface,
                   wlClientState.egl_surface, wlClientState.egl_context);
    verbose("Step 5 passed!\n");

    gladLoadGLLoader((GLADloadproc)eglGetProcAddress);
    verbose("glad loaded gl Symbols!\n");

    const char *gl_query = (const char *)glGetString(GL_VERSION);
    printf("\nGL_VERSION: %s", gl_query);
    gl_query = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    printf("GL_SHADING_LANGUAGE_VERSION: %s", gl_query);
    gl_query = (const char *)glGetString(GL_VENDOR);
    printf("GL_VENDOR: %s", gl_query);
    gl_query = (const char *)glGetString(GL_RENDERER);
    printf("GL_RENDERER: %s", gl_query);

    // We need to manually call this function for once
    // in order to begin rendering.
    // TODO: Achieve this behivour by a InitRender() method.
    wl_surface_frame_done(&wlClientState, NULL, 0);

    wlClientState.shouldClose = false;
    while (!wlClientState.shouldClose) {

        wl_display_dispatch(
            wlClientState.display); // Waits for next event (Blocking)
        // wl_display_dispatch_pending(wlClientState.display); // Non-blocking
        // event polling
    }

exit:
    eglMakeCurrent(wlClientState.egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE,
                   EGL_NO_CONTEXT);
    eglDestroyContext(wlClientState.egl_display, wlClientState.egl_context);
    eglDestroySurface(wlClientState.egl_display, wlClientState.egl_surface);
    wl_egl_window_destroy(wlClientState.egl_window);
    eglTerminate(wlClientState.egl_display);

    zxdg_toplevel_decoration_v1_destroy(wlClientState.xdg_toplevel_decoration);
    zxdg_decoration_manager_v1_destroy(wlClientState.xdg_decoration_manager);
    xdg_toplevel_destroy(wlClientState.xdg_toplevel);
    xdg_surface_destroy(wlClientState.xdg_surface);
    xdg_wm_base_destroy(wlClientState.xdg_wm_base);
    wl_surface_destroy(wlClientState.wl_surface);
    if (wlClientState.wl_keyboard != NULL) {
        wl_keyboard_release(wlClientState.wl_keyboard);
    }
    if (wlClientState.wl_pointer != NULL) {
        wl_pointer_release(wlClientState.wl_pointer);
    }
    if (wlClientState.wl_touch != NULL) {
        wl_touch_release(wlClientState.wl_touch);
    }
    wl_seat_release(wlClientState.wl_seat);
    wl_display_disconnect(wlClientState.display);
    verbose("All resources freed!\n");
    verbose("Quiting program!\n");
    return 0;
}
