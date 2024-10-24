#include "protocols/xdg-decoration-protocol.h"
#include "protocols/xdg-shell-client-protocol.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/input-event-codes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

#define DEBUG 1
#define VERBOSE 1
#include "utility.h"

static void randname(char *buf) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long r = ts.tv_nsec;
    for (int i = 0; i < 6; ++i) {
        buf[i] = 'A' + (r & 15) + (r & 16) * 2;
        r >>= 5;
    }
}

static int create_shm_file(void) {
    int retries = 100;
    do {
        char name[] = "/wl_shm-XXXXXX";
        randname(name + sizeof(name) - 7);
        --retries;
        int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
        if (fd >= 0) {
            shm_unlink(name);
            return fd;
        }
    } while (retries > 0 && errno == EEXIST);
    return -1;
}

static int allocate_shm_file(size_t size) {
    int fd = create_shm_file();
    if (fd < 0)
        return -1;
    int ret;
    do {
        ret = ftruncate(fd, size);
    } while (ret < 0 && errno == EINTR);
    if (ret < 0) {
        close(fd);
        return -1;
    }
    return fd;
}

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

enum touch_event_mask {
    TOUCH_EVENT_DOWN = 1 << 0,
    TOUCH_EVENT_UP = 1 << 1,
    TOUCH_EVENT_MOTION = 1 << 2,
    TOUCH_EVENT_CANCEL = 1 << 3,
    TOUCH_EVENT_SHAPE = 1 << 4,
    TOUCH_EVENT_ORIENTATION = 1 << 5,
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

typedef struct wayland_client_state {
        // Globals:
        struct wl_display *display;
        struct wl_registry *registry;
        struct wl_shm *shm;
        struct wl_compositor *compositor;
        struct xdg_wm_base *xdg_wm_base;
        struct wl_seat *wl_seat;
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
        float offset;
        uint32_t last_frame;
        int width, height;
        bool shouldClose;
        struct pointer_event pointer_event;
        struct xkb_state *xkb_state;
        struct xkb_context *xkb_context;
        struct xkb_keymap *xkb_keymap;
        struct touch_event touch_event;
} wayland_client_state;

static void wl_buffer_release_event_handler(void *data,
                                            struct wl_buffer *wl_buffer) {
    /* Sent by the compositor when it's no longer using this buffer */
    verbose("wl_buffer::release event recieved\n");
    wl_buffer_destroy(wl_buffer);
}

static const struct wl_buffer_listener wl_buffer_listener = {
    .release = wl_buffer_release_event_handler,
};

static struct wl_buffer *draw_frame(struct wayland_client_state *state) {
    /*verbose("draw_frame called!\n");*/
    const int width = state->width;
    const int height = state->height;
    int stride = width * 4;
    int size = stride * height;

    int fd = allocate_shm_file(size);
    if (fd == -1) {
        return NULL;
    }

    uint32_t *data =
        mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        close(fd);
        return NULL;
    }

    struct wl_shm_pool *pool = wl_shm_create_pool(state->shm, fd, size);
    struct wl_buffer *buffer = wl_shm_pool_create_buffer(
        pool, 0, width, height, stride, WL_SHM_FORMAT_XRGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);

    /* Draw checkerboxed background */
    int offset = (int)state->offset % 8;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (((x + offset) + (y + offset) / 8 * 8) % 16 < 8)
                data[y * width + x] = 0xFF666666;
            else
                data[y * width + x] = 0xFFEEEEEE;
        }
    }

    munmap(data, size);
    wl_buffer_add_listener(buffer, &wl_buffer_listener, NULL);
    return buffer;
}
static void xdg_surface_configure_event_handler(void *data,
                                                struct xdg_surface *xdg_surface,
                                                uint32_t serial) {
    verbose("xdg_surface::configure event recieved!\n");
    struct wayland_client_state *state = data;
    xdg_surface_ack_configure(xdg_surface, serial);

    struct wl_buffer *buffer = draw_frame(state);
    wl_surface_attach(state->wl_surface, buffer, 0, 0);
    wl_surface_commit(state->wl_surface);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure_event_handler,
};

