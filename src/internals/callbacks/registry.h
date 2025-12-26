#ifndef REGISTRY_HANDLER_CALLBACKS
#define REGISTRY_HANDLER_CALLBACKS

#include "../../wlClientState.h"
#include <string.h>
#include <wayland-client-protocol.h>

#include "../protocols/pointer-constraints-protocol.h"
#include "../protocols/relative-pointer-protocol.h"
#include "../protocols/xdg-decoration-protocol.h"
#include "../protocols/xdg-shell-client-protocol.h"

#include "../listeners/input-devices.h"
#include "../listeners/xdg-events/wmbase.h"

static void registry_global_event_handler(void *data,
                                          struct wl_registry *registry,
                                          uint32_t name,
                                          const char *interface_string,
                                          uint32_t version) {
    WaylandClientContext *state = (WaylandClientContext *)data;
    if (strcmp(interface_string, wl_compositor_interface.name) == 0) {
        state->compositor = (struct wl_compositor *)wl_registry_bind(
            registry, name, &wl_compositor_interface, version);
    } /*else if (strcmp(interface_string, wl_shm_interface.name) == 0) {
        state->shm = (struct wl_shm *)wl_registry_bind(
            registry, name, &wl_shm_interface, version);
    }*/
    else if (strcmp(interface_string, xdg_wm_base_interface.name) == 0) {
        state->xdg_wm_base = (struct xdg_wm_base *)wl_registry_bind(
            registry, name, &xdg_wm_base_interface, version);
        xdg_wm_base_add_listener(state->xdg_wm_base, &xdg_wm_base_listener,
                                 state);
    } else if (strcmp(interface_string, wl_seat_interface.name) == 0) {
        state->wl_seat = (struct wl_seat *)wl_registry_bind(
            registry, name, &wl_seat_interface, version);
        wl_seat_add_listener(state->wl_seat, &wl_seat_listener, state);
    } else if (strcmp(interface_string,
                      zxdg_decoration_manager_v1_interface.name) == 0) {
        state->xdg_decoration_manager =
            (struct zxdg_decoration_manager_v1 *)wl_registry_bind(
                registry, name, &zxdg_decoration_manager_v1_interface, version);
    } else if (strcmp(interface_string,
                      zwp_relative_pointer_manager_v1_interface.name) == 0) {
        state->relative_pointer_manager =
            (struct zwp_relative_pointer_manager_v1 *)wl_registry_bind(
                registry, name, &zwp_relative_pointer_manager_v1_interface,
                version);
    } else if (strcmp(interface_string,
                      zwp_pointer_constraints_v1_interface.name) == 0) {
        state->pointer_constraint_manager =
            (struct zwp_pointer_constraints_v1 *)wl_registry_bind(
                registry, name, &zwp_pointer_constraints_v1_interface, version);
    }
}

static void registry_global_remove(void *data, struct wl_registry *registry,
                                   uint32_t name) {
    // handle this
    verbose("\nregistry global remove event fired!");
}

#endif // ! REGISTRY_HANDLER_CALLBACKS
