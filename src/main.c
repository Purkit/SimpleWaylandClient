#include "glad/glad.h"
#include "internals/internal_api.h"
#include "keycodes.h"
#include "wlClient.h"
#include <assert.h>
#include <bits/time.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#include "wlClientState.h"
#include <math.h>
#include <sys/mman.h>
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
    /*xdg_surface_set_window_geometry(state->xdg_surface, 0, 0, state->width,
     * state->height);*/
    wl_egl_window_resize(state->egl_window, state->width, state->height, 0, 0);
    glViewport(0, 0, state->width, state->height);
    glScissor(0, 0, state->width, state->height);
}

static void draw_frame_gpu(struct WaylandClientContext *state, double now) {
    float r = 0.5f + 0.5f * sin(now * 0.5f);
    float g = 0.5f + 0.5f * sin(now * 0.7f);
    float b = 0.5f + 0.5f * sin(now * 0.9f);
    /*glClearColor(1.0f, 0.0f, 0.0f, 1.0f);*/
    glClearColor(r, g, b, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    eglSwapBuffers(state->egl_display, state->egl_surface);
}

static void key_event_callback(struct WaylandClientContext *state, KeyCode key,
                               KeyAction action) {
    if (action == KEY_PRESSED) {
        verbose("\nKey Pressed !");
    }
    if (action == KEY_RELEASED) {
        verbose("\nKey Released !");
    }
}

static void mouse_move_callback(struct WaylandClientContext *clientState,
                                float xpos, float ypos) {
    verbose("mouse moved: %f %f", xpos, ypos);
}

static void mouse_click_callback(struct WaylandClientContext *clientState,
                                 MouseButtonCode btn, KeyAction action) {
    if (action == KEY_PRESSED) {
        verbose("Click down!\n");
    }
    if (action == KEY_RELEASED) {
        verbose("Click up!\n");
    }
}

int main(int argc, char **argv) {

    WaylandClientContext wlClientState = {0};

    wlClientState.width  = 640;
    wlClientState.height = 480;

    wlClientState.callbacks.resize = on_resize;
    wlClientState.callbacks.render = draw_frame_gpu;
    // wlClientState.callbacks.key_event       = key_event_callback;
    // wlClientState.callbacks.mouse_motion    = mouse_move_callback;
    // wlClientState.callbacks.mouse_btn_event = mouse_click_callback;

    wayland_client_initialize(&wlClientState);

    egl_create_opengl_context(&wlClientState);

    gladLoadGLLoader((GLADloadproc)eglGetProcAddress);

    wayland_init_rendering(&wlClientState);

    wlClientState.shouldClose = false;

    float xpos, ypos;
    while (!wlClientState.shouldClose) {
        wayland_poll_events(&wlClientState);
        /*draw_frame_gpu(&wlClientState, posixGetTime_sec());*/
        if (getKeyState(&wlClientState, KEY_B) == PRESSED)
            verbose("KEY B Pressed!\n");
        if (isKeyPressed(&wlClientState, KEY_A))
            verbose("A Pressed!\n");
        if (isMouseButtonPressed(&wlClientState, MOUSE_BUTTON_LEFT))
            verbose("LMB !!\n");
    }

    wayland_client_shutdown(&wlClientState);
    verbose("All resources freed!\n");
    verbose("Quiting program!\n");
    return 0;
}
