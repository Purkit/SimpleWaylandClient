#ifndef WAYLAND_WM_BASE_RELATED_EVENT_LISTENERS
#define WAYLAND_WM_BASE_RELATED_EVENT_LISTENERS

#include "../../callbacks/xdg-events/wmbase.h"

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping_handler,
};

#endif // ! WAYLAND_WM_BASE_RELATED_EVENT_LISTENERS
