#ifndef INPUT_H
#define INPUT_H

#include "wayland.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>
#include <xkbcommon/xkbcommon.h>

// #define DEBUG 1
#define VERBOSE 1
#include "utility.h"

enum pointer_event_mask {
    POINTER_EVENT_ENTER = 1 << 0,
    POINTER_EVENT_LEAVE = 1 << 1,
    POINTER_EVENT_MOTION = 1 << 2,
    POINTER_EVENT_BUTTON = 1 << 3,
    POINTER_EVENT_AXIS = 1 << 4,
    POINTER_EVENT_AXIS_SOURCE = 1 << 5,
    POINTER_EVENT_AXIS_STOP = 1 << 6,
    POINTER_EVENT_AXIS_DISCRETE = 1 << 7,
};

enum touch_event_mask {
    TOUCH_EVENT_DOWN = 1 << 0,
    TOUCH_EVENT_UP = 1 << 1,
    TOUCH_EVENT_MOTION = 1 << 2,
    TOUCH_EVENT_CANCEL = 1 << 3,
    TOUCH_EVENT_SHAPE = 1 << 4,
    TOUCH_EVENT_ORIENTATION = 1 << 5,
};

static void wl_pointer_enter(void *data, struct wl_pointer *wl_pointer,
                             uint32_t serial, struct wl_surface *surface,
                             wl_fixed_t surface_x, wl_fixed_t surface_y) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    client_state->pointer_event.event_mask |= POINTER_EVENT_ENTER;
    client_state->pointer_event.serial = serial;
    client_state->pointer_event.surface_x = surface_x,
    client_state->pointer_event.surface_y = surface_y;
}

static void wl_pointer_leave(void *data, struct wl_pointer *wl_pointer,
                             uint32_t serial, struct wl_surface *surface) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    client_state->pointer_event.serial = serial;
    client_state->pointer_event.event_mask |= POINTER_EVENT_LEAVE;
}

static void wl_pointer_motion(void *data, struct wl_pointer *wl_pointer,
                              uint32_t time, wl_fixed_t surface_x,
                              wl_fixed_t surface_y) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    client_state->pointer_event.event_mask |= POINTER_EVENT_MOTION;
    client_state->pointer_event.time = time;
    client_state->pointer_event.surface_x = surface_x,
    client_state->pointer_event.surface_y = surface_y;
}

static void wl_pointer_button(void *data, struct wl_pointer *wl_pointer,
                              uint32_t serial, uint32_t time, uint32_t button,
                              uint32_t state) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    client_state->pointer_event.event_mask |= POINTER_EVENT_BUTTON;
    client_state->pointer_event.time = time;
    client_state->pointer_event.serial = serial;
    client_state->pointer_event.button = button,
    client_state->pointer_event.state = state;
}

static void wl_pointer_axis(void *data, struct wl_pointer *wl_pointer,
                            uint32_t time, uint32_t axis, wl_fixed_t value) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    client_state->pointer_event.event_mask |= POINTER_EVENT_AXIS;
    client_state->pointer_event.time = time;
    client_state->pointer_event.axes[axis].valid = true;
    client_state->pointer_event.axes[axis].value = value;
}

static void wl_pointer_axis_source(void *data, struct wl_pointer *wl_pointer,
                                   uint32_t axis_source) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    client_state->pointer_event.event_mask |= POINTER_EVENT_AXIS_SOURCE;
    client_state->pointer_event.axis_source = axis_source;
}

static void wl_pointer_axis_stop(void *data, struct wl_pointer *wl_pointer,
                                 uint32_t time, uint32_t axis) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    client_state->pointer_event.time = time;
    client_state->pointer_event.event_mask |= POINTER_EVENT_AXIS_STOP;
    client_state->pointer_event.axes[axis].valid = true;
}

static void wl_pointer_axis_discrete(void *data, struct wl_pointer *wl_pointer,
                                     uint32_t axis, int32_t discrete) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    client_state->pointer_event.event_mask |= POINTER_EVENT_AXIS_DISCRETE;
    client_state->pointer_event.axes[axis].valid = true;
    client_state->pointer_event.axes[axis].discrete = discrete;
}

