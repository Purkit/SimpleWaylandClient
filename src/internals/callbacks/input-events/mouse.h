#ifndef MOUSE_CALLBACKS
#define MOUSE_CALLBACKS

#include "../../../utility.h"
#include "../../../wlClientState.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>
#include <xkbcommon/xkbcommon.h>

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

#endif // MOUSE_CALLBACKS
