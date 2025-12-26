#ifndef WAYLAND_CLIENT_STATE
#define WAYLAND_CLIENT_STATE

#include "internals/protocols/pointer-constraints-protocol.h"
#include "internals/types/events.h"
#include "keycodes.h"

#include <stdbool.h>

#include <EGL/egl.h>
#include <stdint.h>
#include <sys/types.h>

typedef enum WindowState {
    WINDOWED   = 0,
    MAXIMIZED  = 1,
    MINIMIZED  = 2,
    FULLSCREEN = 3,
    ACTIVATED  = 4,
    SUSPENDED  = 5,
    RESIZING   = 6
} WindowState;

struct WaylandClientContext;

typedef void (*fp_resize)(struct WaylandClientContext *clientState);
typedef void (*fp_render)(struct WaylandClientContext *clientState,
                          double time);
typedef void (*fp_keyEvent)(struct WaylandClientContext *clientState,
                            KeyCode key, KeyAction action);
typedef void (*fp_mouseBtnEvent)(struct WaylandClientContext *clientState,
                                 MouseButtonCode btn, KeyAction action);
typedef void (*fp_mouseMoveEvent)(struct WaylandClientContext *clientState,
                                  float xpos, float ypos);
typedef void (*fp_mouseScrollEvent)(struct WaylandClientContext *clientState,
                                    float amt, ScrollDir dir);
typedef void (*fp_windowFocusLoss)(struct WaylandClientContext *clientState);
typedef void (*fp_windowFocusGain)(struct WaylandClientContext *clientState);
typedef void (*fp_mouseEnter)(struct WaylandClientContext *clientState);
typedef void (*fp_mouseLeave)(struct WaylandClientContext *clientState);
typedef void (*fp_windowCloseBtnPress)(
    struct WaylandClientContext *clientState);
typedef void (*fp_windowMaximize)(struct WaylandClientContext *clientState);
typedef void (*fp_windowMinimize)(struct WaylandClientContext *clientState);

typedef struct PointerState {
    struct Vector2i {
        int x, y;
    } position, relative_motion, last_btn_press_position;
    uint32_t motion_timestamp;
    uint32_t relative_motion_timestamp;
    uint32_t last_button_press_timestamp;
    KeyState mouseButtonState[MOUSE_BUTTON_COUNT];
    MouseButtonCode last_pressed_button;
} PointerState;

typedef struct WaylandClientContext {
    // Globals and interfaces:
    struct wl_display *display;
    struct wl_registry *registry;

    struct wl_compositor *compositor;
    struct wl_surface *wl_surface;

    struct xdg_wm_base *xdg_wm_base;
    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;

    struct zxdg_decoration_manager_v1 *xdg_decoration_manager;
    struct zxdg_toplevel_decoration_v1 *xdg_toplevel_decoration;

    struct wl_seat *wl_seat;
    struct wl_keyboard *wl_keyboard;
    struct wl_pointer *wl_pointer;
    struct wl_touch *wl_touch;

    struct zwp_relative_pointer_manager_v1 *relative_pointer_manager;
    struct zwp_relative_pointer_v1 *relative_pointer;

    struct zwp_pointer_constraints_v1 *pointer_constraint_manager;
    struct zwp_locked_pointer_v1 *locaked_pointer;
    struct zwp_confined_pointer_v1 *confined_pointer;

    // State
    double last_frame_time;
    int width, height;
    bool shouldClose;
    bool canMaximize;
    bool canMinimize;
    bool canFullscreen;
    bool maximized;
    bool minimized;
    bool activated;
    bool fullscreen;
    bool isKeyRepeat_on;
    int32_t keyRepeat_rate;
    int32_t keyRepeat_delay;
    uint32_t last_keyPress_time;
    float current_mouse_x, current_mouse_y;
    KeyState mouseButtonState[MOUSE_BUTTON_COUNT];
    KeyState keyState[KEY_COUNT];

    struct wl_callback *redraw_signal_callback;
    struct pointer_event_accumulator_t accumulated_pointer_events;
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
        fp_resize resize;
        fp_render render;
        fp_keyEvent key_event;
        fp_mouseBtnEvent mouse_btn_event;
        fp_mouseMoveEvent mouse_motion;
        fp_windowCloseBtnPress window_close;
    } callbacks;

} WaylandClientContext;

#endif // ! WAYLAND_CLIENT_STATE
