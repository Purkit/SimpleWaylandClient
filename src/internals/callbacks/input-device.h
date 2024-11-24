#ifndef WAYLAND_INPUT_DEVICE_DISCOVERY_CAllBACKS
#define WAYLAND_INPUT_DEVICE_DISCOVERY_CAllBACKS

#include "../../wlClientState.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>
#include <xkbcommon/xkbcommon.h>

#include "../listeners/input-events.h"

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
    // do nothing !
}

#endif // ! WAYLAND_INPUT_DEVICE_DISCOVERY_LISTENER
