#ifndef XDG_SURFACE_RELATED_EVENT_CALLBACKS
#define XDG_SURFACE_RELATED_EVENT_CALLBACKS

#include "../../../wlClientState.h"
#include "../../protocols/xdg-shell-client-protocol.h"

static void xdg_surface_configure_event_handler(void *data,
                                                struct xdg_surface *xdg_surface,
                                                uint32_t serial) {
    struct WaylandClientContext *state = (struct WaylandClientContext *)data;
    xdg_surface_ack_configure(xdg_surface, serial);

    wl_surface_commit(state->wl_surface);
}

#endif // ! XDG_SURFACE_RELATED_EVENT_CALLBACKS
