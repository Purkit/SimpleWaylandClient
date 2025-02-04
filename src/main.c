#include "glad/glad.h"
#include "timer/timer_api.h"
#include "wlClient.h"
#include <assert.h>
#include <bits/time.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
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

static void draw_frame_gpu(struct WaylandClientContext *state, uint64_t now) {
    float r = 0.5f + 0.5f * sin(now * 0.5f);
    float g = 0.5f + 0.5f * sin(now * 0.7f);
    float b = 0.5f + 0.5f * sin(now * 0.9f);
    /*glClearColor(1.0f, 0.0f, 0.0f, 1.0f);*/
    glClearColor(r, g, b, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    eglSwapBuffers(state->egl_display, state->egl_surface);
}

int main(int argc, char **argv) {

    WaylandClientContext wlClientState = {0};

    wlClientState.width  = 640;
    wlClientState.height = 480;

    wlClientState.resize = on_resize;
    wlClientState.render = draw_frame_gpu;

    wayland_client_initialize(&wlClientState);

    egl_create_opengl_context(&wlClientState);

    gladLoadGLLoader((GLADloadproc)eglGetProcAddress);

    // wayland_init_rendering(&wlClientState);

    wlClientState.shouldClose = false;

    while (!wlClientState.shouldClose) {
        wayland_poll_events(&wlClientState);
        draw_frame_gpu(&wlClientState, posixGetTime_sec());
    }

    wayland_client_shutdown(&wlClientState);
    verbose("All resources freed!\n");
    verbose("Quiting program!\n");
    return 0;
}
