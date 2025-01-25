#include "glad/glad.h"
#include "wlClient.h"
#include <assert.h>
#include <bits/time.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-egl-core.h>
#include <xkbcommon/xkbcommon.h>

// #define DEBUG 1
#define VERBOSE 1
#include "utility.h"

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

int main(int argc, char **argv) {

    WaylandClientContext wlClientState = {0};
    wlClientState.width = 640;
    wlClientState.height = 480;

    wlClientState.resize = on_resize;
    wlClientState.render = draw_frame_gpu;

    wayland_client_initialize(&wlClientState);

    egl_create_opengl_context(&wlClientState);

    gladLoadGLLoader((GLADloadproc)eglGetProcAddress);

    wayland_init_rendering(&wlClientState);

    wlClientState.shouldClose = false;
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    wlClientState.last_frame = time.tv_nsec / 1.0e9f;
    while (!wlClientState.shouldClose) {

        wl_display_dispatch(
            wlClientState.display); // Waits for next event (Blocking)
        // wl_display_dispatch_pending(wlClientState.display); // Non-blocking
        // event polling
    }

    wayland_client_shutdown(&wlClientState);
    verbose("All resources freed!\n");
    verbose("Quiting program!\n");
    return 0;
}