static void xdg_toplevel_configure_event_handler(
    void *data, struct xdg_toplevel *xdg_toplevel, int32_t width,
    int32_t height, struct wl_array *states) {
    verbose("xdg_toplevel::configure event recieved!\n");
    // struct wayland_client_state *state = data;
    // state->width = width;
    // state->height = height;
}

static void
xdg_toplevel_close_event_handler(void *data,
                                 struct xdg_toplevel *xdg_toplevel) {
    verbose("close button clicked!\n");
    struct wayland_client_state *state = data;
    state->shouldClose = true;
}

static void
wm_capabilities_broadcast_event_handler(void *data,
                                        struct xdg_toplevel *xdg_toplevel,
                                        struct wl_array *capabilities) {
    // Process the list of cabapilities supported by the compositor
    // and enable or disable the equivalent UI features.
    verbose("xdg_toplevel::wm_capabilities_broadcast event fired!\n");
}

static void
xdg_toplevel_configure_bounds_event_handler(void *data,
                                            struct xdg_toplevel *xdg_toplevel,
                                            int32_t width, int32_t height) {
    // TODO: handle later !
}

static const struct xdg_toplevel_listener xdg_toplevel_events_listener = {
    .configure = xdg_toplevel_configure_event_handler,
    .close = xdg_toplevel_close_event_handler,
    .configure_bounds = xdg_toplevel_configure_bounds_event_handler,
    .wm_capabilities = wm_capabilities_broadcast_event_handler,
};

static void xdg_wm_base_ping_handler(void *data,
                                     struct xdg_wm_base *xdg_wm_base,
                                     uint32_t serial) {
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping_handler,
};

static const struct wl_callback_listener wl_surface_frame_listener;

static void wl_surface_frame_done(void *data, struct wl_callback *cb,
                                  uint32_t time) {
    /* Destroy this callback */
    wl_callback_destroy(cb);
    /* Request another frame */
    struct wayland_client_state *state = data;
    cb = wl_surface_frame(state->wl_surface);
    wl_callback_add_listener(cb, &wl_surface_frame_listener, state);
    /* Update scroll amount at 24 pixels per second */
    if (state->last_frame != 0) {
        int elapsed = time - state->last_frame;
        state->offset += elapsed / 1000.0 * 24;
    }

    /* Submit a frame for this event */
    struct wl_buffer *buffer = draw_frame(state);
    wl_surface_attach(state->wl_surface, buffer, 0, 0);
    wl_surface_damage_buffer(state->wl_surface, 0, 0, INT32_MAX, INT32_MAX);
    wl_surface_commit(state->wl_surface);

    state->last_frame = time;
}

static const struct wl_callback_listener wl_surface_frame_listener = {
    .done = wl_surface_frame_done,
};

static void wl_pointer_enter(void *data, struct wl_pointer *wl_pointer,
                             uint32_t serial, struct wl_surface *surface,
                             wl_fixed_t surface_x, wl_fixed_t surface_y) {
    struct wayland_client_state *client_state = data;
    client_state->pointer_event.event_mask |= POINTER_EVENT_ENTER;
    client_state->pointer_event.serial = serial;
    client_state->pointer_event.surface_x = surface_x,
    client_state->pointer_event.surface_y = surface_y;
}

static void wl_pointer_leave(void *data, struct wl_pointer *wl_pointer,
                             uint32_t serial, struct wl_surface *surface) {
    struct wayland_client_state *client_state = data;
    client_state->pointer_event.serial = serial;
    client_state->pointer_event.event_mask |= POINTER_EVENT_LEAVE;
}

