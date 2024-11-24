#ifndef WAYLAND_WM_BASE_RELATED_EVENT_CALLBACKS
#define WAYLAND_WM_BASE_RELATED_EVENT_CALLBACKS

#include "../../protocols/xdg-shell-client-protocol.h"

static void xdg_wm_base_ping_handler(void *data,
                                     struct xdg_wm_base *xdg_wm_base,
                                     uint32_t serial) {
    xdg_wm_base_pong(xdg_wm_base, serial);
}

#endif // ! WAYLAND_WM_BASE_RELATED_EVENT_CALLBACKS
