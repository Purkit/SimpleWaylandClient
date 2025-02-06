#ifndef WAYLAND_CLIENT_STATE
#define WAYLAND_CLIENT_STATE

#include "internals/types/events.h"
#include "keycodes.h"

#include <stdbool.h>

#include <EGL/egl.h>
#include <stdint.h>

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
    double last_frame_time;
    int width, height;
    bool shouldClose;
    bool isKeyRepeat_on;
    int32_t keyRepeat_rate;
    int32_t keyRepeat_delay;
    uint32_t last_keyPress_time;
    float current_mouse_x, current_mouse_y;
    KeyState mouseButtonState[MOUSE_BUTTON_COUNT];
    KeyState keyState[KEY_COUNT];

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

    // Callbacks:
    struct {
        void (*resize)(struct WaylandClientContext *);
        void (*render)(struct WaylandClientContext *, double);
    } callbacks;

} WaylandClientContext;

#endif // ! WAYLAND_CLIENT_STATE
