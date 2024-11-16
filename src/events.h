#ifndef LOW_LEVEL_EVENTS_H
#define LOW_LEVEL_EVENTS_H

#include <stdint.h>
#include <wayland-util.h>

struct pointer_event {
        uint32_t event_mask;
        wl_fixed_t surface_x, surface_y;
        uint32_t button, state;
        uint32_t time;
        uint32_t serial;
        struct {
                bool valid;
                wl_fixed_t value;
                int32_t discrete;
        } axes[2];
        uint32_t axis_source;
};

struct touch_point {
        bool valid;
        int32_t id;
        uint32_t event_mask;
        wl_fixed_t surface_x, surface_y;
        wl_fixed_t major, minor;
        wl_fixed_t orientation;
};

struct touch_event {
        uint32_t event_mask;
        uint32_t time;
        uint32_t serial;
        struct touch_point points[10];
};

#endif // !LOW_LEVEL_EVENTS_H