static void wl_pointer_motion(void *data, struct wl_pointer *wl_pointer,
                              uint32_t time, wl_fixed_t surface_x,
                              wl_fixed_t surface_y) {
    struct wayland_client_state *client_state = data;
    client_state->pointer_event.event_mask |= POINTER_EVENT_MOTION;
    client_state->pointer_event.time = time;
    client_state->pointer_event.surface_x = surface_x,
    client_state->pointer_event.surface_y = surface_y;
}

static void wl_pointer_button(void *data, struct wl_pointer *wl_pointer,
                              uint32_t serial, uint32_t time, uint32_t button,
                              uint32_t state) {
    struct wayland_client_state *client_state = data;
    client_state->pointer_event.event_mask |= POINTER_EVENT_BUTTON;
    client_state->pointer_event.time = time;
    client_state->pointer_event.serial = serial;
    client_state->pointer_event.button = button,
    client_state->pointer_event.state = state;
}

static void wl_pointer_axis(void *data, struct wl_pointer *wl_pointer,
                            uint32_t time, uint32_t axis, wl_fixed_t value) {
    struct wayland_client_state *client_state = data;
    client_state->pointer_event.event_mask |= POINTER_EVENT_AXIS;
    client_state->pointer_event.time = time;
    client_state->pointer_event.axes[axis].valid = true;
    client_state->pointer_event.axes[axis].value = value;
}

static void wl_pointer_axis_source(void *data, struct wl_pointer *wl_pointer,
                                   uint32_t axis_source) {
    struct wayland_client_state *client_state = data;
    client_state->pointer_event.event_mask |= POINTER_EVENT_AXIS_SOURCE;
    client_state->pointer_event.axis_source = axis_source;
}

static void wl_pointer_axis_stop(void *data, struct wl_pointer *wl_pointer,
                                 uint32_t time, uint32_t axis) {
    struct wayland_client_state *client_state = data;
    client_state->pointer_event.time = time;
    client_state->pointer_event.event_mask |= POINTER_EVENT_AXIS_STOP;
    client_state->pointer_event.axes[axis].valid = true;
}

