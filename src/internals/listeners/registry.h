#ifndef WAYLAND_REGISTRY_LISTENERS
#define WAYLAND_REGISTRY_LISTENERS

#include "../callbacks/registry.h"

static const struct wl_registry_listener registry_listener = {
    .global = registry_global_event_handler,
    .global_remove = registry_global_remove,
};
#endif // !WAYLAND_REGISTRY_LISTENERS
