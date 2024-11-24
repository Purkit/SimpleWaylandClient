#include "protocols/xdg-decoration-protocol.h"
#include "protocols/xdg-shell-client-protocol.h"
#include <assert.h>
#include <bits/time.h>
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

#include "glad/glad.h"
#include "wlClient.h"

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

    float time_secs = time * 1e-3f;
    float elapsed = time_secs - state->last_frame;
    float fps = 1.0f / elapsed;
    verbose("dt=%f, fps=%f\n", elapsed, fps);

    draw_frame_gpu(state);
    wl_surface_commit(state->wl_surface);
    state->last_frame = time_secs;
}

static const struct wl_callback_listener wl_surface_frame_listener = {
    .done = wl_surface_frame_done,
};

int main(int argc, char **argv) {

    WaylandClientContext wlClientState = {0};
    wlClientState.width = 640;
    wlClientState.height = 480;
    // wlClientState.on_resize_callback = on_resize;
    // wlClientState.render_callback = draw_frame_gpu;

    wayland_client_initialize(&wlClientState);

    egl_create_opengl_context(&wlClientState);

    gladLoadGLLoader((GLADloadproc)eglGetProcAddress);

    // We need to manually call this function for once
    // in order to begin rendering.
    // TODO: Achieve this behivour by a InitRender() method.
    // wl_surface_frame_done(&wlClientState, NULL, 0);

    wlClientState.shouldClose = false;
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    wlClientState.last_frame = time.tv_nsec / 1.0e9f;
    while (!wlClientState.shouldClose) {

        wl_display_dispatch(
            wlClientState.display); // Waits for next event (Blocking)
        // wl_display_dispatch_pending(wlClientState.display); // Non-blocking
        // event polling
        draw_frame_gpu(&wlClientState);
        wl_surface_commit(wlClientState.wl_surface);
        clock_gettime(CLOCK_MONOTONIC, &time);
        double currentTime = time.tv_nsec / 1.0e9f;
        double elapsed = currentTime - wlClientState.last_frame;
        double fps = 1.0f / elapsed;
        verbose("dt=%f, fps=%f\n", elapsed, fps);
        wlClientState.last_frame = currentTime;
    }

exit:
    wayland_client_shutdown(&wlClientState);
    verbose("All resources freed!\n");
    verbose("Quiting program!\n");
    return 0;
}
