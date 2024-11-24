#ifndef WAYLAND_INPUT_DEVICE_DISCOVERY_LISTENER
#define WAYLAND_INPUT_DEVICE_DISCOVERY_LISTENER

#include "../callbacks/input-device.h"

static const struct wl_seat_listener wl_seat_listener = {
    .capabilities = wl_seat_capabilities,
    .name = wl_seat_name,
};

#endif // ! WAYLAND_INPUT_DEVICE_DISCOVERY_LISTENER