static void wl_pointer_high_resolution_axis_event(void *data,
                                                  struct wl_pointer *wl_pointer,
                                                  uint32_t axis,
                                                  int32_t value120) {

    // Print axis event information
    verbose("Axis value 120 event:\n");
    verbose("  Axis: %d\n", axis);
    verbose("  Value: %d\n", value120);

    // Handle horizontal scroll
    if (axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL) {
        // Calculate scroll amount
        int scroll_amount = value120 / 120;

        // Scroll horizontally
        verbose("Scrolling horizontally by %d units\n", scroll_amount);
        // Add code to handle horizontal scrolling
    }

    // Handle vertical scroll
    else if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL) {
        // Calculate scroll amount
        int scroll_amount = value120 / 120;

        // Scroll vertically
        verbose("Scrolling vertically by %d units\n", scroll_amount);
        // Add code to handle vertical scrolling
    }

    // Handle other axes (e.g., WL_POINTER_AXIS_LEFT_BTN,
    // WL_POINTER_AXIS_RIGHT_BTN)
    else {
        verbose("Unhandled axis event\n");
    }
}

static void wl_pointer_relative_direction_event(void *data,
                                                struct wl_pointer *wl_pointer,
                                                uint32_t axis,
                                                uint32_t direction) {
    // Print axis relative event information
    verbose("Axis relative event:\n");
    verbose("  Axis: %d\n", axis);
    verbose("  Direction: %d\n", direction);

    // Handle horizontal axis
    if (axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL) {
        if (direction > 0) {
            verbose("Moving right\n");
            // Add code to handle right movement
        } else if (direction < 0) {
            verbose("Moving left\n");
            // Add code to handle left movement
        }
    }

    // Handle vertical axis
    else if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL) {
        if (direction > 0) {
            verbose("Moving down\n");
            // Add code to handle down movement
        } else if (direction < 0) {
            verbose("Moving up\n");
            // Add code to handle up movement
        }
    }
}

static void wl_pointer_frame(void *data, struct wl_pointer *wl_pointer) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    struct pointer_event *event = &client_state->pointer_event;
    verbose("pointer frame @ %d: ", event->time);

    if (event->event_mask & POINTER_EVENT_ENTER) {
        verbose("entered %f, %f ", wl_fixed_to_double(event->surface_x),
                wl_fixed_to_double(event->surface_y));
    }

    if (event->event_mask & POINTER_EVENT_LEAVE) {
        verbose("leave");
    }

    if (event->event_mask & POINTER_EVENT_MOTION) {
        verbose("motion %f, %f ", wl_fixed_to_double(event->surface_x),
                wl_fixed_to_double(event->surface_y));
    }

    if (event->event_mask & POINTER_EVENT_BUTTON) {
        const char *state = event->state == WL_POINTER_BUTTON_STATE_RELEASED
                                ? "released"
                                : "pressed";
        verbose("button %d %s ", event->button, state);
    }

    uint32_t axis_events = POINTER_EVENT_AXIS | POINTER_EVENT_AXIS_SOURCE |
                           POINTER_EVENT_AXIS_STOP |
                           POINTER_EVENT_AXIS_DISCRETE;
    const char *axis_name[2] = {
        //[WL_POINTER_AXIS_VERTICAL_SCROLL] = "vertical",
        //[WL_POINTER_AXIS_HORIZONTAL_SCROLL] = "horizontal",
    };
    const char *axis_source[4] = {
        // [WL_POINTER_AXIS_SOURCE_WHEEL] = "wheel",
        // [WL_POINTER_AXIS_SOURCE_FINGER] = "finger",
        // [WL_POINTER_AXIS_SOURCE_CONTINUOUS] = "continuous",
        // [WL_POINTER_AXIS_SOURCE_WHEEL_TILT] = "wheel tilt",
    };
    if (event->event_mask & axis_events) {
        for (size_t i = 0; i < 2; ++i) {
            if (!event->axes[i].valid) {
                continue;
            }
            verbose("%s axis ", axis_name[i]);
            if (event->event_mask & POINTER_EVENT_AXIS) {
                verbose("value %f ", wl_fixed_to_double(event->axes[i].value));
            }
            if (event->event_mask & POINTER_EVENT_AXIS_DISCRETE) {
                verbose("discrete %d ", event->axes[i].discrete);
            }
            if (event->event_mask & POINTER_EVENT_AXIS_SOURCE) {
                verbose("via %s ", axis_source[event->axis_source]);
            }
            if (event->event_mask & POINTER_EVENT_AXIS_STOP) {
                verbose("(stopped) ");
            }
        }
    }

    verbose("\n");
    memset(event, 0, sizeof(*event));
}