static void wl_pointer_axis_discrete(void *data, struct wl_pointer *wl_pointer,
                                     uint32_t axis, int32_t discrete) {
    struct wayland_client_state *client_state = data;
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
    struct wayland_client_state *client_state = data;
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
        char *state = event->state == WL_POINTER_BUTTON_STATE_RELEASED
                          ? "released"
                          : "pressed";
        verbose("button %d %s ", event->button, state);
    }

    uint32_t axis_events = POINTER_EVENT_AXIS | POINTER_EVENT_AXIS_SOURCE |
                           POINTER_EVENT_AXIS_STOP |
                           POINTER_EVENT_AXIS_DISCRETE;
    char *axis_name[2] = {
        [WL_POINTER_AXIS_VERTICAL_SCROLL] = "vertical",
        [WL_POINTER_AXIS_HORIZONTAL_SCROLL] = "horizontal",
    };
    char *axis_source[4] = {
        [WL_POINTER_AXIS_SOURCE_WHEEL] = "wheel",
        [WL_POINTER_AXIS_SOURCE_FINGER] = "finger",
        [WL_POINTER_AXIS_SOURCE_CONTINUOUS] = "continuous",
        [WL_POINTER_AXIS_SOURCE_WHEEL_TILT] = "wheel tilt",
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
    struct wayland_client_state *client_state = data;
    assert(format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1);

    char *map_shm = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
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
    struct wayland_client_state *client_state = data;
    verbose("keyboard enter; keys pressed are:\n");
    uint32_t *key;
    wl_array_for_each(key, keys) {
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
    struct wayland_client_state *client_state = data;
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
    struct wayland_client_state *client_state = data;
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
get_touch_point(struct wayland_client_state *client_state, int32_t id) {
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
    struct wayland_client_state *client_state = data;
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
    struct wayland_client_state *client_state = data;
    struct touch_point *point = get_touch_point(client_state, id);
    if (point == NULL) {
        return;
    }
    point->event_mask |= TOUCH_EVENT_UP;
}

static void wl_touch_motion(void *data, struct wl_touch *wl_touch,
                            uint32_t time, int32_t id, wl_fixed_t x,
                            wl_fixed_t y) {
    struct wayland_client_state *client_state = data;
    struct touch_point *point = get_touch_point(client_state, id);
    if (point == NULL) {
        return;
    }
    point->event_mask |= TOUCH_EVENT_MOTION;
    point->surface_x = x, point->surface_y = y;
    client_state->touch_event.time = time;
}

static void wl_touch_cancel(void *data, struct wl_touch *wl_touch) {
    struct wayland_client_state *client_state = data;
    client_state->touch_event.event_mask |= TOUCH_EVENT_CANCEL;
}

static void wl_touch_shape(void *data, struct wl_touch *wl_touch, int32_t id,
                           wl_fixed_t major, wl_fixed_t minor) {
    struct wayland_client_state *client_state = data;
    struct touch_point *point = get_touch_point(client_state, id);
    if (point == NULL) {
        return;
    }
    point->event_mask |= TOUCH_EVENT_SHAPE;
    point->major = major, point->minor = minor;
}

static void wl_touch_orientation(void *data, struct wl_touch *wl_touch,
                                 int32_t id, wl_fixed_t orientation) {
    struct wayland_client_state *client_state = data;
    struct touch_point *point = get_touch_point(client_state, id);
    if (point == NULL) {
        return;
    }
    point->event_mask |= TOUCH_EVENT_ORIENTATION;
    point->orientation = orientation;
}

static void wl_touch_frame(void *data, struct wl_touch *wl_touch) {
    struct wayland_client_state *client_state = data;
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
    struct wayland_client_state *state = data;

    bool have_pointer = capabilities & WL_SEAT_CAPABILITY_POINTER;

    if (have_pointer && state->wl_pointer == NULL) {
        state->wl_pointer = wl_seat_get_pointer(state->wl_seat);
        wl_pointer_add_listener(state->wl_pointer, &wl_pointer_listener, state);
    } else if (!have_pointer && state->wl_pointer != NULL) {
        wl_pointer_release(state->wl_pointer);
        state->wl_pointer = NULL;
    }

    bool have_keyboard = capabilities & WL_SEAT_CAPABILITY_KEYBOARD;

    if (have_keyboard && state->wl_keyboard == NULL) {
        state->wl_keyboard = wl_seat_get_keyboard(state->wl_seat);
        wl_keyboard_add_listener(state->wl_keyboard, &wl_keyboard_listener,
                                 state);
    } else if (!have_keyboard && state->wl_keyboard != NULL) {
        wl_keyboard_release(state->wl_keyboard);
        state->wl_keyboard = NULL;
    }

    bool have_touch = capabilities & WL_SEAT_CAPABILITY_TOUCH;

    if (have_touch && state->wl_touch == NULL) {
        state->wl_touch = wl_seat_get_touch(state->wl_seat);
        wl_touch_add_listener(state->wl_touch, &wl_touch_listener, state);
    } else if (!have_touch && state->wl_touch != NULL) {
        wl_touch_release(state->wl_touch);
        state->wl_touch = NULL;
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

static void registry_global_event_handler(void *data,
                                          struct wl_registry *registry,
                                          uint32_t name,
                                          const char *interface_string,
                                          uint32_t version) {
    wayland_client_state *state = data;
    if (strcmp(interface_string, wl_compositor_interface.name) == 0) {
        state->compositor =
            wl_registry_bind(registry, name, &wl_compositor_interface, 6);
    } else if (strcmp(interface_string, wl_shm_interface.name) == 0) {
        state->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    } else if (strcmp(interface_string, xdg_wm_base_interface.name) == 0) {
        state->xdg_wm_base =
            wl_registry_bind(registry, name, &xdg_wm_base_interface, 6);
        xdg_wm_base_add_listener(state->xdg_wm_base, &xdg_wm_base_listener,
                                 state);
    } else if (strcmp(interface_string, wl_seat_interface.name) == 0) {
        state->wl_seat =
            wl_registry_bind(registry, name, &wl_seat_interface, 9);
        wl_seat_add_listener(state->wl_seat, &wl_seat_listener, state);
    } else if (strcmp(interface_string,
                      zxdg_decoration_manager_v1_interface.name) == 0) {
        state->xdg_decoration_manager = wl_registry_bind(
            registry, name, &zxdg_decoration_manager_v1_interface, 1);
    }
}

static void registry_global_remove(void *data, struct wl_registry *registry,
                                   uint32_t name) {
    // handle this
}

static const struct wl_registry_listener registry_listener = {
    .global = registry_global_event_handler,
    .global_remove = registry_global_remove,
};

int main(int argc, char **argv) {
    wayland_client_state interfaces = {0};
    interfaces.width = 640;
    interfaces.height = 480;

    interfaces.display = wl_display_connect(NULL);
    if (!interfaces.display) {
        verbose("Failed to connect to the wayland compositor!\n");
        return 1;
    }
    interfaces.registry = wl_display_get_registry(interfaces.display);
    interfaces.xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    wl_registry_add_listener(interfaces.registry, &registry_listener,
                             &interfaces);
    wl_display_roundtrip(interfaces.display);

    interfaces.wl_surface = wl_compositor_create_surface(interfaces.compositor);
    interfaces.xdg_surface = xdg_wm_base_get_xdg_surface(interfaces.xdg_wm_base,
                                                         interfaces.wl_surface);

    xdg_surface_add_listener(interfaces.xdg_surface, &xdg_surface_listener,
                             &interfaces);
    interfaces.xdg_toplevel = xdg_surface_get_toplevel(interfaces.xdg_surface);
    xdg_toplevel_add_listener(interfaces.xdg_toplevel,
                              &xdg_toplevel_events_listener, &interfaces);
    xdg_toplevel_set_title(interfaces.xdg_toplevel, "hello wayland");
    interfaces.xdg_toplevel_decoration =
        zxdg_decoration_manager_v1_get_toplevel_decoration(
            interfaces.xdg_decoration_manager, interfaces.xdg_toplevel);
    wl_surface_commit(interfaces.wl_surface);

    struct wl_callback *cb = wl_surface_frame(interfaces.wl_surface);
    wl_callback_add_listener(cb, &wl_surface_frame_listener, &interfaces);

    interfaces.shouldClose = false;
    while (!interfaces.shouldClose) {

        wl_display_dispatch(
            interfaces.display); // Waits for next event (Blocking)
        // wl_display_dispatch_pending(interfaces.display); // Non-blocking
        // event polling
    }

    zxdg_toplevel_decoration_v1_destroy(interfaces.xdg_toplevel_decoration);
    zxdg_decoration_manager_v1_destroy(interfaces.xdg_decoration_manager);
    xdg_toplevel_destroy(interfaces.xdg_toplevel);
    xdg_surface_destroy(interfaces.xdg_surface);
    xdg_wm_base_destroy(interfaces.xdg_wm_base);
    wl_surface_destroy(interfaces.wl_surface);
    if (interfaces.wl_keyboard != NULL) {
        wl_keyboard_release(interfaces.wl_keyboard);
    }
    if (interfaces.wl_pointer != NULL) {
        wl_pointer_release(interfaces.wl_pointer);
    }
    if (interfaces.wl_touch != NULL) {
        wl_touch_release(interfaces.wl_touch);
    }
    wl_seat_release(interfaces.wl_seat);
    wl_display_disconnect(interfaces.display);
    verbose("All resources freed!\n");
    verbose("Quiting program!\n");
    return 0;
}
