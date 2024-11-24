#ifndef TOUCH_CALLBACK
#define TOUCH_CALLBACK

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

enum touch_event_mask {
    TOUCH_EVENT_DOWN = 1 << 0,
    TOUCH_EVENT_UP = 1 << 1,
    TOUCH_EVENT_MOTION = 1 << 2,
    TOUCH_EVENT_CANCEL = 1 << 3,
    TOUCH_EVENT_SHAPE = 1 << 4,
    TOUCH_EVENT_ORIENTATION = 1 << 5,
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

#endif // !TOUCH_CALLBACK
#define TOUCH_CALLBACK