static const struct wl_pointer_listener wl_pointer_listener = {
    .enter = wl_pointer_enter,
    .leave = wl_pointer_leave,
    .motion = wl_pointer_motion,
    .button = wl_pointer_button,
    .axis = wl_pointer_axis,
    .frame = wl_pointer_frame,
    .axis_source = wl_pointer_axis_source,
    .axis_stop = wl_pointer_axis_stop,
    .axis_discrete = wl_pointer_axis_discrete,
    .axis_value120 = wl_pointer_high_resolution_axis_event,
    .axis_relative_direction = wl_pointer_relative_direction_event,
};

static void wl_keyboard_keymap(void *data, struct wl_keyboard *wl_keyboard,
                               uint32_t format, int32_t fd, uint32_t size) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    assert(format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1);

    char *map_shm = (char *)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    assert(map_shm != MAP_FAILED);

    struct xkb_keymap *xkb_keymap = xkb_keymap_new_from_string(
        client_state->xkb_context, map_shm, XKB_KEYMAP_FORMAT_TEXT_V1,
        XKB_KEYMAP_COMPILE_NO_FLAGS);
    munmap(map_shm, size);
    close(fd);

    struct xkb_state *xkb_state = xkb_state_new(xkb_keymap);
    xkb_keymap_unref(client_state->xkb_keymap);
    xkb_state_unref(client_state->xkb_state);
    client_state->xkb_keymap = xkb_keymap;
    client_state->xkb_state = xkb_state;
}

static void wl_keyboard_enter(void *data, struct wl_keyboard *wl_keyboard,
                              uint32_t serial, struct wl_surface *surface,
                              struct wl_array *keys) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    verbose("keyboard enter; keys pressed are:\n");
    uint32_t *key;
    for (key = (uint32_t *)keys->data;
         (const char *)key < ((const char *)keys->data + keys->size); key++) {
        char buf[128];
        xkb_keysym_t sym =
            xkb_state_key_get_one_sym(client_state->xkb_state, *key + 8);
        xkb_keysym_get_name(sym, buf, sizeof(buf));
        verbose("sym: %-12s (%d), ", buf, sym);
        xkb_state_key_get_utf8(client_state->xkb_state, *key + 8, buf,
                               sizeof(buf));
        verbose("utf8: '%s'\n", buf);
    }
}

static void wl_keyboard_key(void *data, struct wl_keyboard *wl_keyboard,
                            uint32_t serial, uint32_t time, uint32_t key,
                            uint32_t state) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    char buf[128];
    uint32_t keycode = key + 8;
    xkb_keysym_t sym =
        xkb_state_key_get_one_sym(client_state->xkb_state, keycode);
    xkb_keysym_get_name(sym, buf, sizeof(buf));
    const char *action =
        state == WL_KEYBOARD_KEY_STATE_PRESSED ? "press" : "release";
    verbose("key %s: sym: %-12s (%d) [RAW: %d, +8: %d], ", action, buf, sym,
            key, keycode);
    xkb_state_key_get_utf8(client_state->xkb_state, keycode, buf, sizeof(buf));
    verbose("utf8: '%s'\n", buf);
}

