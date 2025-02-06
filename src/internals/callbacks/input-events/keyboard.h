#ifndef KEYBOARD_CALLBACKS
#define KEYBOARD_CALLBACKS

#include "../../../utility.h"
#include "../../../wlClientState.h"
#include "../../internal_api.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>
#include <xkbcommon/xkbcommon.h>

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
    client_state->xkb_state  = xkb_state;
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

    const KeyCode key_code = _getOurKeyCode_from_xkb_keysym(
        xkb_state_key_get_one_sym(client_state->xkb_state, key + 8));
    const KeyState key_state =
        (state == WL_KEYBOARD_KEY_STATE_PRESSED ? PRESSED : UNPRESSED);
    _registerKeyState(client_state, key_code, key_state);

    char buf[2];
    if (xkb_state_key_get_utf8(client_state->xkb_state, key + 8, buf,
                               sizeof(buf)) != 0) {
        // TODO: call get_character callback from here
        verbose("utf8: '%s'\n", buf);
    }
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
    WaylandClientContext *client_state = (WaylandClientContext *)data;

    client_state->keyRepeat_rate  = rate;
    client_state->keyRepeat_delay = delay;
}

#endif // KEYBOARD_CALLBACKS
