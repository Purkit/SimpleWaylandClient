#ifndef WAYLAND_H
#define WAYLAND_H

#include "input/events.h"
#include <stdbool.h>

#include <EGL/egl.h>
#include <wayland-egl-backend.h>
#include <wayland-egl-core.h>
#include <wayland-egl.h>

#include <stdio.h>
// #define DEBUG 1
#define VERBOSE 1
#include "utility.h"

typedef struct WaylandClientContext {
        // Globals:
        struct wl_display *display;
        struct wl_registry *registry;
        struct wl_compositor *compositor;
        struct xdg_wm_base *xdg_wm_base;
        struct wl_seat *wl_seat;
        struct wl_shm *shm;

        // Interfaces:
        struct wl_surface *wl_surface;
        struct xdg_surface *xdg_surface;
        struct xdg_toplevel *xdg_toplevel;
        struct zxdg_decoration_manager_v1 *xdg_decoration_manager;
        struct zxdg_toplevel_decoration_v1 *xdg_toplevel_decoration;
        struct wl_keyboard *wl_keyboard;
        struct wl_pointer *wl_pointer;
        struct wl_touch *wl_touch;

        // State
        double last_frame;
        int width, height;
        bool shouldClose;
        struct wl_callback *redraw_signal_callback;
        struct pointer_event pointer_event;
        struct xkb_state *xkb_state;
        struct xkb_context *xkb_context;
        struct xkb_keymap *xkb_keymap;
        struct touch_event touch_event;

        // EGL Objects:
        EGLDisplay egl_display;
        EGLConfig egl_config;
        EGLContext egl_context;
        EGLSurface egl_surface;
        struct wl_egl_window *egl_window;

} WaylandClientContext;

int wayland_client_initialize(WaylandClientContext *wlClientState);
int egl_create_opengl_context(WaylandClientContext *clientState);

void wayland_client_shutdown(WaylandClientContext *wlClientState);

#endif // WAYLAND_H