static void wl_keyboard_leave(void *data, struct wl_keyboard *wl_keyboard,
                              uint32_t serial, struct wl_surface *surface) {
    verbose("keyboard leave\n");
}

static void wl_keyboard_modifiers(void *data, struct wl_keyboard *wl_keyboard,
                                  uint32_t serial, uint32_t mods_depressed,
                                  uint32_t mods_latched, uint32_t mods_locked,
                                  uint32_t group) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    xkb_state_update_mask(client_state->xkb_state, mods_depressed, mods_latched,
                          mods_locked, 0, 0, group);
}

static void wl_keyboard_repeat_info(void *data, struct wl_keyboard *wl_keyboard,
                                    int32_t rate, int32_t delay) {
    verbose("wl_keyboard::repeat_info event fired!\n");
}

static const struct wl_keyboard_listener wl_keyboard_listener = {
    .keymap = wl_keyboard_keymap,
    .enter = wl_keyboard_enter,
    .leave = wl_keyboard_leave,
    .key = wl_keyboard_key,
    .modifiers = wl_keyboard_modifiers,
    .repeat_info = wl_keyboard_repeat_info,
};

static struct touch_point *
get_touch_point(struct WaylandClientContext *client_state, int32_t id) {
    struct touch_event *touch = &client_state->touch_event;
    const size_t nmemb = sizeof(touch->points) / sizeof(struct touch_point);
    int invalid = -1;
    for (size_t i = 0; i < nmemb; ++i) {
        if (touch->points[i].id == id) {
            return &touch->points[i];
        }
        if (invalid == -1 && !touch->points[i].valid) {
            invalid = i;
        }
    }
    if (invalid == -1) {
        return NULL;
    }
    touch->points[invalid].valid = true;
    touch->points[invalid].id = id;
    return &touch->points[invalid];
}

static void wl_touch_down(void *data, struct wl_touch *wl_touch,
                          uint32_t serial, uint32_t time,
                          struct wl_surface *surface, int32_t id, wl_fixed_t x,
                          wl_fixed_t y) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    struct touch_point *point = get_touch_point(client_state, id);
    if (point == NULL) {
        return;
    }
    point->event_mask |= TOUCH_EVENT_UP;
    point->surface_x = wl_fixed_to_double(x),
    point->surface_y = wl_fixed_to_double(y);
    client_state->touch_event.time = time;
    client_state->touch_event.serial = serial;
}

static void wl_touch_up(void *data, struct wl_touch *wl_touch, uint32_t serial,
                        uint32_t time, int32_t id) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    struct touch_point *point = get_touch_point(client_state, id);
    if (point == NULL) {
        return;
    }
    point->event_mask |= TOUCH_EVENT_UP;
}

static void wl_touch_motion(void *data, struct wl_touch *wl_touch,
                            uint32_t time, int32_t id, wl_fixed_t x,
                            wl_fixed_t y) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    struct touch_point *point = get_touch_point(client_state, id);
    if (point == NULL) {
        return;
    }
    point->event_mask |= TOUCH_EVENT_MOTION;
    point->surface_x = x, point->surface_y = y;
    client_state->touch_event.time = time;
}

static void wl_touch_cancel(void *data, struct wl_touch *wl_touch) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    client_state->touch_event.event_mask |= TOUCH_EVENT_CANCEL;
}

static void wl_touch_shape(void *data, struct wl_touch *wl_touch, int32_t id,
                           wl_fixed_t major, wl_fixed_t minor) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    struct touch_point *point = get_touch_point(client_state, id);
    if (point == NULL) {
        return;
    }
    point->event_mask |= TOUCH_EVENT_SHAPE;
    point->major = major, point->minor = minor;
}

static void wl_touch_orientation(void *data, struct wl_touch *wl_touch,
                                 int32_t id, wl_fixed_t orientation) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    struct touch_point *point = get_touch_point(client_state, id);
    if (point == NULL) {
        return;
    }
    point->event_mask |= TOUCH_EVENT_ORIENTATION;
    point->orientation = orientation;
}

