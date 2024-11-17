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

#include "input/callbacks/keyboard.h"
#include "input/callbacks/mouse.h"
#include "input/callbacks/touch.h"

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

static const struct wl_keyboard_listener wl_keyboard_listener = {
    .keymap = wl_keyboard_keymap,
    .enter = wl_keyboard_enter,
    .leave = wl_keyboard_leave,
    .key = wl_keyboard_key,
    .modifiers = wl_keyboard_modifiers,
    .repeat_info = wl_keyboard_repeat_info,
};

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