static void wl_touch_frame(void *data, struct wl_touch *wl_touch) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;
    struct touch_event *touch = &client_state->touch_event;
    const size_t nmemb = sizeof(touch->points) / sizeof(struct touch_point);
    verbose("touch event @ %d:\n", touch->time);

    for (size_t i = 0; i < nmemb; ++i) {
        struct touch_point *point = &touch->points[i];
        if (!point->valid) {
            continue;
        }
        verbose("point %d: ", touch->points[i].id);

        if (point->event_mask & TOUCH_EVENT_DOWN) {
            verbose("down %f,%f ", wl_fixed_to_double(point->surface_x),
                    wl_fixed_to_double(point->surface_y));
        }

        if (point->event_mask & TOUCH_EVENT_UP) {
            verbose("up ");
        }

        if (point->event_mask & TOUCH_EVENT_MOTION) {
            verbose("motion %f,%f ", wl_fixed_to_double(point->surface_x),
                    wl_fixed_to_double(point->surface_y));
        }

        if (point->event_mask & TOUCH_EVENT_SHAPE) {
            verbose("shape %fx%f ", wl_fixed_to_double(point->major),
                    wl_fixed_to_double(point->minor));
        }

        if (point->event_mask & TOUCH_EVENT_ORIENTATION) {
            verbose("orientation %f ", wl_fixed_to_double(point->orientation));
        }

        point->valid = false;
        verbose("\n");
    }
}

static const struct wl_touch_listener wl_touch_listener = {
    .down = wl_touch_down,
    .up = wl_touch_up,
    .motion = wl_touch_motion,
    .frame = wl_touch_frame,
    .cancel = wl_touch_cancel,
    .shape = wl_touch_shape,
    .orientation = wl_touch_orientation,
};

static void wl_seat_capabilities(void *data, struct wl_seat *wl_seat,
                                 uint32_t capabilities) {
    WaylandClientContext *client_state = (WaylandClientContext *)data;

    bool have_pointer = capabilities & WL_SEAT_CAPABILITY_POINTER;

    if (have_pointer && client_state->wl_pointer == NULL) {
        client_state->wl_pointer = wl_seat_get_pointer(client_state->wl_seat);
        wl_pointer_add_listener(client_state->wl_pointer, &wl_pointer_listener,
                                client_state);
    } else if (!have_pointer && client_state->wl_pointer != NULL) {
        wl_pointer_release(client_state->wl_pointer);
        client_state->wl_pointer = NULL;
    }

    bool have_keyboard = capabilities & WL_SEAT_CAPABILITY_KEYBOARD;

    if (have_keyboard && client_state->wl_keyboard == NULL) {
        client_state->wl_keyboard = wl_seat_get_keyboard(client_state->wl_seat);
        wl_keyboard_add_listener(client_state->wl_keyboard,
                                 &wl_keyboard_listener, client_state);
    } else if (!have_keyboard && client_state->wl_keyboard != NULL) {
        wl_keyboard_release(client_state->wl_keyboard);
        client_state->wl_keyboard = NULL;
    }

    bool have_touch = capabilities & WL_SEAT_CAPABILITY_TOUCH;

    if (have_touch && client_state->wl_touch == NULL) {
        client_state->wl_touch = wl_seat_get_touch(client_state->wl_seat);
        wl_touch_add_listener(client_state->wl_touch, &wl_touch_listener,
                              client_state);
    } else if (!have_touch && client_state->wl_touch != NULL) {
        wl_touch_release(client_state->wl_touch);
        client_state->wl_touch = NULL;
    }
}

static void wl_seat_name(void *data, struct wl_seat *wl_seat,
                         const char *name) {
    verbose("seat name: %s\n", name);
}

static const struct wl_seat_listener wl_seat_listener = {
    .capabilities = wl_seat_capabilities,
    .name = wl_seat_name,
};

#endif // INPUT_H
